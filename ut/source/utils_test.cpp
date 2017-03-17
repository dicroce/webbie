
#include "utils_test.h"
#include "webbie/webbie_utils.h"

using namespace std;
using namespace cppkit;
using namespace webbie;

REGISTER_TEST_FIXTURE(utils_test);

void utils_test::setup()
{
}

void utils_test::teardown()
{
}

void utils_test::test_basic()
{
    cppkit::ck_string host, uri;
    int port;
    parse_url_parts( "https://www.google.com/foo/bar", host, port, uri );
    UT_ASSERT( host == "www.google.com" );
    UT_ASSERT( port == 443 );
    UT_ASSERT( uri == "/foo/bar" );

    parse_url_parts( "http://www.google.com:443", host, port, uri );
    UT_ASSERT( host == "www.google.com" );
    UT_ASSERT( port == 443 );
    UT_ASSERT( uri == "/" );

    parse_url_parts( "http://www.google.com", host, port, uri );
    UT_ASSERT( host == "www.google.com" );
    UT_ASSERT( port == 80 );
    UT_ASSERT( uri == "/" );

    parse_url_parts( "http://127.0.0.1:10010/data_sources", host, port, uri );
    UT_ASSERT( host == "127.0.0.1" );
    UT_ASSERT( port == 10010 );
    UT_ASSERT( uri == "/data_sources" );
}
