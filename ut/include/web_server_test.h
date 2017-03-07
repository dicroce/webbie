
#ifndef _webbie_web_server_test_h
#define _webbie_web_server_test_h

#include "framework.h"

class web_server_test : public test_fixture
{
public:
    TEST_SUITE(web_server_test);
        TEST(web_server_test::test_basic);
    TEST_SUITE_END();

    ~web_server_test() throw() {}
    void setup();
    void teardown();

protected:
    void test_basic();

};

#endif
