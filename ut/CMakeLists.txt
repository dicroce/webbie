cmake_minimum_required (VERSION 2.8.12)

add_executable (webbie_ut include/framework.h
                          include/test_client_request.h
                          include/test_client_response.h
                          include/test_range_request_generator.h
                          include/test_server_response.h
                          include/test_utils.h
                          include/test_web_server.h
                          source/framework.cpp
                          source/test_client_request.cpp
                          source/test_client_response.cpp
                          source/test_range_request_generator.cpp
                          source/test_server_response.cpp
                          source/test_utils.cpp
                          source/test_web_server.cpp)

target_include_directories (webbie_ut PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(webbie_ut -static-libstdc++)
target_link_libraries(webbie_ut webbie cppkit)
