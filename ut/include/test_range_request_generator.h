
#include "framework.h"

class test_range_request_generator : public test_fixture
{
public:
    RTF_FIXTURE(test_range_request_generator);
      TEST(test_range_request_generator::test_basic);
      TEST(test_range_request_generator::test_even);
    RTF_FIXTURE_END();

    virtual ~test_range_request_generator() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_basic();
    void test_even();
};
