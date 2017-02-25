
#ifndef _WEBBY_CLIENTSIDE_REQUEST_TEST_H_
#define _WEBBY_CLIENTSIDE_REQUEST_TEST_H_

#include "framework.h"

class client_request_test : public test_fixture
{
public:
    TEST_SUITE(client_request_test);
        TEST(client_request_test::test_default_ctor);
        TEST(client_request_test::test_copy_ctor);
        TEST(client_request_test::test_assignment_op);
        TEST(client_request_test::test_write_request);
        TEST(client_request_test::test_write_post);
        TEST(client_request_test::test_write_post_with_body);
        TEST(client_request_test::test_mpf_1430);
        TEST(client_request_test::test_mpf_1422);
    TEST_SUITE_END();

    ~client_request_test() throw() {}
    void setup();
    void teardown();

protected:
    void test_default_ctor();
    void test_copy_ctor();
    void test_assignment_op();
    void test_write_request();
    void test_write_post();
    void test_write_post_with_body();
    void test_mpf_1430();
    void test_mpf_1422();

 private:

    int _testNum;
};

#endif
