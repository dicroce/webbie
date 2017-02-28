
#include "framework.h"

class client_response_test : public test_fixture
{
public:
    TEST_SUITE_INIT(client_response_test)
        _total(0)
    TEST_SUITE_BEGIN()
        TEST(client_response_test::test_default_constructor);
        TEST(client_response_test::test_copy_constructor);
        TEST(client_response_test::test_assignment_operator);
        TEST(client_response_test::test_receive_response);
        TEST(client_response_test::test_streaming);
        TEST(client_response_test::test_100_continue);
        TEST(client_response_test::test_interrupted_streaming);
        TEST(client_response_test::test_multi_part);
        TEST(client_response_test::TestColonsInHeaders);
        TEST(client_response_test::TestMultipleHeadersWithSameKey);
        TEST(client_response_test::TestSpaceAtHeaderStart);
        TEST(client_response_test::TestTabAtHeaderStart);
        TEST(client_response_test::TestSpaceAtHeaderLine);
        TEST(client_response_test::TestTabAtHeaderLine);
        TEST(client_response_test::TestMultipleKeys);
    TEST_SUITE_END()

public:

    virtual ~client_response_test() throw() {}
    void setup();
    void teardown();

protected:

    void test_default_constructor();
    void test_copy_constructor();
    void test_assignment_operator();
    void test_receive_response();
    void test_streaming();
    void test_100_continue();
    void test_interrupted_streaming();
    void test_multi_part();
    void TestColonsInHeaders();
    void TestMultipleHeadersWithSameKey();
    void TestSpaceAtHeaderStart();
    void TestTabAtHeaderStart();
    void TestSpaceAtHeaderLine();
    void TestTabAtHeaderLine();
    void TestMultipleKeys();

 private:

    int _testNum;
    int _total;
};
