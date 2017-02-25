
#include "client_request_test.h"
#include "cppkit/ck_socket.h"
#include "cppkit/os/ck_time_utils.h"
#include "webbie/client_request.h"
#include "webbie/server_request.h"
#include <thread>

using namespace std;
using namespace cppkit;
using namespace webbie;

REGISTER_TEST_FIXTURE(client_request_test);

void client_request_test::setup()
{
}


void client_request_test::teardown()
{
}
#if 0
void* client_request_test::EntryPoint()
{

    if(_testNum == 0)
    {
        XSocket socket;
        socket.Bind(4224);
        socket.Listen();

        XRef<XSocket> clientSocket = socket.Accept();

        ServerSideRequest request;

        request.ReadRequest(clientSocket);

        UT_ASSERT(request.GetMethod() == "get");

        URI uri = request.GetURI();

        UT_ASSERT(uri.GetFullRawURI() == "/vmas/device/status");

        clientSocket->Shutdown(SOCKET_SHUT_SEND_FLAGS);
        socket.Shutdown(SOCKET_SHUT_RECV_FLAGS);

        clientSocket->Close();
        socket.Close();
    }
    else if(_testNum == 1)
    {
        XSocket socket;
        socket.Bind(4334);
        socket.Listen();

        XRef<XSocket> clientSocket = socket.Accept();

        ServerSideRequest request;

        request.ReadRequest(clientSocket);

        UT_ASSERT(request.GetMethod() == "post");

        UT_ASSERT(request.GetContentType().Contains("x-www-form-urlencoded"));

        XHash<XString>& postVars = request.GetPostVars();

        UT_ASSERT(postVars.Find("name1"));
        UT_ASSERT(*postVars.Find("name1") == "val1");
        UT_ASSERT(postVars.Find("name2"));
        UT_ASSERT(*postVars.Find("name2") == "val2");

        URI uri = request.GetURI();

        UT_ASSERT(uri.GetFullRawURI() == "/vmas/device/status");

        XString bodyAsString = request.GetBodyAsString();

        clientSocket->Shutdown(SOCKET_SHUT_FLAGS);
        socket.Shutdown(SOCKET_SHUT_FLAGS);
        clientSocket->Close();
        socket.Close();
    }
    else if(_testNum == 2)
    {
        XSocket socket;
        socket.Bind(4444);
        socket.Listen();

        XRef<XSocket> clientSocket = socket.Accept();

        ServerSideRequest request;

        request.ReadRequest(clientSocket);

        UT_ASSERT(request.GetMethod() == "post");

        UT_ASSERT(request.GetBodySize() == 10);

        unsigned char body[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        UT_ASSERT(memcmp(body, request.GetBody(), 10) == 0);

        URI uri = request.GetURI();

        UT_ASSERT(uri.GetFullRawURI() == "/vmas/device/status");

        XString bodyAsString = request.GetBodyAsString();

        clientSocket->Shutdown(SOCKET_SHUT_FLAGS);
        socket.Shutdown(SOCKET_SHUT_FLAGS);
        clientSocket->Close();
        socket.Close();
    }
    else if(_testNum == 3)
    {
        XSocket socket;
        socket.Bind(4554);
        socket.Listen();

        XRef<XSocket> clientSocket = socket.Accept();

        ServerSideRequest request;

        request.ReadRequest(clientSocket);

        UT_ASSERT(request.GetMethod() == "post");

        XString bodyAsString = request.GetBodyAsString();

        UT_ASSERT(bodyAsString == "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Header><evHeader><userID>302</userID><UCN>uuid:c7824e90-d16f-434e-9af2-75dcb8444348</UCN></evHeader></s:Header><s:Body><u:Seek xmlns:u=\"urn:schemas-pelco-com:service:StreamControl:1\"><sessionId>uuid:0770218a-1dd2-11b2-a641-8ed9042c8688</sessionId><startTime>2010-09-07T18:02:05.147</startTime></u:Seek></s:Body></s:Envelope>");

        clientSocket->Shutdown(SOCKET_SHUT_FLAGS);
        socket.Shutdown(SOCKET_SHUT_FLAGS);
        clientSocket->Close();
        socket.Close();
    }
    else if(_testNum == 4)
    {
        XSocket socket;
        socket.Bind(4354);
        socket.Listen();

        XRef<XSocket> clientSocket = socket.Accept();

        ServerSideRequest request;
        request.ReadRequest(clientSocket);

        UT_ASSERT(request.GetMethod() == "get");

        UT_ASSERT(*request.GetHeader("hELLO") == "world");
        UT_ASSERT(*request.GetHeader("GOODbye") == "Cruel World");
        UT_ASSERT(*request.GetHeader("badbye") == "shouldn't have bought it");
        UT_ASSERT(*request.GetHeader("silly_duck") == "yes, he's silly");
        UT_ASSERT(request.GetHeader("silly duck") == 0);
        UT_ASSERT(request.GetHeader("george") == 0);

        clientSocket->Shutdown(SOCKET_SHUT_FLAGS);
        socket.Shutdown(SOCKET_SHUT_FLAGS);
        clientSocket->Close();
        socket.Close();
    }
    else if ( _testNum == 5 )
    {
    }
    return NULL;
}
#endif
void client_request_test::test_default_ctor()
{
    int port = UT_NEXT_PORT();
    client_request request("127.0.0.1", port);
}

void client_request_test::test_copy_ctor()
{
    int port = UT_NEXT_PORT();
    client_request ra("127.0.0.1", port);
    ra.set_uri("/vmas/device/status/?a=1");
    client_request rb(ra);
    UT_ASSERT(rb._uri.get_full_raw_uri() == "/vmas/device/status?a=1");
}

void client_request_test::test_assignment_op()
{
    int port = UT_NEXT_PORT();
    client_request ra("127.0.0.1", port);
    ra.set_uri("/vmas/device/status/");
    client_request rb("127.0.0.1", port);
    rb = ra;
    UT_ASSERT(rb._uri.get_full_raw_uri() == "/vmas/device/status");
}

void client_request_test::test_write_request()
{
    int port = UT_NEXT_PORT();

    auto th = std::thread([port](){
        ck_socket socket;
        socket.bind(port);
        socket.listen();

        auto clientSocket = socket.accept();

        server_request request;

        request.read_request(clientSocket);

        UT_ASSERT(request.get_method() == METHOD_GET);

        uri u = request.get_uri();

        UT_ASSERT(u.get_full_raw_uri() == "/vmas/device/status");
    });
    th.detach();

    ck_sleep(1);

    ck_socket socket;
    socket.connect("127.0.0.1", port);

    client_request request("127.0.0.1", port);

    request.set_uri("/vmas/device/status");

    request.write_request(socket);
}

void client_request_test::test_write_post()
{
#if 0
    int port = UT_NEXT_PORT();

    auto th = std::thread([port](){
    });
    th.detach();




    _testNum = 1;

    Start();

    ck_sleep(1);

    XRef<XSocket> socket = new XSocket;
    socket->Connect((char*)"127.0.0.1", 4334);

    ClientSideRequest request;

    request.SetRequestType("POST");
    request.AddPostVar("name1", "val1");
    request.AddPostVar("name2", "val2");

    request.SetURI("/vmas/device/status");

    request.WriteRequest(socket);

    socket->Shutdown(SOCKET_SHUT_FLAGS);
    socket->Close();

    Join();
#endif
}

void client_request_test::test_write_post_with_body()
{
#if 0
    int port = UT_NEXT_PORT();

    auto th = std::thread([port](){
    });
    th.detach();




    _testNum = 2;

    Start();

    ck_sleep(1);

    XRef<XSocket> socket = new XSocket;
    socket->Connect((char*)"127.0.0.1", 4444);

    ClientSideRequest request;

    request.SetURI("/vmas/device/status");

    request.SetRequestType("POST");

    unsigned char body[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    request.SetBody(body, 10);

    request.WriteRequest(socket);

    socket->Shutdown(SOCKET_SHUT_FLAGS);
    socket->Close();

    Join();
#endif
}

// The return values of ServerSideRequest::GetHeader are supposed to have been stripped.
void client_request_test::test_mpf_1430()
{
#if 0
    int port = UT_NEXT_PORT();

    auto th = std::thread([port](){
    });
    th.detach();




    _testNum = 4;

    Start();

    ck_sleep(1);

    XRef<XSocket> socket = new XSocket;
    socket->Connect((char*)"127.0.0.1", 4354);

    ClientSideRequest request;

    request.SetRequestType("GET");
    request.AddHeader("hello", "world");
    request.AddHeader("GOODBYE", " Cruel World");
    request.AddHeader("badbye", "shouldn't have bought it ");
    request.AddHeader("silly_duck", " yes, he's silly ");

    UT_ASSERT(request.GetHeader("hELLO") == "world");
    UT_ASSERT(request.GetHeader("GOODbye") == "Cruel World");
    UT_ASSERT(request.GetHeader("badbye") == "shouldn't have bought it");
    UT_ASSERT(request.GetHeader("silly_duck") == "yes, he's silly");
    UT_ASSERT(request.GetHeader("silly duck") == "");
    UT_ASSERT(request.GetHeader("george") == "");

    request.WriteRequest(socket);

    socket->Shutdown(SOCKET_SHUT_FLAGS);
    socket->Close();

    Join();
#endif
}

void client_request_test::test_mpf_1422()
{
    int port = UT_NEXT_PORT();

    auto th = std::thread([port](){
        ck_socket clientSocket;
        ck_sleep(1);
        clientSocket.connect("127.0.0.1", port);
        client_request req("127.0.0.1", port);
        req.write_request(clientSocket);
    });
    th.detach();

    ck_socket socket;
    socket.bind(port);
    socket.listen();

    auto clientSocket = socket.accept();
    server_request request;
    request.read_request(clientSocket);

    auto host = request.get_header("Host");
    UT_ASSERT(host.value() == ck_string::format("127.0.0.1:%d", port));
}
