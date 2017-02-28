
/// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-
/// webbie - http://www.cppkit.org
/// Copyright (c) 2013, Tony Di Croce
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without modification, are permitted
/// provided that the following conditions are met:
///
/// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and
///    the following disclaimer.
/// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
///    and the following disclaimer in the documentation and/or other materials provided with the
///    distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
/// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
/// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
/// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
/// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
/// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
/// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/// The views and conclusions contained in the software and documentation are those of the authors and
/// should not be interpreted as representing official policies, either expressed or implied, of the cppkit
/// Project.
/// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-

#ifndef webbie_webbie_exception_h
#define webbie_webbie_exception_h

#include "cppkit/ck_exception.h"
#include "cppkit/ck_string.h"

namespace webbie
{

/// An internal exception type used to enter io error state.
class webbie_io_exception : public cppkit::ck_exception
{
public:
    CK_API webbie_io_exception();

    CK_API virtual ~webbie_io_exception() throw() {}

    CK_API webbie_io_exception(const char* msg, ...);

    CK_API webbie_io_exception(const cppkit::ck_string& msg);
};

/// General exception type for Webby.
class webbie_exception : public cppkit::ck_exception
{
public:
    CK_API webbie_exception();

    CK_API virtual ~webbie_exception() throw() {}

    CK_API webbie_exception(const char* msg, ...);

    CK_API webbie_exception(const cppkit::ck_string& msg);
};

class http_exception : public webbie_exception
{
public:
    CK_API http_exception( int statusCode );

    CK_API virtual ~http_exception() throw() {}

    CK_API http_exception( int statusCode, const char* msg, ... );

    CK_API http_exception( int statusCode, const cppkit::ck_string& msg );

    int get_status() const { return _statusCode; }

private:
    int _statusCode;
};

class http_400_exception : public http_exception
{
public:
    CK_API http_400_exception();

    CK_API virtual ~http_400_exception() throw() {}

    CK_API http_400_exception( const char* msg, ... );

    CK_API http_400_exception( const cppkit::ck_string& msg );
};

class http_401_exception : public http_exception
{
public:
    CK_API http_401_exception();

    CK_API virtual ~http_401_exception() throw() {}

    CK_API http_401_exception( const char* msg, ... );

    CK_API http_401_exception( const cppkit::ck_string& msg );
};

class http_403_exception : public http_exception
{
public:
    CK_API http_403_exception();

    CK_API virtual ~http_403_exception() throw() {}

    CK_API http_403_exception( const char* msg, ... );

    CK_API http_403_exception( const cppkit::ck_string& msg );
};

class http_404_exception : public http_exception
{
public:
    CK_API http_404_exception();

    CK_API virtual ~http_404_exception() throw() {}

    CK_API http_404_exception( const char* msg, ... );

    CK_API http_404_exception( const cppkit::ck_string& msg );
};

class http_415_exception : public http_exception
{
public:
    CK_API http_415_exception();

    CK_API virtual ~http_415_exception() throw() {}

    CK_API http_415_exception( const char* msg, ... );

    CK_API http_415_exception( const cppkit::ck_string& msg );
};

class http_453_exception : public http_exception
{
public:
    CK_API http_453_exception();

    CK_API virtual ~http_453_exception() throw() {}

    CK_API http_453_exception( const char* msg, ... );

    CK_API http_453_exception( const cppkit::ck_string& msg );
};

class http_500_exception : public http_exception
{
public:
    CK_API http_500_exception();

    CK_API virtual ~http_500_exception() throw() {}

    CK_API http_500_exception( const char* msg, ... );

    CK_API http_500_exception( const cppkit::ck_string& msg );
};

class http_501_exception : public http_exception
{
public:
    CK_API http_501_exception();

    CK_API virtual ~http_501_exception() throw() {}

    CK_API http_501_exception( const char* msg, ... );

    CK_API http_501_exception( const cppkit::ck_string& msg );
};

/// \brief Throws an exception corresponding to the given status code
///        or a plain http_exception if there isn't one.
CK_API void throw_http_exception( int statusCode, const char* msg, ... );
CK_API void throw_http_exception( int statusCode, const cppkit::ck_string& msg );

#define HTTP_THROW(PARAMS) \
CK_MACRO_BEGIN \
    try \
    { \
        throw_http_exception PARAMS ; \
    } \
    catch(http_exception& e) \
    { \
        e.set_throw_point(__LINE__, __FILE__); \
        std::vector<std::string> stack; \
        cppkit::ck_stack_trace::get_stack(stack); \
        e.set_stack(stack); \
        CK_LOG_WARNING("%s thrown. Msg: \"%s\", At: %s(%d)\n", \
                       e.get_type_name(),                      \
                       e.get_msg(),                            \
                       __FILE__,                               \
                       __LINE__);                              \
        cppkit::ck_log::log_backtrace(stack); \
        throw; \
    } \
CK_MACRO_END

#define CATCH_TRANSLATE_HTTP_EXCEPTIONS(a) \
    catch( http_400_exception& ex ) \
    { \
        CK_LOG_ERROR( "%s", ex.what() ); \
        a.set_status_code( response_bad_request ); \
    } \
    catch( http_401_exception& ex ) \
    { \
        CK_LOG_ERROR( "%s", ex.what() ); \
        response.set_status_code( response_unauthorized ); \
    } \
    catch( http_403_exception& ex ) \
    { \
        CK_LOG_ERROR( "%s", ex.what() ); \
        response.set_status_code( response_forbidden ); \
    } \
    catch( http_404_exception& ex ) \
    { \
        CK_LOG_ERROR( "%s", ex.what() ); \
        response.set_status_code( response_not_found ); \
    } \
    catch( http_500_exception& ex ) \
    { \
        CK_LOG_ERROR( "%s", ex.what() ); \
        response.set_status_code( response_internal_server_error ); \
    } \
    catch( http_501_exception& ex ) \
    { \
        CK_LOG_ERROR( "%s", ex.what() ); \
        response.set_status_code( response_not_implemented ); \
    }

}

#endif
