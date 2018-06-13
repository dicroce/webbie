
#include "webbie/http_exception.h"

using namespace webbie;
using namespace cppkit;
using namespace std;

webbie_io_exception::webbie_io_exception() :
    ck_exception()
{
}

webbie_io_exception::webbie_io_exception( const char* msg, ... ) :
    ck_exception()
{
    va_list args;
    va_start( args, msg );
    set_msg( ck_string_utils::format( msg, args ) );
    va_end( args );
}

webbie_exception_generic::webbie_exception_generic() :
    ck_exception()
{
}

webbie_exception_generic::webbie_exception_generic( const char* msg, ... ) :
    ck_exception()
{
    va_list args;
    va_start( args, msg );
    set_msg( ck_string_utils::format( msg, args ) );
    va_end( args );
}

webbie_exception_generic::webbie_exception_generic( const string& msg ) :
    ck_exception( msg )
{
}

webbie_exception::webbie_exception( int statusCode ) :
    webbie_exception_generic(),
    _statusCode( statusCode )
{
}

webbie_exception::webbie_exception( int statusCode, const char* msg, ... ) :
    webbie_exception_generic(),
    _statusCode( statusCode )
{
    va_list args;
    va_start( args, msg );
    set_msg( ck_string_utils::format( msg, args ) );
    va_end( args );
}

webbie_exception::webbie_exception( int statusCode, const string& msg ) :
    webbie_exception_generic( msg ),
    _statusCode( statusCode )
{
}

webbie_400_exception::webbie_400_exception() :
    webbie_exception( 400 )
{
}

webbie_400_exception::webbie_400_exception( const char* msg, ... ) :
    webbie_exception( 400 )
{
    va_list args;
    va_start( args, msg );
    set_msg( ck_string_utils::format( msg, args ) );
    va_end( args );
}

webbie_400_exception::webbie_400_exception( const string& msg ) :
    webbie_exception( 400, msg )
{
}

webbie_401_exception::webbie_401_exception() :
    webbie_exception( 401 )
{
}

webbie_401_exception::webbie_401_exception( const char* msg, ... ) :
    webbie_exception( 401 )
{
    va_list args;
    va_start( args, msg );
    set_msg( ck_string_utils::format( msg, args ) );
    va_end( args );
}

webbie_401_exception::webbie_401_exception( const string& msg ) :
    webbie_exception( 401, msg )
{
}

webbie_403_exception::webbie_403_exception() :
    webbie_exception( 403 )
{
}

webbie_403_exception::webbie_403_exception( const char* msg, ... ) :
    webbie_exception( 403 )
{
    va_list args;
    va_start( args, msg );
    set_msg( ck_string_utils::format( msg, args ) );
    va_end( args );
}

webbie_403_exception::webbie_403_exception( const string& msg ) :
    webbie_exception( 403, msg )
{
}

webbie_404_exception::webbie_404_exception() :
    webbie_exception( 404 )
{
}

webbie_404_exception::webbie_404_exception( const char* msg, ... ) :
    webbie_exception( 404 )
{
    va_list args;
    va_start( args, msg );
    set_msg( ck_string_utils::format( msg, args ) );
    va_end( args );
}

webbie_404_exception::webbie_404_exception( const string& msg ) :
    webbie_exception( 404, msg )
{
}

webbie_415_exception::webbie_415_exception() :
    webbie_exception( 415 )
{
}

webbie_415_exception::webbie_415_exception( const char* msg, ... ) :
    webbie_exception( 415 )
{
    va_list args;
    va_start( args, msg );
    set_msg( ck_string_utils::format( msg, args ) );
    va_end( args );
}

webbie_415_exception::webbie_415_exception( const string& msg ) :
    webbie_exception( 415, msg )
{
}

webbie_453_exception::webbie_453_exception() :
    webbie_exception( 453 )
{
}

webbie_453_exception::webbie_453_exception( const char* msg, ... ) :
    webbie_exception( 453 )
{
    va_list args;
    va_start( args, msg );
    set_msg( ck_string_utils::format( msg, args ) );
    va_end( args );
}

webbie_453_exception::webbie_453_exception( const string& msg ) :
    webbie_exception( 453, msg )
{
}

webbie_500_exception::webbie_500_exception() :
    webbie_exception( 500 )
{
}

webbie_500_exception::webbie_500_exception( const char* msg, ... ) :
    webbie_exception( 500 )
{
    va_list args;
    va_start( args, msg );
    set_msg( ck_string_utils::format( msg, args ) );
    va_end( args );
}

webbie_500_exception::webbie_500_exception( const string& msg ) :
    webbie_exception( 500, msg )
{
}

webbie_501_exception::webbie_501_exception() :
    webbie_exception( 501 )
{
}

webbie_501_exception::webbie_501_exception( const char* msg, ... ) :
    webbie_exception( 501 )
{
    va_list args;
    va_start( args, msg );
    set_msg( ck_string_utils::format( msg, args ) );
    va_end( args );
}

webbie_501_exception::webbie_501_exception( const string& msg ) :
    webbie_exception( 501, msg )
{
}

void webbie::throw_webbie_exception( int statusCode, const char* msg, ... )
{
    va_list args;
    va_start( args, msg );
    const string message = ck_string_utils::format( msg, args );
    va_end( args );

    webbie::throw_webbie_exception( statusCode, message );
}

void webbie::throw_webbie_exception( int statusCode, const string& msg )
{
    switch( statusCode )
    {
        case 400:
        {
            webbie_400_exception e( msg );
            throw e;
        }
        case 401:
        {
            webbie_401_exception e( msg );
            throw e;
        }
        case 403:
        {
            webbie_403_exception e( msg );
            throw e;
        }
        case 404:
        {
            webbie_404_exception e( msg );
            throw e;
        }
        case 415:
        {
            webbie_415_exception e( msg );
            throw e;
        }
        case 453:
        {
            webbie_453_exception e( msg );
            throw e;
        }
        case 500:
        {
            webbie_500_exception e( msg );
            throw e;
        }
        case 501:
        {
            webbie_501_exception e( msg );
            throw e;
        }
        default:
        {
            webbie_exception e( statusCode, msg );
            throw e;
        }
    }
}

