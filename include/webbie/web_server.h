
#ifndef webbie_web_server_h
#define webbie_web_server_h

#include "webbie/server_request.h"
#include "webbie/server_response.h"
#include "webbie/methods.h"
#include "webbie/status_codes.h"
#include "webbie/http_exception.h"
#include "cppkit/ck_server_threaded.h"
#include "cppkit/ck_socket.h"
#include <functional>
#include <thread>
#include <map>
#include <string>

#define WS_CATCH(type, code) \
    catch(type& ex) \
    { \
        CK_LOG_EXCEPTION(ex); \
        if(conn.valid()) \
        { \
            server_response response; \
            response.set_status_code(code); \
            response.write_response(conn); \
        } \
    }

namespace webbie
{

template<class SOK_T>
class web_server;

template<class SOK_T>
class web_server final
{
public:
    typedef std::function<server_response(const web_server<SOK_T>& ws, cppkit::ck_buffered_socket<SOK_T>& conn, const server_request& request)> http_cb;

    web_server(int port, const std::string& sockAddr = std::string()) :
        _cbs(),
        _server(port, std::bind(&web_server<SOK_T>::_server_conn_cb, this, std::placeholders::_1), sockAddr),
        _serverThread()
    {
    }

    web_server(const web_server<SOK_T>&) = delete;

    ~web_server() noexcept
    {
        stop();
    }

    web_server<SOK_T>& operator = (const web_server<SOK_T>&) = delete;

    void start()
    {
        _serverThread = std::thread(&cppkit::ck_server_threaded<SOK_T>::start, &_server);
    }

    void stop()
    {
        if( _server.started() )
        {
            _server.stop();
            _serverThread.join();
        }
    }

    void add_route( int method, const std::string& path, http_cb cb )
    {
        _cbs[method][path] = cb;
    }

    SOK_T& get_socket() { return _server.get_socket(); }

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
                CK_STHROW( webbie_404_exception, ("No routes match request method type.") );

            auto path = ruri.get_full_resource_path();

            auto foundRoute = foundMethod->second.end();

            for(auto i = foundMethod->second.begin(), e = foundMethod->second.end(); i != e; ++i)
            {
                if(path.compare(0, i->first.length(), i->first) == 0)
                    foundRoute = i;
            }

            if( foundRoute == foundMethod->second.end() )
                CK_STHROW( webbie_404_exception, ("Unable to found path: %s", path.c_str()) );

            auto response = foundRoute->second(*this, conn, request);

            if(!response.written() && conn.valid())
                response.write_response(conn);
        }
        WS_CATCH(webbie_400_exception, response_bad_request)
        WS_CATCH(webbie_401_exception, response_unauthorized)
        WS_CATCH(webbie_403_exception, response_forbidden)
        WS_CATCH(webbie_404_exception, response_not_found)
        WS_CATCH(webbie_500_exception, response_internal_server_error)
        WS_CATCH(webbie_501_exception, response_not_implemented)
        WS_CATCH(cppkit::ck_not_found_exception, response_not_found)
        WS_CATCH(cppkit::ck_invalid_argument_exception, response_bad_request)
        WS_CATCH(cppkit::ck_unauthorized_exception, response_forbidden)
        WS_CATCH(cppkit::ck_not_implemented_exception, response_not_implemented)
        WS_CATCH(cppkit::ck_timeout_exception, response_internal_server_error)
        WS_CATCH(cppkit::ck_io_exception, response_internal_server_error)
        WS_CATCH(cppkit::ck_internal_exception, response_internal_server_error)
        catch(std::exception& ex)
        {
            // Note: we cannot write a response here because this exception
            // might indicate our sockets are not invalid simply because of
            // and incomplete io... but nevertheless our sockets might not
            // be able to communicate becuase the other side make have
            // broken its connection.
            CK_LOG_EXCEPTION(ex);
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
