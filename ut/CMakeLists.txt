cmake_minimum_required(VERSION 2.8)
project(ut)

include(common.cmake NO_POLICY_SCOPE)

set(SOURCES source/framework.cpp
            source/client_request_test.cpp
            source/client_response_test.cpp
            source/web_server_test.cpp
            source/utils_test.cpp
            source/range_request_generator_test.cpp)

set(APPLICATION_TYPE "NORMAL_STATIC")

set(COMMON_LIBS cppkit webbie)
set(THIRD_PARTY_LIBS ssl crypto)

include("${devel_artifacts_path}/build/base_ut.cmake" NO_POLICY_SCOPE)
