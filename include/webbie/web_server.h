
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

#ifndef webbie_web_server_h
#define webbie_web_server_h

#include "webbie/server_request.h"
#include "webbie/server_response.h"
#include "webbie/methods.h"
#include "webbie/status_codes.h"
#include "webbie/webbie_exception.h"
#include "cppkit/ck_server_threaded.h"
#include <functional>
#include <thread>
#include <map>
#include <string>

namespace webbie
{

template<class SOK_T>
class web_server;

template<class SOK_T>
class web_server final
{
public:
    typedef std::function<server_response(const web_server<SOK_T>& ws, cppkit::ck_buffered_socket<SOK_T>& conn, const server_request& request)> http_cb;

    CK_API web_server(int port, const cppkit::ck_string& sockAddr = cppkit::ck_string()) :
        _cbs(),
        _server(port, std::bind(&web_server<SOK_T>::_server_conn_cb, this, std::placeholders::_1), sockAddr),
        _serverThread()
    {
    }

    CK_API web_server(const web_server<SOK_T>&) = delete;

    CK_API ~web_server() throw()
    {
        stop();
    }

    CK_API web_server<SOK_T>& operator = (const web_server<SOK_T>&) = delete;

    CK_API void start()
    {
        _serverThread = std::thread(&cppkit::ck_server_threaded<SOK_T>::start, &_server);
    }

    CK_API void stop()
    {
        if( _server.started() )
        {
            _server.stop();
            _serverThread.join();
        }
    }

    CK_API void add_route( int method, const cppkit::ck_string& path, http_cb cb )
    {
        _cbs[method][path.to_std_string()] = cb;
    }

    CK_API SOK_T& get_socket() { return _server.get_socket(); }

private:
    void _server_conn_cb(cppkit::ck_buffered_socket<SOK_T>& conn)
    {
        server_request request;
        request.read_request(conn);

        uri ruri = request.get_uri();

        try
        {
            auto foundMethod = _cbs.find(request.get_method());

            if( foundMethod == _cbs.end() )
                CK_STHROW( http_404_exception, ("No routes match request method type.") );

            auto path = ruri.get_full_resource_path();

            auto foundRoute = foundMethod->second.end();

            for(auto i = foundMethod->second.begin(), e = foundMethod->second.end(); i != e; ++i)
            {
                if(path.to_std_string().compare(0, i->first.length(), i->first) == 0)
                    foundRoute = i;
            }

            if( foundRoute == foundMethod->second.end() )
                CK_STHROW( http_404_exception, ("Unable to found path: %s", path.c_str()) );

            auto response = foundRoute->second(*this, conn, request);

            if(!response.written() && conn.valid())
                response.write_response(conn);
        }
        catch(http_404_exception& ex)
        {
            CK_LOG_CPPKIT_EXCEPTION(ex);

            if(conn.valid())
            {
                server_response response;
                response.set_status_code(response_not_found);
                response.write_response(conn);
            }
        }
        catch(std::exception& ex)
        {
            // Note: we cannot write a response here because this exception
            // might indicate our sockets are not invalid simply because of
            // and incomplete io... but nevertheless our sockets might not
            // be able to communicate becuase the other side make have
            // broken its connection.
            CK_LOG_STD_EXCEPTION(ex);
        }
        catch(...)
        {
            // Note: we cannot write a response here because this exception
            // might indicate our sockets are not invalid simply because of
            // and incomplete io... but nevertheless our sockets might not
            // be able to communicate becuase the other side make have
            // broken its connection.

            CK_LOG_NOTICE("An unknown exception has occurred in our web server.");
        }
    }

    std::map<int, std::map<std::string, http_cb>> _cbs;
    cppkit::ck_server_threaded<SOK_T> _server;
    std::thread _serverThread;
};

}

#endif
