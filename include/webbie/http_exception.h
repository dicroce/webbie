
#ifndef webbie_webbie_exception_h
#define webbie_webbie_exception_h

#include "cppkit/ck_exception.h"
#include "cppkit/ck_string_utils.h"

namespace webbie
{

/// An internal exception type used to enter io error state.
class webbie_io_exception : public cppkit::ck_exception
{
public:
    webbie_io_exception();

    virtual ~webbie_io_exception() noexcept {}

    webbie_io_exception(const char* msg, ...);

    webbie_io_exception(const std::string& msg);
};

/// General exception type for Webby.
class webbie_exception_generic : public cppkit::ck_exception
{
public:
    webbie_exception_generic();

    virtual ~webbie_exception_generic() noexcept {}

    webbie_exception_generic(const char* msg, ...);

    webbie_exception_generic(const std::string& msg);
};

class webbie_exception : public webbie_exception_generic
{
public:
    webbie_exception( int statusCode );

    virtual ~webbie_exception() noexcept {}

    webbie_exception( int statusCode, const char* msg, ... );

    webbie_exception( int statusCode, const std::string& msg );

    int get_status() const { return _statusCode; }

private:
    int _statusCode;
};

class webbie_400_exception : public webbie_exception
{
public:
    webbie_400_exception();

    virtual ~webbie_400_exception() noexcept {}

    webbie_400_exception( const char* msg, ... );

    webbie_400_exception( const std::string& msg );
};

class webbie_401_exception : public webbie_exception
{
public:
    webbie_401_exception();

    virtual ~webbie_401_exception() noexcept {}

    webbie_401_exception( const char* msg, ... );

    webbie_401_exception( const std::string& msg );
};

class webbie_403_exception : public webbie_exception
{
public:
    webbie_403_exception();

    virtual ~webbie_403_exception() noexcept {}

    webbie_403_exception( const char* msg, ... );

    webbie_403_exception( const std::string& msg );
};

class webbie_404_exception : public webbie_exception
{
public:
    webbie_404_exception();

    virtual ~webbie_404_exception() noexcept {}

    webbie_404_exception( const char* msg, ... );

    webbie_404_exception( const std::string& msg );
};

class webbie_415_exception : public webbie_exception
{
public:
    webbie_415_exception();

    virtual ~webbie_415_exception() noexcept {}

    webbie_415_exception( const char* msg, ... );

    webbie_415_exception( const std::string& msg );
};

class webbie_453_exception : public webbie_exception
{
public:
    webbie_453_exception();

    virtual ~webbie_453_exception() noexcept{}

    webbie_453_exception( const char* msg, ... );

    webbie_453_exception( const std::string& msg );
};

class webbie_500_exception : public webbie_exception
{
public:
    webbie_500_exception();

    virtual ~webbie_500_exception() noexcept {}

    webbie_500_exception( const char* msg, ... );

    webbie_500_exception( const std::string& msg );
};

class webbie_501_exception : public webbie_exception
{
public:
    webbie_501_exception();

    virtual ~webbie_501_exception() noexcept {}

    webbie_501_exception( const char* msg, ... );

    webbie_501_exception( const std::string& msg );
};

/// \brief Throws an exception corresponding to the given status code
///        or a plain webbie_exception if there isn't one.
void throw_webbie_exception( int statusCode, const char* msg, ... );
void throw_webbie_exception( int statusCode, const std::string& msg );

#define CATCH_TRANSLATE_HTTP_EXCEPTIONS(a) \
    catch( webbie_400_exception& ex ) \
    { \
        CK_LOG_ERROR( "%s", ex.what() ); \
        a.set_status_code( response_bad_request ); \
    } \
    catch( webbie_401_exception& ex ) \
    { \
        CK_LOG_ERROR( "%s", ex.what() ); \
        response.set_status_code( response_unauthorized ); \
    } \
    catch( webbie_403_exception& ex ) \
    { \
        CK_LOG_ERROR( "%s", ex.what() ); \
        response.set_status_code( response_forbidden ); \
    } \
    catch( webbie_404_exception& ex ) \
    { \
        CK_LOG_ERROR( "%s", ex.what() ); \
        response.set_status_code( response_not_found ); \
    } \
    catch( webbie_500_exception& ex ) \
    { \
        CK_LOG_ERROR( "%s", ex.what() ); \
        response.set_status_code( response_internal_server_error ); \
    } \
    catch( webbie_501_exception& ex ) \
    { \
        CK_LOG_ERROR( "%s", ex.what() ); \
        response.set_status_code( response_not_implemented ); \
    }

}

#endif
