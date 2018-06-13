
#ifndef _webbie_client_request_h
#define _webbie_client_request_h

#include "cppkit/interfaces/ck_stream_io.h"
#include "cppkit/ck_string_utils.h"

#include <unordered_map>
#include <memory>
#include <vector>

#include "webbie/uri.h"
#include "webbie/methods.h"

class test_client_request;
class test_range_request_generator;

namespace webbie
{

const std::string get_request = "GET";
const std::string post_request = "POST";
const std::string put_request = "PUT";
const std::string delete_request = "DELETE";
const std::string patch_request = "PATCH";

class client_request
{
    friend class ::test_client_request;
    friend class ::test_range_request_generator;

public:
    client_request( const std::string& host, int hostPort );

    client_request( const client_request& rhs );

    virtual ~client_request() noexcept;

    client_request& operator = ( const client_request& rhs );

    void set_method( int method );

    void write_request( cppkit::ck_stream_io& socket ) const;

    void set_accept_type( const std::string& acceptType );

    void set_content_type( const std::string& contentType );

    void set_uri( const uri& res );

    void set_basic_authentication( const std::string& user, const std::string& pass );

    void add_post_var( const std::string& name, const std::string& value );

    void add_header( const std::string& name, const std::string& value );

    void set_body( const uint8_t* src, size_t size );
    void set_body( const std::string& body );

private:
    std::string _get_headers_as_string( cppkit::ck_stream_io& socket ) const;

    uri _uri;
    std::string _acceptType;
    std::string _contentType;
    int _method;
    std::string _authData;
    std::unordered_map<std::string, std::string> _postVars;
    std::unordered_map<std::string, std::string> _headerParts;

    mutable std::vector<uint8_t> _body;
    std::string _host;
    int _hostPort;
};

}

#endif
