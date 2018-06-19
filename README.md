webbie
======

Minimal http implementation suitable for implementing basic rest interfaces. Example:

    web_server<ck_socket> ws(4242);

    ws.add_route(METHOD_GET, "/hi", [](const web_server<ck_socket>& ws,
                                       ck_buffered_socket<ck_socket>& conn,
                                       const server_request& request)->server_response {
        server_response response;
        response.set_body("hello");
        return response;
    });

    ws.start();

Check out the unit tests for more examples.

building
========
mkdir build && cd build && cmake .. && make
