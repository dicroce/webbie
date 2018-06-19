
#include "webbie/server_request.h"
#include "webbie/client_request.h"
#include "webbie/http_exception.h"
#include "webbie/utils.h"
#include "webbie/methods.h"
#include "cppkit/ck_socket.h"
#include "cppkit/ck_string_utils.h"

using namespace webbie;
using namespace cppkit;
using namespace std;

static const unsigned int MAX_HEADER_LINE = 16384;

server_request::server_request() :
    _initialLine(),
    _headerParts(),
    _postVars(),
    _body(),
    _contentType()
{
}

server_request::server_request(const server_request& obj) :
    _initialLine(obj._initialLine),
    _headerParts(obj._headerParts),
    _postVars(obj._postVars),
    _body(obj._body),
    _contentType()
{
}

server_request::~server_request() noexcept
{
}

server_request& server_request::operator = (const server_request& obj)
{
    _initialLine = obj._initialLine;
    _headerParts = obj._headerParts;
    _postVars = obj._postVars;
    _body = obj._body;
    _contentType = obj._contentType;

    return *this;
}

void server_request::read_request(cppkit::ck_stream_io& socket)
{
    list<string> requestLines;

    {
        char lineBuf[MAX_HEADER_LINE+1];
        memset(lineBuf, 0, MAX_HEADER_LINE+1);

        {
            char* writer = &lineBuf[0];
            _clean_socket(socket, &writer);

            // Get initial header line
            _read_header_line(socket, writer, true);
        }

        _initialLine = ck_string_utils::strip_eol(string(lineBuf));

        /// Now, read the rest of the header lines...
        do
        {
            memset(lineBuf, 0, MAX_HEADER_LINE);
            _read_header_line(socket, lineBuf, false);

        } while(!_add_line(requestLines, lineBuf));
    }

    /// Now, populate our header hash...

    _headerParts.clear();

    const vector<string> initialLineParts = ck_string_utils::split(_initialLine, ' ');

    if(initialLineParts.size() != 3)
        CK_STHROW(webbie_exception_generic, ("HTTP request initial line exceeded 3 parts"));

    _set_header("method", initialLineParts[0]);
    _set_header("uri", initialLineParts[1]);
    _set_header("http_version", ck_string_utils::strip_eol(initialLineParts[2]));

    _process_request_lines(requestLines);

    const string method = ck_string_utils::to_lower(initialLineParts[0]);
    if( method == ck_string_utils::to_lower(method_text( METHOD_POST )) ||
        method == ck_string_utils::to_lower(method_text( METHOD_PUT )) ||
        method == ck_string_utils::to_lower(method_text( METHOD_PATCH )) ||
        method == ck_string_utils::to_lower(method_text( METHOD_DELETE )) )
        _process_body(socket);
}

bool server_request::is_patch_request() const
{
    return get_method() == METHOD_PATCH;
}

bool server_request::is_post_request() const
{
    return get_method() == METHOD_POST;
}

bool server_request::is_get_request() const
{
    return get_method() == METHOD_GET;
}

bool server_request::is_put_request() const
{
    return get_method() == METHOD_PUT;
}

bool server_request::is_delete_request() const
{
    return get_method() == METHOD_DELETE;
}

int server_request::get_method() const
{
    auto h = get_header("method");
    if(h.is_null())
        CK_STHROW(webbie_exception_generic, ("request has no method."));
    
    return method_type(h.value());
}

uri server_request::get_uri() const
{
    auto h = get_header("uri");
    if(h.is_null())
        CK_STHROW(webbie_exception_generic, ("request has no uri."));

    return uri(h.value());
}

string server_request::get_content_type() const
{
    return _contentType;
}

void server_request::_set_header(const string& name, const string& value)
{
    const string adjName = adjust_header_name(name);
    const string adjValue = adjust_header_value(value);

    _headerParts[adjName] = adjValue;
}

ck_nullable<string> server_request::get_header( const std::string& key ) const
{
    ck_nullable<string> result;

    auto found = _headerParts.find( ck_string_utils::to_lower(key) );
    if( found != _headerParts.end() )
        result.set_value(found->second);

    return result;
}

map<string,string> server_request::get_headers() const
{
    return _headerParts;
}

const uint8_t* server_request::get_body() const
{
    return &_body[0];
}

size_t server_request::get_body_size() const
{
    return _body.size();
}

string server_request::get_body_as_string() const
{
    return string((char*)&_body[0], _body.size());
}

map<string,string> server_request::get_post_vars() const
{
    return _postVars;
}

void server_request::_clean_socket(ck_stream_io& socket, char** writer)
{
    if(!socket.valid())
        CK_STHROW(webbie_exception_generic, ("Invalid Socket"));

    char tempBuffer[1];

    // Clear junk off the socket
    while(true)
    {
        socket.recv(tempBuffer, 1);
        if( !socket.valid() )
            CK_STHROW( webbie_io_exception, ("Socket invalid."));

        if(!ck_string_utils::is_space(tempBuffer[0]))
        {
            **writer = tempBuffer[0];
            ++*writer;
            break;
        }
    }
}

void server_request::_read_header_line(ck_stream_io& socket, char* writer, bool firstLine)
{
    bool lineDone = false;
    size_t bytesReadThisLine = 0;

    // Get initial header line
    while(!lineDone && bytesReadThisLine + 1 < MAX_HEADER_LINE)
    {
        socket.recv(writer, 1);
        if( !socket.valid() )
            CK_STHROW( webbie_io_exception, ("Socket invalid."));

        ++bytesReadThisLine;

        if(*writer == '\n')
            lineDone = true;

        ++writer;
    }

    if(!lineDone)
        CK_STHROW(webbie_exception_generic, ("The HTTP initial request line exceeded our max."));
}


bool server_request::_add_line(std::list<string>& lines, const string& line)
{
    if(ck_string_utils::starts_with(line, "\r\n") || ck_string_utils::starts_with(line, "\n"))
        return true;

    if(ck_string_utils::starts_with(line, " ") || ck_string_utils::starts_with(line, "\t"))
    {
        if(!lines.empty())
            lines.back() += line;
        else
            CK_STHROW(webbie_exception_generic, ("First line of header missing needed seperator."));
    }
    else
        lines.push_back(line);

    return false;
}

void server_request::_process_request_lines(const list<string>& requestLines)
{
    // Now, iterate on the header lines...

    for(auto iter = requestLines.begin(), end  = requestLines.end(); iter != end; ++iter)
    {
        const size_t firstColon = iter->find(':');

        if(firstColon != string::npos)
        {
            const string key = iter->substr(0, firstColon);
            const string val = firstColon + 1 < iter->size() ? iter->substr(firstColon + 1) : "";

            _set_header(key, ck_string_utils::strip_eol(val));
        }
    }
}

void server_request::_process_body(ck_stream_io& socket)
{
    auto cl = get_header("Content-Length");

    if(!cl.is_null())
    {
        auto contentLengthString = ck_string_utils::strip(cl.value());
        uint32_t contentLength = ck_string_utils::s_to_uint32(contentLengthString);

        if(!contentLength)
            return;

        _body.resize(contentLength);

        unsigned char* buffer = &_body[0];

        socket.recv(buffer, contentLength);
        if( !socket.valid() )
            CK_STHROW( webbie_io_exception, ("Socket invalid."));

        auto ct = get_header("Content-Type");

        if(!ct.is_null())
        {
            _contentType = ck_string_utils::lstrip(ct.value());

            if(ck_string_utils::contains(_contentType, "x-www-form-urlencoded"))
            {
                string rawBody((char*)&_body[0], (int)_body.size());

                vector<string> parts = ck_string_utils::split(rawBody, "&");

                for(size_t i = 0; i < parts.size(); ++i)
                {
                    string nvPair = parts[i];

                    vector<string> nameAndValue = ck_string_utils::split(nvPair, "=");

                    if(nameAndValue.size() == 2)
                        _postVars.insert( make_pair( (string)nameAndValue[0], nameAndValue[1] ) );
                }
            }
        }
    }
}
