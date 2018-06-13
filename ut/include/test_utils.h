
#include "framework.h"

class test_utils : public test_fixture
{
public:

    RTF_FIXTURE(test_utils);
      TEST(test_utils::test_basic);
    RTF_FIXTURE_END();

    virtual ~test_utils() throw() {}

    virtual void setup();
    virtual void teardown();

    void test_basic();
};
