
#include "test_client_request.h"
#include "cppkit/ck_socket.h"
#include "webbie/client_request.h"
#include "webbie/server_request.h"
#include <thread>
#include <unistd.h>

using namespace std;
using namespace cppkit;
using namespace webbie;

REGISTER_TEST_FIXTURE(test_client_request);

void test_client_request::setup()
{
}

void test_client_request::teardown()
{
}

void test_client_request::test_default_ctor()
{
    int port = RTF_NEXT_PORT();
    client_request request("127.0.0.1", port);
}

void test_client_request::test_copy_ctor()
{
    int port = RTF_NEXT_PORT();
    client_request ra("127.0.0.1", port);
    ra.set_uri("/vmas/device/status/?a=1");
    client_request rb(ra);
    RTF_ASSERT(rb._uri.get_full_raw_uri() == "/vmas/device/status?a=1");
}

void test_client_request::test_assignment_op()
{
    int port = RTF_NEXT_PORT();
    client_request ra("127.0.0.1", port);
    ra.set_uri("/vmas/device/status/");
    client_request rb("127.0.0.1", port);
    rb = ra;
    RTF_ASSERT(rb._uri.get_full_raw_uri() == "/vmas/device/status");
}

void test_client_request::test_write_request()
{
    int port = RTF_NEXT_PORT();

    auto th = std::thread([port](){
        ck_socket socket;
        socket.bind(port);
        socket.listen();

        auto clientSocket = socket.accept();

        server_request request;

        request.read_request(clientSocket);

        RTF_ASSERT(request.get_method() == METHOD_GET);

        uri u = request.get_uri();

        RTF_ASSERT(u.get_full_raw_uri() == "/vmas/device/status");
    });
    th.detach();

    usleep(1000000);

    ck_socket socket;
    socket.connect("127.0.0.1", port);

    client_request request("127.0.0.1", port);

    request.set_uri("/vmas/device/status");

    request.write_request(socket);
}
