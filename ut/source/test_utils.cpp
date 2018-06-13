
#include "test_utils.h"
#include "webbie/utils.h"

using namespace std;
using namespace cppkit;
using namespace webbie;

REGISTER_TEST_FIXTURE(test_utils);

void test_utils::setup()
{
}

void test_utils::teardown()
{
}

void test_utils::test_basic()
{
    string host, uri, protocol;
    int port;
    parse_url_parts( "https://www.google.com/foo/bar", host, port, protocol, uri );
    RTF_ASSERT( host == "www.google.com" );
    RTF_ASSERT( port == 443 );
    RTF_ASSERT( uri == "/foo/bar" );

    parse_url_parts( "http://www.google.com:443", host, port, protocol, uri );
    RTF_ASSERT( host == "www.google.com" );
    RTF_ASSERT( port == 443 );
    RTF_ASSERT( uri == "/" );

    parse_url_parts( "http://www.google.com", host, port, protocol, uri );
    RTF_ASSERT( host == "www.google.com" );
    RTF_ASSERT( port == 80 );
    RTF_ASSERT( uri == "/" );

    parse_url_parts( "http://127.0.0.1:10010/data_sources", host, port, protocol, uri );
    RTF_ASSERT( host == "127.0.0.1" );
    RTF_ASSERT( port == 10010 );
    RTF_ASSERT( uri == "/data_sources" );
}
