
#include "test_web_server.h"
#include "webbie/web_server.h"
#include "webbie/client_request.h"
#include "webbie/client_response.h"

using namespace std;
using namespace cppkit;
using namespace webbie;

REGISTER_TEST_FIXTURE(test_web_server);

void test_web_server::setup()
{
}

void test_web_server::teardown()
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

void test_web_server::test_basic()
{
    int port = RTF_NEXT_PORT();

    web_server<ck_socket> ws( port );

    ws.add_route(METHOD_GET, "/hi", [](const web_server<ck_socket>& ws, cppkit::ck_buffered_socket<cppkit::ck_socket>& conn, const server_request& request)->server_response {
        server_response response;
        response.set_body("hello");
        return response;
    });

    ws.start();

    usleep(100000);

    client_request request("127.0.0.1", port);
    request.set_uri("/hi");

    auto response = _send_request(port, request);

    RTF_ASSERT(response.get_body_as_string() == "hello" );
}
