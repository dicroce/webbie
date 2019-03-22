cmake_minimum_required (VERSION 2.8.12)

include (DownloadProject.cmake)

if(NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif()

set (CMAKE_CXX_STANDARD 14)

# Dependencies

if(NOT EXISTS ${cppkit_SOURCE_DIR})
    download_project (PROJ cppkit
                    GIT_REPOSITORY https://github.com/dicroce/cppkit
                    GIT_TAG master
                    ${UPDATE_DISCONNECTED_IF_AVAILABLE})

    add_subdirectory (${cppkit_SOURCE_DIR} ${cppkit_BINARY_DIR})
endif()

# webbie

add_library (webbie include/webbie/client_request.h
                    include/webbie/client_response.h
                    include/webbie/http_exception.h
                    include/webbie/methods.h
                    include/webbie/range_request_generator.h
                    include/webbie/server_request.h
                    include/webbie/server_response.h
                    include/webbie/status_codes.h
                    include/webbie/uri.h
                    include/webbie/utils.h
                    include/webbie/web_server.h
                    source/client_request.cpp
                    source/client_response.cpp
                    source/http_exception.cpp
                    source/methods.cpp
                    source/range_request_generator.cpp
                    source/server_request.cpp
                    source/server_response.cpp
                    source/uri.cpp
                    source/utils.cpp
                    source/web_server.cpp)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(cppkit PUBLIC "-O0")
    target_compile_options(cppkit PUBLIC "-ggdb")
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(cppkit PUBLIC "-O3")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

target_include_directories (webbie PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries (webbie cppkit)

add_subdirectory(ut)
