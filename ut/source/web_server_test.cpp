
#include "web_server_test.h"
#include "cppkit/os/ck_time_utils.h"
#include "cppkit/ck_ssl_socket.h"
#include "webbie/web_server.h"
#include "webbie/client_request.h"
#include "webbie/client_response.h"
#include "webbie/server_request.h"
#include "webbie/server_response.h"

using namespace std;
using namespace cppkit;
using namespace webbie;

REGISTER_TEST_FIXTURE(web_server_test);

void web_server_test::setup()
{
}


void web_server_test::teardown()
{
}

client_response _send_request(int port, client_request& request)
{
    ck_socket socket;
    socket.connect("127.0.0.1", port);
    request.write_request(socket);

    client_response response;
    response.read_response(socket);

    return response;
}

void web_server_test::test_basic()
{
    int port = UT_NEXT_PORT();

    web_server<ck_socket> ws( port );

    ws.add_route(METHOD_GET, "/hi", [](const web_server<ck_socket>& ws, cppkit::ck_buffered_socket<cppkit::ck_socket>& conn, const server_request& request)->server_response {
        server_response response;
        response.set_body("hello");
        return response;
    });

    ws.start();

    ck_usleep(100000);

    client_request request("127.0.0.1", port);
    request.set_uri("/hi");

    auto response = _send_request(port, request);

    UT_ASSERT(response.get_body_as_string() == "hello" );
}

void web_server_test::test_basic_ssl()
{
    int port = UT_NEXT_PORT();

    web_server<ck_ssl_socket> ws( port );

    ck_ssl_socket& sok = ws.get_socket();
    sok.use_pem_certificate_file( "ServerCRT1.crt" );
    sok.use_pem_rsa_private_key_file( "PrivateKey1.key" );

    ws.add_route(METHOD_GET, "/hi", [](const web_server<ck_ssl_socket>& ws, cppkit::ck_buffered_socket<cppkit::ck_ssl_socket>& conn, const server_request& request)->server_response {
        server_response response;
        response.set_body("hello");
        return response;
    });

    ws.start();

    ck_usleep(100000);

    client_request request("127.0.0.1", port);
    request.set_uri("/hi");

    ck_ssl_socket socket;
    socket.connect("127.0.0.1", port);
    request.write_request(socket);

    client_response response;
    response.read_response(socket);

    UT_ASSERT(response.get_body_as_string() == "hello" );
}

void web_server_test::test_not_found()
{
    int port = UT_NEXT_PORT();

    web_server<ck_socket> ws( port );

    ws.add_route(METHOD_GET, "/hi", [](const web_server<ck_socket>& ws, cppkit::ck_buffered_socket<cppkit::ck_socket>& conn, const server_request& request)->server_response {
        server_response response;
        response.set_body("hello");
        return response;
    });

    ws.start();

    ck_usleep(100000);

    {
        client_request request("127.0.0.1", port);
        request.set_uri("/hello");

        auto response = _send_request(port, request);

        UT_ASSERT(response.get_status() == response_not_found );
    }

    // make sure that after a server returns a 404, it still functioning.

    {
        client_request request("127.0.0.1", port);
        request.set_uri("/hi");

        auto response = _send_request(port, request);

        UT_ASSERT(response.get_body_as_string() == "hello" );
    }
}
