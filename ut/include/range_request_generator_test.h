
#ifndef _webbie_range_request_generator_test_h
#define _webbie_range_request_generator_test_h

#include "framework.h"

class range_request_generator_test : public test_fixture
{
public:
    TEST_SUITE(range_request_generator_test);
        TEST(range_request_generator_test::test_basic);
        TEST(range_request_generator_test::test_even);
    TEST_SUITE_END();

    ~range_request_generator_test() throw() {}
    void setup();
    void teardown();

protected:
    void test_basic();
    void test_even();

};

#endif
