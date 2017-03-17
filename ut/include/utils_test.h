
#ifndef _WEBBY_UTILS_TEST_H_
#define _WEBBY_UTILS_TEST_H_

#include "framework.h"

class utils_test : public test_fixture
{
public:
    TEST_SUITE(utils_test);
        TEST(utils_test::test_basic);
    TEST_SUITE_END();

    ~utils_test() throw() {}
    void setup();
    void teardown();

protected:
    void test_basic();

 private:

    int _testNum;
};

#endif
