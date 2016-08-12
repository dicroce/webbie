cmake_minimum_required(VERSION 2.8)
project(webbie)

set(SOURCES include/webbie/client_request.h
            include/webbie/client_response.h
            include/webbie/webbie_exception.h
            include/webbie/webbie_utils.h
            include/webbie/methods.h
            include/webbie/server_request.h
            include/webbie/server_response.h
            include/webbie/status_codes.h
            include/webbie/uri.h
            include/webbie/url.h
            source/webbie_exception.cpp
            source/webbie_utils.cpp
            source/uri.cpp
            source/url.cpp
            source/client_request.cpp
            source/client_response.cpp
            source/server_request.cpp
            source/server_response.cpp
            source/methods.cpp)

set(COMMON_LIBS cppkit)

include(common.cmake NO_POLICY_SCOPE)
include("${devel_artifacts_path}/build/base_lib.cmake" NO_POLICY_SCOPE)
