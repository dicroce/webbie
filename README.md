webbie
======

Minimal http implementation suitable for implementing basic rest interfaces. Example:

    r_web_server<ck_socket> ws(4242);

    ws.add_route(METHOD_GET, "/hi", [](const r_web_server<ck_socket>& ws, cppkit::ck_buffered_socket<cppkit::ck_socket>& conn, const r_server_request& request)->r_server_response {
        r_server_response response;
        response.set_body("hello");
        return response;
    });

    ws.start();

Check out the unit tests for more examples.

building
========
mkdir build && cd build && cmake .. && make
