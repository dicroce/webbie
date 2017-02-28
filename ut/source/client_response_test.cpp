
#include "client_response_test.h"
#include "webbie/client_request.h"
#include "webbie/client_response.h"
#include "webbie/server_request.h"
#include "webbie/server_response.h"
#include "webbie/webbie_exception.h"
#include "cppkit/os/ck_time_utils.h"
#include "cppkit/ck_socket.h"
#include <thread>

using namespace std;
using namespace cppkit;
using namespace webbie;

REGISTER_TEST_FIXTURE(client_response_test);

void client_response_test::setup()
{
}


void client_response_test::teardown()
{
}

void client_response_test::test_default_constructor()
{
    client_response response;
}

void client_response_test::test_copy_constructor()
{
}

void client_response_test::test_assignment_operator()
{
}

void client_response_test::test_receive_response()
{
    int port = UT_NEXT_PORT();

    auto th = thread([&](){
        ck_socket socket;
        socket.bind(port);
        socket.listen();

        auto clientSocket = socket.accept();

        server_request ssRequest;
        ssRequest.read_request(clientSocket);

        auto body = ssRequest.get_body_as_string();

        server_response ssResponse;
        ssResponse.set_body(body);
        UT_ASSERT_NO_THROW(ssResponse.write_response(clientSocket));

        clientSocket.close();

        socket.close();
    });
    th.detach();

    ck_usleep(250000);

    ck_socket socket;
    socket.connect("127.0.0.1", port);

    ck_string message = "Hello, Webby!";

    client_request request("127.0.0.1", port);

    request.set_method(METHOD_POST);
    request.set_body(ck_byte_ptr(message.c_str(), message.length()));

    UT_ASSERT_NO_THROW(request.write_request(socket));

    client_response response;
    response.read_response(socket);

    auto responseBody = response.get_body_as_string();

    UT_ASSERT(responseBody == "Hello, Webby!");
}

void client_response_test::test_streaming()
{
    int port = UT_NEXT_PORT();

    auto th = thread([&](){
        ck_socket socket;
        socket.bind(port);
        socket.listen();

        auto clientSocket = socket.accept();

        server_request ssRequest;
        ssRequest.read_request(clientSocket);

        server_response ssResponse;
        ssResponse.set_content_type("application/x-rtsp-tunnelled");

        for(uint8_t i = 0; i < 10; ++i)
            ssResponse.write_chunk(clientSocket, 1, &i);
        
        ssResponse.write_chunk_finalizer(clientSocket);
    });
    th.detach();

    ck_usleep(250000);

    ck_socket socket;
    socket.connect("127.0.0.1", port);

    ck_string message = "Hello, Webby!";

    client_request request("127.0.0.1", port);
    request.set_method(METHOD_POST);
    request.set_body(message);

    request.write_request(socket);

    uint8_t sum = 0;

    client_response response;
    response.register_chunk_callback([&](const vector<uint8_t>& chunk, const client_response& response){
        sum += chunk[0];
    },
    true);
    response.read_response(socket);

    // For this test the server sends back 10 chunks, each 1 byte long... Containing the values
    // 9 down to 0. The sum of 0 .. 9 == 45. Our callback just sums them, so at this point our
    // _total should == 45.
    UT_ASSERT(sum == 45);
}

void client_response_test::test_100_continue()
{
    int port = UT_NEXT_PORT();

    auto th = thread([&](){
        ck_socket socket;
        socket.bind(port);
        socket.listen();

        auto clientSocket = socket.accept();

        server_request ssRequest;
        ssRequest.read_request(clientSocket);

        auto body = ssRequest.get_body_as_string();

        ck_string cont = "HTTP/1.1 100 Continue\r\n\r\n";
        clientSocket.send(cont.c_str(), cont.length());

        server_response ssResponse;
        ssResponse.set_body(body);
        ssResponse.write_response(clientSocket);
    });
    th.detach();

    ck_usleep(250000);

    ck_socket socket;
    socket.connect("127.0.0.1", port);

    ck_string message = "Hello, Webby!";

    client_request request("127.0.0.1", port);
    request.set_method(METHOD_POST);
    request.set_body(message);

    request.write_request(socket);

    client_response response;
    response.read_response(socket);

    auto responseBody = response.get_body_as_string();

    UT_ASSERT(responseBody == "Hello, Webby!");
}

void client_response_test::test_interrupted_streaming()
{
    int port = UT_NEXT_PORT();

    auto th = thread([&](){
        ck_socket socket;
        socket.bind(port);
        socket.listen();

        auto clientSocket = socket.accept();

        server_request ssRequest;
        ssRequest.read_request(clientSocket);

        server_response ssResponse;
        ssResponse.set_content_type("application/x-rtsp-tunnelled");

        for(uint8_t i = 0; i < 10; ++i)
        {
            ssResponse.write_chunk(clientSocket, 1, &i);
            ck_sleep(1);
        }

        ssResponse.write_chunk_finalizer(clientSocket);
    });
    th.detach();

    ck_usleep(250000);

    ck_socket socket;
    socket.connect("127.0.0.1", port);

    ck_string message = "Hello, Webby!";

    client_request request("127.0.0.1", port);
    request.set_method(METHOD_POST);
    request.set_body(message);

    request.write_request(socket);

    auto thInt = std::thread([&](){
        ck_sleep(5);
        socket.close();
    });
    thInt.detach();

    uint8_t sum = 0;

    client_response response;
    response.register_chunk_callback([&](const vector<uint8_t>& chunk, const client_response& response){
        sum += chunk[0];
    },
    true);

    UT_ASSERT_THROWS(response.read_response(socket), webbie_io_exception);

    // For this test the server sends back 10 chunks, each 1 byte long... Containing the values
    // 9 down to 0. But, we interrupt it 5 seconds in... So, the total here is probably 10 (0+1+2+3+4), but
    // the safer test (with less race conditions) is to just test that the _total is less than the 45 it would
    // be if it had run to completion.
    UT_ASSERT(sum < 45);
}

void client_response_test::test_multi_part()
{
    int port = UT_NEXT_PORT();

    auto th = thread([&](){
        ck_socket socket;
        socket.bind(port);
        socket.listen();

        auto clientSocket = socket.accept();

        server_request ssRequest;
        ssRequest.read_request(clientSocket);

        server_response ssResponse;
        ssResponse.set_content_type("multipart/x-mixed-replace; boundary=\"foo\"");

        ssResponse.write_response(clientSocket);

        for(uint8_t i = 0; i < 10; ++i)
        {
            map<string,ck_string> headers;
            headers.insert(make_pair("Content-Type", "image/jpeg"));
            ssResponse.write_part(clientSocket, "foo", headers, &i, 1);
            ck_sleep(1);
        }

        ssResponse.write_part_finalizer( clientSocket, "foo");
    });
    th.detach();

    ck_usleep(250000);

    ck_socket socket;
    socket.connect("127.0.0.1", port);

    ck_string message = "Hello, Webby!";

    client_request request("127.0.0.1", port);
    request.set_method(METHOD_POST);
    request.set_body(message);

    request.write_request(socket);

    uint8_t sum = 0;

    client_response response;
    response.register_part_callback([&](const std::vector<uint8_t>& chunk, const std::map<std::string, cppkit::ck_string>& headers, const client_response& response){
        sum += chunk[0];
    });

    response.read_response(socket);

    UT_ASSERT(sum == 45);
}

void client_response_test::TestColonsInHeaders()
{
    int port = UT_NEXT_PORT();

    auto th = thread([&](){
        ck_socket socket;
        socket.bind(port);
        socket.listen();

        ck_socket clientSocket = socket.accept();

        server_request ssRequest;
        ssRequest.read_request(clientSocket);

        server_response ssResponse;
        ssResponse.add_additional_header("HeaderWithColon", "This header has a : in it.");

        ssResponse.write_response(clientSocket);
    });
    th.detach();

    ck_usleep(250000);

    ck_socket socket;
    socket.connect("127.0.0.1", port);

    client_request request("127.0.0.1", port);
    request.write_request(socket);

    client_response response;
    response.read_response(socket);

    ck_string headerValue = response.get_header("HeaderWithColon");

    UT_ASSERT(headerValue.contains("This header has a : in it."));
}

void client_response_test::TestMultipleHeadersWithSameKey()
{
    int port = UT_NEXT_PORT();

    auto th = thread([&](){
        ck_socket socket;
        socket.bind(port);
        socket.listen();

        ck_socket clientSocket = socket.accept();

        server_request ssRequest;
        ssRequest.read_request(clientSocket);

        //Note: We don't use server_response here because server_response does not support
        //multiple header values associated with the same key. If this is needed, we could add it, but
        //for now we just return a manually constructed response.

        ck_string response = "HTTP/1.1 200 OK\r\nDate: Mon Nov 14 09:58:23 2011\r\nContent-Type: text/plain\r\nkey: val 1\r\nkey: val 2\r\n\r\n";
        clientSocket.send(response.c_str(), response.length());
    });
    th.detach();

    ck_usleep(250000);

    ck_socket socket;
    socket.connect("127.0.0.1", port);

    client_request request("127.0.0.1", port);
    request.write_request(socket);

    client_response response;
    response.read_response(socket);

    vector<ck_string> matchingHeaders = response.get_all_matching_headers("key");

    int numHeaders = matchingHeaders.size();

    UT_ASSERT(numHeaders == 2);

    UT_ASSERT(matchingHeaders[0] == "val 1");
    UT_ASSERT(matchingHeaders[1] == "val 2");
}

void client_response_test::TestSpaceAtHeaderStart()
{
    int port = UT_NEXT_PORT();

    auto th = thread([&](){
        ck_socket socket;
        socket.bind(port);
        socket.listen();

        ck_socket clientSocket = socket.accept();

        server_request ssRequest;
        ssRequest.read_request(clientSocket);

        ck_string response = "HTTP/1.1 200 OK\r\n Date: Mon Nov 14 09:58:23 2011\r\nContent-Type: text/plain\r\nkey: val 1\r\nhole: val 2\r\nvalue:key\r\n\r\n";
        clientSocket.send(response.c_str(), response.length());
    });
    th.detach();

    ck_usleep(250000);

    ck_socket socket;
    socket.connect("127.0.0.1", port);

    client_request request("127.0.0.1", port);
    request.write_request(socket);

    client_response response;
    UT_ASSERT_THROWS(response.read_response(socket), webbie_exception);
}

void client_response_test::TestTabAtHeaderStart()
{
    int port = UT_NEXT_PORT();

    auto th = thread([&](){
        ck_socket socket;
        socket.bind(port);
        socket.listen();

        ck_socket clientSocket = socket.accept();

        server_request ssRequest;
        ssRequest.read_request(clientSocket);

        ck_string response = "HTTP/1.1 200 OK\r\n\tDate: Mon Nov 14 09:58:23 2011\r\nContent-Type: text/plain\r\nkey: val 1\r\nhole: val 2\r\nvalue:key\r\n\r\n";
        clientSocket.send(response.c_str(), response.length());
    });
    th.detach();

    ck_usleep(250000);

    ck_socket socket;
    socket.connect("127.0.0.1", port);

    client_request request("127.0.0.1", port);
    request.write_request(socket);

    client_response response;
    UT_ASSERT_THROWS(response.read_response(socket), webbie_exception);
}

void client_response_test::TestSpaceAtHeaderLine()
{
    int port = UT_NEXT_PORT();

    auto th = thread([&](){
        ck_socket socket;
        socket.bind(port);
        socket.listen();

        ck_socket clientSocket = socket.accept();

        server_request ssRequest;
        ssRequest.read_request(clientSocket);

        ck_string response = "HTTP/1.1 200 OK\r\nDate: Mon Nov 14 09:58:23 2011\r\nContent-Type: text/plain\r\nkey: val 1\r\nhole: val 2\r\n value:key\r\n\r\n";
        clientSocket.send(response.c_str(), response.length());
    });
    th.detach();

    ck_usleep(250000);

    ck_socket socket;
    socket.connect("127.0.0.1", port);

    client_request request("127.0.0.1", port);
    request.write_request(socket);

    client_response response;
    response.read_response(socket);

    vector<ck_string> matchingHeaders = response.get_all_matching_headers("key");
    UT_ASSERT(matchingHeaders.size() == 1);
    UT_ASSERT(matchingHeaders[0] == "val 1");

    matchingHeaders = response.get_all_matching_headers("hole");
    UT_ASSERT(matchingHeaders.size() == 1);
    UT_ASSERT(matchingHeaders[0] == "val 2\r\n value:key");

    UT_ASSERT(response.get_all_matching_headers("value").empty());
}

void client_response_test::TestTabAtHeaderLine()
{
    int port = UT_NEXT_PORT();

    auto th = thread([&](){
        ck_socket socket;
        socket.bind(port);
        socket.listen();

        ck_socket clientSocket = socket.accept();

        server_request ssRequest;
        ssRequest.read_request(clientSocket);

        ck_string response = "HTTP/1.1 200 OK\r\nDate: Mon Nov 14 09:58:23 2011\r\nContent-Type: text/plain\r\nkey: val 1\r\nhole: val 2\r\n\tvalue:key\r\n\r\n";
        clientSocket.send(response.c_str(), response.length());
    });
    th.detach();

    ck_usleep(250000);

    ck_socket socket;
    socket.connect("127.0.0.1", port);

    client_request request("127.0.0.1", port);
    request.write_request(socket);

    client_response response;
    response.read_response(socket);

    vector<ck_string> matchingHeaders = response.get_all_matching_headers("key");
    UT_ASSERT(matchingHeaders.size() == 1);
    UT_ASSERT(matchingHeaders[0] == "val 1");

    matchingHeaders = response.get_all_matching_headers("hole");
    UT_ASSERT(matchingHeaders.size() == 1);
    UT_ASSERT(matchingHeaders[0] == "val 2\r\n\tvalue:key");

    UT_ASSERT(response.get_all_matching_headers("value").empty());
}

void client_response_test::TestMultipleKeys()
{
    int port = UT_NEXT_PORT();

    auto th = thread([&](){
        ck_socket socket;
        socket.bind(port);
        socket.listen();

        ck_socket clientSocket = socket.accept();

        server_request ssRequest;
        ssRequest.read_request(clientSocket);

        ck_string response = "HTTP/1.1 200 OK\r\nDate: Mon Nov 14 09:58:23 2011\r\nContent-Type: text/plain\r\nkey: val 1\r\nhole: val 2\r\nvalue:key\r\n\r\n";
        clientSocket.send(response.c_str(), response.length());
    });
    th.detach();

    ck_usleep(250000);

    ck_socket socket;
    socket.connect("127.0.0.1", port);

    client_request request("127.0.0.1", port);
    request.write_request(socket);

    client_response response;
    response.read_response(socket);

    vector<ck_string> matchingHeaders = response.get_all_matching_headers("key");
    UT_ASSERT(matchingHeaders.size() == 1);
    UT_ASSERT(matchingHeaders[0] == "val 1");

    matchingHeaders = response.get_all_matching_headers("hole");
    UT_ASSERT(matchingHeaders.size() == 1);
    UT_ASSERT(matchingHeaders[0] == "val 2");

    matchingHeaders = response.get_all_matching_headers("value");
    UT_ASSERT(matchingHeaders.size() == 1);
    UT_ASSERT(matchingHeaders[0] == "key");
}
