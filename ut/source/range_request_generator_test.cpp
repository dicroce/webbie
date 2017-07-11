
#include "range_request_generator_test.h"
#include "webbie/range_request_generator.h"

using namespace std;
using namespace cppkit;
using namespace webbie;

REGISTER_TEST_FIXTURE(range_request_generator_test);

void range_request_generator_test::setup()
{
}

void range_request_generator_test::teardown()
{
}

void range_request_generator_test::test_basic()
{
    range_request_generator rrg("/foo",15,4,0,"127.0.0.1",8080);

    //range = bytes=0-3
    //range = bytes=4-7
    //range = bytes=8-11
    //range = bytes=12-14

    int vals[] = {0,3,4,7,8,11,12,14};
    int vi= 0;

    while(rrg.valid())
    {
        auto req = rrg.get();
        auto uri = req._uri.get_full_raw_uri();
        auto outerParts = req._headerParts["range"].split("=");
        auto innerParts = outerParts[1].split("-");
        UT_ASSERT(vals[vi] == innerParts[0].to_int());
        ++vi;
        UT_ASSERT(vals[vi] == innerParts[1].to_int());
        ++vi;
        rrg.next();
    }
}
