
#include "webbie/client_response.h"
#include "webbie/status_codes.h"
#include "webbie/http_exception.h"
#include "webbie/utils.h"
#include "cppkit/ck_socket.h"
#include "cppkit/ck_string_utils.h"

#include <iostream>

using namespace webbie;
using namespace cppkit;
using namespace std;

static const unsigned int MAX_HEADER_LINE = 2048;

client_response::client_response() :
    _initialLine(),
    _headerParts(),
    _bodyContents(),
    _success(false),
    _statusCode(-1),
    _chunkCallback(),
    _partCallback(),
    _chunk(),
    _streaming(false)
{
}

client_response::client_response(const client_response& rhs)
    : _initialLine(rhs._initialLine),
      _headerParts(rhs._headerParts),
      _bodyContents(rhs._bodyContents),
      _success(rhs._success),
      _statusCode(-1),
      _chunkCallback(rhs._chunkCallback),
      _partCallback(rhs._partCallback),
      _chunk(rhs._chunk),
      _streaming(rhs._streaming)
{
}

client_response::~client_response() noexcept
{
}

client_response& client_response::operator = (const client_response& rhs)
{
    if(&rhs != this)
    {
        _initialLine = rhs._initialLine;
        _headerParts = rhs._headerParts;
        _bodyContents = rhs._bodyContents;
        _success = rhs._success;
        _statusCode = -1;
        _chunkCallback = rhs._chunkCallback;
        _partCallback = rhs._partCallback;
        _chunk = rhs._chunk;
        _streaming = rhs._streaming;
    }
    return *this;
}

void client_response::read_response(cppkit::ck_stream_io& socket)
{
READ_BEGIN:
    list<string> requestLines;

    {
        char lineBuf[MAX_HEADER_LINE+1];
        memset(lineBuf, 0, MAX_HEADER_LINE+1);

        {
            char* writer = &lineBuf[0];

            // Get initial header line
            _read_header_line(socket, writer, true);
        }

        _initialLine = ck_string_utils::strip_eol(string(lineBuf));

        /// Now, read the rest of the header lines...
        do
        {
            memset(lineBuf, 0, MAX_HEADER_LINE+1);
            _read_header_line(socket, lineBuf, false);

        } while(!_add_line(requestLines, lineBuf));
    }

    /// Now, populate our header hash...

    _headerParts.clear();

    const vector<string> initialLineParts = ck_string_utils::split(_initialLine, ' ');

    if(initialLineParts.size() <= 2)
        CK_STHROW(webbie_exception_generic, ("HTTP request initial line doesn't have enough parts."));

    _add_header(string("http_version"), initialLineParts[0]);
    _add_header(string("response_code"), initialLineParts[1]);

    // After response code, we have a message, usually either "OK" or "Not Found", this code appends all the initial line
    // pieces after the first two parts so that we end up with a complete "message".

    string msg = initialLineParts[2];

    for(int i = 3, e = (int)initialLineParts.size(); i < e; ++i)
    {
        msg += " ";
        msg += initialLineParts[i];
    }

    _add_header(string("message"), msg);

    auto i = _headerParts.find( "response_code" );
    if( i != _headerParts.end() )
    {
        _statusCode = ck_string_utils::s_to_int(i->second.front());
        if(response_ok <= _statusCode && _statusCode < response_multiple_choices)
            _success = true;
    }

    // Handling a "100 continue" initial line, as per http 1.1 spec; we basically
    // just restart... A 100 continue means another complete header and body follows...
    if(ck_string_utils::contains(ck_string_utils::to_lower(msg), "continue"))
        goto READ_BEGIN;

    _process_request_lines(requestLines);
    _process_body(socket);
}

void client_response::_clean_socket(ck_stream_io& socket, char** writer)
{
    if(!socket.valid())
        CK_STHROW(webbie_exception_generic, ("Invalid Socket"));

    char tempBuffer[1];

    // Clear junk off the socket
    while(true)
    {
        socket.recv(tempBuffer, 1);

        if(!ck_string_utils::is_space(tempBuffer[0]))
        {
            **writer = tempBuffer[0];
            ++*writer;
            break;
        }
    }
}

void client_response::_read_header_line(ck_stream_io& socket, char* writer, bool firstLine)
{
    bool lineDone = false;
    size_t bytesReadThisLine = 0;

    bool maybeTerminator = true;

    // Get initial header line
    while(!lineDone && (bytesReadThisLine + 1) < MAX_HEADER_LINE)
    {
        socket.recv(writer, 1);

        ++bytesReadThisLine;

        auto justRead = *writer;
        ++writer;

        if(justRead != '\r' && justRead != '\n')
            maybeTerminator = false;

        if((!maybeTerminator && (justRead == '\n')) || (maybeTerminator && (justRead == '\n')))
            lineDone = true;
    }

    if(!lineDone)
    {
        string msg = firstLine ? "The HTTP initial request line exceeded our max."
                                : "The HTTP line exceeded our max.";

        CK_STHROW(webbie_exception_generic, (msg));
    }
}

bool client_response::_add_line(std::list<string>& lines, const string& line)
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

void client_response::_process_request_lines(const list<string>& requestLines)
{
    // Now, iterate on the header lines...

    for(list<string>::const_iterator iter = requestLines.begin(), end = requestLines.end();
        iter != end;
        ++iter)
    {
        const size_t firstColon = iter->find(':');

        if(firstColon != string::npos)
        {
            const string key = iter->substr(0, firstColon);
            const string val = firstColon + 1 < iter->size() ? iter->substr(firstColon + 1) : "";

            _add_header(ck_string_utils::to_lower(key), ck_string_utils::strip_eol(val));
        }
    }
}

void client_response::_process_body(ck_stream_io& socket)
{
    /// Get the body if we were given a Content Length
    auto found = _headerParts.find( "content-length" );
    if( found != _headerParts.end() )
    {
        const uint32_t contentLength = ck_string_utils::s_to_uint32((*found).second.front());

        if(contentLength > 0)
        {
            _bodyContents.resize( contentLength );
            socket.recv(&_bodyContents[0], contentLength);
        }
    }
    else if( (found = _headerParts.find( "transfer-encoding" )) != _headerParts.end() )
    {
        if(ck_string_utils::contains(ck_string_utils::to_lower((*found).second.front()), "chunked"))
        {
            _read_chunked_body(socket);
            return;
        }
    }

    if( (found = _headerParts.find("content-type")) != _headerParts.end() )
    {
        if(ck_string_utils::contains(ck_string_utils::to_lower((*found).second.front()), "multipart"))
        {
            _read_multi_part(socket);
            return;
        }
    }

    if(ck_string_utils::contains(ck_string_utils::to_lower(get_header("content-type")), "multipart"))
        _read_multi_part(socket);
}

vector<uint8_t> client_response::release_body()
{
    return std::move(_bodyContents);
}

const void* client_response::get_body() const
{
    return &_bodyContents[0];
}

size_t client_response::get_body_size() const
{
    return _bodyContents.size();
}

string client_response::get_body_as_string() const
{
    return string((char*)&_bodyContents[0], _bodyContents.size());
}

string client_response::get_header(const string& name) const
{
    auto values = _headerParts.find( ck_string_utils::to_lower(name) );

    return (values != _headerParts.end()) ? (*values).second.front() : "";
}

vector<string> client_response::get_all_matching_headers(const string& header) const
{
    vector<string> matchingHeaders;

    auto matches = _headerParts.find(ck_string_utils::to_lower(header));
    if( matches != _headerParts.end() )
    {
        for( auto i = (*matches).second.begin(); i != (*matches).second.end(); ++i )
            matchingHeaders.push_back( *i );
    }

    return matchingHeaders;
}

string client_response::get_message()
{
    return get_header("message");
}

bool client_response::is_success() const
{
    return _success;
}

bool client_response::is_failure() const
{
    return !_success;
}

void client_response::register_chunk_callback( chunk_callback cb, bool streaming )
{
    _chunkCallback = cb;
    _streaming = streaming;
}

void client_response::register_part_callback( part_callback pb )
{
    _partCallback = pb;
}

void client_response::_read_chunked_body(ck_stream_io& socket)
{
    char lineBuf[MAX_HEADER_LINE+1];
    bool moreChunks = true;

    while(moreChunks)
    {
        memset(lineBuf, 0, MAX_HEADER_LINE+1);

        _read_header_line(socket, lineBuf, false);

        if(lineBuf[0] == '0')
        {
            _consume_footer(socket);
            return;
        }

        if(_embed_null(lineBuf))
        {
            const string chunkLenS = lineBuf;

            uint32_t chunkLen;
            sscanf(chunkLenS.c_str(), "%x", &chunkLen);

            // We read our chunk into a temporary "chunk" ck_memory object, we then optionally
            // call our "chunk callback" function... Finally, we copy the new chunk into the
            // main body contents object.

            _chunk.resize( chunkLen );
            socket.recv(&_chunk[0], chunkLen);

            // call callback here...
            if( _chunkCallback )
                _chunkCallback( _chunk, *this );

            // We only append a chunk to our "_bodyContents" if we are not streaming (
            // because "streams" potentially have no end, so an ck_memory that contains the
            // complete body contents would just grow forever).

            size_t oldSize = _bodyContents.size();

            _bodyContents.resize( oldSize + chunkLen );

            if(!_streaming)
                memcpy( &_bodyContents[oldSize], &_chunk[0], chunkLen);

            _read_end_of_line(socket);
        }
    }
}

bool client_response::_embed_null(char* lineBuf)
{
    // Scan forward in the chunk header line and embed a nullptr on the first non legal chunk size char.
    for(size_t i = 0; i < MAX_HEADER_LINE; ++i)
    {
        if(!_is_legal_chunk_size_char(lineBuf[i]))
        {
            lineBuf[i] = 0;
            return true;
        }
    }

    return false;
}

void client_response::_read_multi_part(ck_stream_io& socket)
{
    char lineBuf[MAX_HEADER_LINE+1];
    bool moreParts = true;

    while(moreParts)
    {
        // First, read the boundary line...
        memset(lineBuf, 0, MAX_HEADER_LINE);

        _read_header_line(socket, lineBuf, false);

        if(ck_string_utils::ends_with(ck_string_utils::strip_eol(string(lineBuf)), "--"))
            break;

        map<string,string> partHeaders = _read_multi_header_lines(socket, lineBuf);

        auto i = partHeaders.find( "content-length" );
        if( i != partHeaders.end() )
        {
            const int partContentLength = ck_string_utils::s_to_int((*i).second);

            _chunk.resize(partContentLength);

            socket.recv(&_chunk[0], partContentLength);

            // call callback here...
            if( _partCallback )
                _partCallback( _chunk, partHeaders, *this );

            _read_end_of_line(socket);
        }
        else
            CK_STHROW(webbie_exception_generic, ("Oops. Mime multipart without a Content-Length!"));
    }
}

void client_response::_read_end_of_line(ck_stream_io& socket)
{
    char lineEnd[2] = {0, 0};

    socket.recv(&lineEnd[0], 1);

    if(lineEnd[0] == '\r')
        socket.recv(&lineEnd[1], 1);

    if(!_is_end_of_line(lineEnd))
        CK_STHROW(webbie_exception_generic, ("A chunk line didn't end with appropriate terminator."));
}

map<string,string> client_response::_read_multi_header_lines(ck_stream_io& socket, char* lineBuf)
{
    std::list<string> partLines;

    /// Now, read the rest of the header lines...
    while(true)
    {
        memset(lineBuf, 0, MAX_HEADER_LINE);

        _read_header_line(socket, lineBuf, false);

        if(_add_line(partLines, lineBuf))
            break;
    }

    map<string,string> partHeaders;

    for(auto iter = partLines.begin(), end = partLines.end(); iter != end; ++iter)
    {
        auto lineParts = ck_string_utils::split(*iter, ':');
        partHeaders.insert( make_pair( adjust_header_name( lineParts[0] ), adjust_header_value( ck_string_utils::strip_eol(lineParts[1]) ) ) );
    }

    return partHeaders;
}

void client_response::debug_print_request()
{
    string method = get_header(string("method"));
    string response_code = get_header(string("response_code"));
    string http_version = get_header(string("http_version"));
    printf("method = %s\n", method.c_str());
    printf("response_code = %s\n", response_code.c_str());
    printf("http_version = %s\n", http_version.c_str());
    fflush(stdout);
}

void client_response::_consume_footer(ck_stream_io& socket)
{
    char lineBuf[MAX_HEADER_LINE+1];

    /// Now, read the rest of the header lines...
    while(true)
    {
        memset(lineBuf, 0, MAX_HEADER_LINE);

        _read_header_line(socket, lineBuf, false);

        if(_is_end_of_line(lineBuf))
            break;
    }
}

void client_response::_add_header(const string& name, const string& value)
{
    const string adjName = adjust_header_name(name);
    const string adjValue = adjust_header_value(value);

    auto found = _headerParts.find( adjName );
    if( found != _headerParts.end() )
        (*found).second.push_back( adjValue );
    else
    {
        list<string> values;
        values.push_back(adjValue);
        _headerParts.insert( make_pair( adjName, values ) );
    }
}
