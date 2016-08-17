
/// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-
/// webbie - http://www.cppkit.org
/// Copyright (c) 2013, Tony Di Croce
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without modification, are permitted
/// provided that the following conditions are met:
///
/// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and
///    the following disclaimer.
/// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
///    and the following disclaimer in the documentation and/or other materials provided with the
///    distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
/// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
/// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
/// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
/// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
/// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
/// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/// The views and conclusions contained in the software and documentation are those of the authors and
/// should not be interpreted as representing official policies, either expressed or implied, of the cppkit
/// Project.
/// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-

#include "webbie/server_response.h"
#include "webbie/webbie_exception.h"

#include "cppkit/ck_socket.h"
#include "cppkit/ck_string.h"

#include <ctime>

using namespace webbie;
using namespace cppkit;
using namespace std;

server_response::server_response(status_code status, const ck_string& contentType) :
    _status(status),
    _contentType(contentType),
    _body(),
    _headerWritten(false),
    _additionalHeaders()
{
}

server_response::server_response(const server_response& obj) :
    _status(obj._status),
    _contentType(obj._contentType),
    _body(obj._body),
    _headerWritten(obj._headerWritten),
    _additionalHeaders(obj._additionalHeaders)
{
}

server_response::~server_response() throw()
{
}

server_response& server_response::operator = (const server_response& obj)
{
    _status = obj._status;
    _contentType = obj._contentType;
    _body = obj._body;
    _headerWritten = obj._headerWritten;
    _additionalHeaders = obj._additionalHeaders;

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

void server_response::set_content_type(const ck_string& contentType)
{
    _contentType = contentType;
}

ck_string server_response::get_content_type() const
{
    return _contentType;
}

void server_response::set_body(const ck_string& body)
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

ck_string server_response::get_body_as_string() const
{
    return ck_string((char*)&_body[0], _body.size());
}

void server_response::clear_additional_headers()
{
    _additionalHeaders.clear();
}

void server_response::add_additional_header(const ck_string& headerName,
                                            const ck_string& headerValue)
{
    auto found = _additionalHeaders.find( headerName );
    if( found != _additionalHeaders.end() )
        _additionalHeaders.erase(found);

    _additionalHeaders.insert( make_pair( headerName, headerValue) );
}

ck_string server_response::get_additional_header(const ck_string& headerName)
{
    auto found = _additionalHeaders.find( headerName );
    if( found != _additionalHeaders.end() )
        return (*found).second;

    return ck_string();
}

bool server_response::write_response(shared_ptr<ck_stream_io> socket)
{
    time_t now = time(0);
    char* cstr = ctime(&now);

    if( cstr == NULL )
        CK_STHROW(webbie_exception, ("Please set Content-Type: before calling write_response()."));

    // RStrip to remove \n added by ctime
    ck_string timeString = ck_string(cstr).rstrip();

    if( (_body.size() > 0) && (_contentType.length() <= 0) )
        CK_STHROW(webbie_exception, ("Please set Content-Type: before calling write_response()."));

    ck_string responseHeader = ck_string::format("HTTP/1.1 %d %s\r\nDate: %s\r\n",
                                                 _status,
                                                 _get_status_message(_status).c_str(),
                                                 timeString.c_str() );

    if( _contentType.length() > 0 )
        responseHeader += ck_string::format( "Content-Type: %s\r\n",
                                             _contentType.c_str() );

    responseHeader += ck_string::format("Content-Length: %d\r\n", _body.size());

    auto i = _additionalHeaders.begin(), end = _additionalHeaders.end();
    while( i != end )
    {
        responseHeader += ck_string::format("%s: %s\r\n", (*i).first.c_str(), (*i).second.c_str());
        i++;
    }

    responseHeader += ck_string::format("\r\n");

    if(!_send_string(socket, responseHeader))
        return false;

    if(_body.size() > 0)
    {
        if(!_send_data(socket, &_body[0], _body.size()))
            return false;
    }

    return true;
}

bool server_response::write_chunk(shared_ptr<ck_stream_io> socket, size_t sizeChunk, const void* bits)
{
    if(!_headerWritten)
    {
        if(!_write_header(socket))
            return false;
    }

    const ck_string chunkSizeString = ck_string::format("%s;\r\n", ck_string::from_uint((unsigned int)sizeChunk, 16).c_str());

    if(!_send_string(socket, chunkSizeString))
        return false;

    if(!_send_data(socket, bits, sizeChunk))
        return false;

    if(!_send_string(socket, "\r\n"))
        return false;

    return true;
}

bool server_response::write_chunk_finalizer(shared_ptr<ck_stream_io> socket)
{
    return _send_string(socket, "0\r\n\r\n");
}

bool server_response::write_part(shared_ptr<cppkit::ck_stream_io> socket,
                                 const cppkit::ck_string& boundary,
                                 const map<string,ck_string>& partHeaders,
                                 void* chunk,
                                 uint32_t size)
{
    if(!_send_string(socket, ck_string::format("--%s\r\n", boundary.c_str())))
        return false;

    for( auto i = partHeaders.begin(); i != partHeaders.end(); i++ )
    {
        ck_string headerName = (*i).first;
        ck_string headerValue = (*i).second;
        ck_string headerLine = ck_string::format("%s: %s\r\n",headerName.c_str(),headerValue.c_str());

        if( !_send_string( socket, headerLine ) )
            return false;
    }

    if(!_send_string(socket, ck_string::format("Content-Length: %d\r\n", size)))
        return false;

    if(!_send_string(socket, "\r\n"))
        return false;

    if(!_send_data(socket, chunk, size))
        return false;

    if(!_send_string(socket, "\r\n"))
        return false;

    return true;
}

bool server_response::write_part_finalizer(shared_ptr<cppkit::ck_stream_io> socket, const cppkit::ck_string& boundary)
{
    return _send_string(socket, ck_string::format("--%s--\r\n", boundary.c_str()));
}

ck_string server_response::_get_status_message(status_code sc)
{
    switch(sc)
    {
    case response_continue:
        return ck_string("Continue");

    case response_switching_protocols:
        return ck_string("Switching Protocols");

    case response_ok:
        return ck_string("OK");

    case response_created:
        return ck_string("Created");

    case response_accepted:
        return ck_string("Accepted");

    case response_no_content:
        return ck_string("No Content");

    case response_reset_content:
        return ck_string("Reset Content");

    case response_bad_request:
        return ck_string("Bad Request");

    case response_unauthorized:
        return ck_string("Unauthorized");

    case response_forbidden:
        return ck_string("Forbidden");

    case response_not_found:
        return ck_string("Not Found");

    case response_internal_server_error:
        return ck_string("Internal Server Error");

    case response_not_implemented:
        return ck_string("Not Implemented");

    default:
        break;
    };

    CK_STHROW(webbie_exception, ("Unknown status code."));
}

bool server_response::_write_header(shared_ptr<ck_stream_io> socket)
{
    time_t now = time(0);
    char* cstr = ctime(&now);

    if( cstr == NULL )
        CK_STHROW(webbie_exception, ("Please set Content-Type: before calling WriteResponse()."));

    // RStrip to remove \n added by ctime
    ck_string timeString = ck_string(cstr).rstrip();

    if(_contentType.length() <= 0)
        CK_STHROW(webbie_exception, ("Please set Content-Type: before calling WriteChunk()."));

    ck_string responseHeader = ck_string::format("HTTP/1.1 %d %s\r\nDate: %s\r\nContent-Type: %s\r\nTransfer-Encoding: chunked\r\n",
                                                 _status,
                                                 _get_status_message(_status).c_str(),
                                                 timeString.c_str(),
                                                 _contentType.c_str());

    for( auto i = _additionalHeaders.begin(); i != _additionalHeaders.end(); i++ )
    {
        responseHeader += ck_string::format("%s: %s\r\n", (*i).first.c_str(), (*i).second.c_str());
    }

    responseHeader += ck_string::format("\r\n");

    if(!_send_string(socket, responseHeader))
        return false;

    _headerWritten = true;

    return true;
}

bool server_response::_send_string(shared_ptr<ck_stream_io> socket, const ck_string& line)
{
    return _send_data(socket, line.c_str(), line.size());
}

bool server_response::_send_data(shared_ptr<ck_stream_io> socket, const void* data, size_t dataLen)
{
    const ssize_t bytesSent = socket->send(data, dataLen);

    return socket->valid() && bytesSent == (ssize_t)dataLen;
}
