
#include "webbie/server_response.h"
#include "webbie/http_exception.h"
#include "cppkit/ck_socket.h"
#include "cppkit/ck_string_utils.h"

#include <ctime>

using namespace webbie;
using namespace cppkit;
using namespace std;

server_response::server_response(status_code status, const string& contentType) :
    _status(status),
    _connectionClose(true),
    _contentType(contentType),
    _body(),
    _headerWritten(false),
    _additionalHeaders(),
    _responseWritten(false)
{
}

server_response::server_response(const server_response& obj) :
    _status(obj._status),
    _connectionClose(obj._connectionClose),
    _contentType(obj._contentType),
    _body(obj._body),
    _headerWritten(obj._headerWritten),
    _additionalHeaders(obj._additionalHeaders),
    _responseWritten(obj._responseWritten)
{
}

server_response::~server_response() noexcept
{
}

server_response& server_response::operator = (const server_response& obj)
{
    _status = obj._status;
    _connectionClose = obj._connectionClose;
    _contentType = obj._contentType;
    _body = obj._body;
    _headerWritten = obj._headerWritten;
    _additionalHeaders = obj._additionalHeaders;
    _responseWritten = obj._responseWritten;

    return *this;
}

void server_response::set_status_code(status_code status)
{
    _status = status;
}

status_code server_response::get_status_code() const
{
    return _status;
}

void server_response::set_content_type(const string& contentType)
{
    _contentType = contentType;
}

string server_response::get_content_type() const
{
    return _contentType;
}

void server_response::set_body(vector<uint8_t>&& body)
{
    _body = std::move(body);
}

void server_response::set_body(const string& body)
{
    set_body( body.length(), body.c_str() );
}

void server_response::set_body( size_t bodySize, const void* bits )
{
    _body.resize( bodySize );
    memcpy( &_body[0], bits, bodySize );
}

size_t server_response::get_body_size() const
{
    return _body.size();
}

const void* server_response::get_body() const
{
    return &_body[0];
}

string server_response::get_body_as_string() const
{
    return string((char*)&_body[0], _body.size());
}

void server_response::clear_additional_headers()
{
    _additionalHeaders.clear();
}

void server_response::add_additional_header(const string& headerName,
                                            const string& headerValue)
{
    auto found = _additionalHeaders.find( headerName );
    if( found != _additionalHeaders.end() )
        _additionalHeaders.erase(found);

    _additionalHeaders.insert( make_pair( headerName, headerValue) );
}

string server_response::get_additional_header(const string& headerName)
{
    auto found = _additionalHeaders.find( headerName );
    if( found != _additionalHeaders.end() )
        return (*found).second;

    return string();
}

void server_response::write_response(ck_stream_io& socket)
{
    _responseWritten = true;

    time_t now = time(0);
    char* cstr = ctime(&now);

    if( cstr == nullptr )
        CK_STHROW(webbie_exception_generic, ("Please set Content-Type: before calling write_response()."));

    // RStrip to remove \n added by ctime
    string timeString = ck_string_utils::rstrip(string(cstr));

    if( (_body.size() > 0) && (_contentType.length() <= 0) )
        CK_STHROW(webbie_exception_generic, ("Please set Content-Type: before calling write_response()."));

    string responseHeader = ck_string_utils::format("HTTP/1.1 %d %s\r\nDate: %s\r\n",
                                             _status,
                                             _get_status_message(_status).c_str(),
                                             timeString.c_str() );

    if( _connectionClose )
        responseHeader += string("connection: close\r\n");

    if( _contentType.length() > 0 )
        responseHeader += ck_string_utils::format( "Content-Type: %s\r\n",
                                            _contentType.c_str() );

    responseHeader += ck_string_utils::format("Content-Length: %d\r\n", _body.size());

    auto i = _additionalHeaders.begin(), end = _additionalHeaders.end();
    while( i != end )
    {
        responseHeader += ck_string_utils::format("%s: %s\r\n", (*i).first.c_str(), (*i).second.c_str());
        i++;
    }

    responseHeader += ck_string_utils::format("\r\n");

    socket.send(responseHeader.c_str(), responseHeader.length());
    if( !socket.valid() )
        CK_STHROW( webbie_io_exception, ("Socket invalid."));

    if(_body.size() > 0)
    {
        socket.send(&_body[0], _body.size());
        if( !socket.valid() )
            CK_STHROW( webbie_io_exception, ("Socket invalid."));
    }
}

void server_response::write_chunk(ck_stream_io& socket, size_t sizeChunk, const void* bits)
{
    _responseWritten = true;

    if(!_headerWritten)
        _write_header(socket);

    auto chunkSizeString = ck_string_utils::format("%s;\r\n", ck_string_utils::format("%x", (unsigned int)sizeChunk).c_str());
    socket.send(chunkSizeString.c_str(), chunkSizeString.length());
    if( !socket.valid() )
        CK_STHROW( webbie_io_exception, ("Socket invalid."));

    socket.send(bits, sizeChunk);
    if( !socket.valid() )
        CK_STHROW( webbie_io_exception, ("Socket invalid."));

    string newLine("\r\n");
    socket.send(newLine.c_str(), newLine.length());
    if( !socket.valid() )
        CK_STHROW( webbie_io_exception, ("Socket invalid."));
}

void server_response::write_chunk_finalizer(ck_stream_io& socket)
{
    string finalizer("0\r\n\r\n");
    socket.send(finalizer.c_str(), finalizer.length());
    if( !socket.valid() )
        CK_STHROW( webbie_io_exception, ("Socket invalid."));
}

void server_response::write_part(ck_stream_io& socket,
                                   const string& boundary,
                                   const map<string,string>& partHeaders,
                                   void* chunk,
                                   uint32_t size)
{
    _responseWritten = true;

    auto boundaryLine = ck_string_utils::format("--%s\r\n", boundary.c_str());
    socket.send(boundaryLine.c_str(), boundaryLine.length());
    if( !socket.valid() )
        CK_STHROW( webbie_io_exception, ("Socket invalid."));

    for( auto i = partHeaders.begin(); i != partHeaders.end(); i++ )
    {
        string headerName = (*i).first;
        string headerValue = (*i).second;
        string headerLine = ck_string_utils::format("%s: %s\r\n",headerName.c_str(),headerValue.c_str());

        socket.send(headerLine.c_str(), headerLine.length());
        if( !socket.valid() )
            CK_STHROW( webbie_io_exception, ("Socket invalid."));
    }

    auto contentLength = ck_string_utils::format("Content-Length: %d\r\n", size);
    socket.send(contentLength.c_str(), contentLength.length());
    if( !socket.valid() )
        CK_STHROW( webbie_io_exception, ("Socket invalid."));

    string newLine("\r\n");
    socket.send(newLine.c_str(), newLine.length());
    if( !socket.valid() )
        CK_STHROW( webbie_io_exception, ("Socket invalid."));

    socket.send(chunk, size);
    if( !socket.valid() )
        CK_STHROW( webbie_io_exception, ("Socket invalid."));

    socket.send(newLine.c_str(), newLine.length());
    if( !socket.valid() )
        CK_STHROW( webbie_io_exception, ("Socket invalid."));
}

void server_response::write_part_finalizer(ck_stream_io& socket, const string& boundary)
{
    auto finalizerLine = ck_string_utils::format("--%s--\r\n", boundary.c_str());
    socket.send(finalizerLine.c_str(), finalizerLine.length());
    if( !socket.valid() )
        CK_STHROW( webbie_io_exception, ("Socket invalid."));
}

string server_response::_get_status_message(status_code sc) const
{
    switch(sc)
    {
    case response_continue:
        return string("Continue");

    case response_switching_protocols:
        return string("Switching Protocols");

    case response_ok:
        return string("OK");

    case response_created:
        return string("Created");

    case response_accepted:
        return string("Accepted");

    case response_no_content:
        return string("No Content");

    case response_reset_content:
        return string("Reset Content");

    case response_bad_request:
        return string("Bad Request");

    case response_unauthorized:
        return string("Unauthorized");

    case response_forbidden:
        return string("Forbidden");

    case response_not_found:
        return string("Not Found");

    case response_internal_server_error:
        return string("Internal Server Error");

    case response_not_implemented:
        return string("Not Implemented");

    default:
        break;
    };

    CK_STHROW(webbie_exception_generic, ("Unknown status code."));
}

bool server_response::_write_header(ck_stream_io& socket)
{
    time_t now = time(0);
    char* cstr = ctime(&now);

    if( cstr == nullptr )
        CK_STHROW(webbie_exception_generic, ("Please set Content-Type: before calling WriteResponse()."));

    // RStrip to remove \n added by ctime
    string timeString = ck_string_utils::rstrip(string(cstr));

    if(_contentType.length() <= 0)
        CK_STHROW(webbie_exception_generic, ("Please set Content-Type: before calling WriteChunk()."));

    string responseHeader = ck_string_utils::format("HTTP/1.1 %d %s\r\nDate: %s\r\nContent-Type: %s\r\nTransfer-Encoding: chunked\r\n",
                                             _status,
                                             _get_status_message(_status).c_str(),
                                             timeString.c_str(),
                                             _contentType.c_str());

    for( auto h : _additionalHeaders )
    {
        responseHeader += ck_string_utils::format("%s: %s\r\n",h.first.c_str(), h.second.c_str());
    }

    responseHeader += ck_string_utils::format("\r\n");

    socket.send(responseHeader.c_str(), responseHeader.length());
    if( !socket.valid() )
        CK_STHROW( webbie_io_exception, ("Socket invalid."));

    _headerWritten = true;

    return true;
}
