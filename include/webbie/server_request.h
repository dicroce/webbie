
#ifndef webbie_server_request_h
#define webbie_server_request_h

#include <list>
#include <map>
#include <memory>
#include <vector>
#include "cppkit/interfaces/ck_stream_io.h"
#include "cppkit/ck_string_utils.h"
#include "cppkit/ck_nullable.h"
#include "webbie/uri.h"
#include "webbie/methods.h"

namespace webbie
{

class server_request
{
public:
    server_request();

    server_request(const server_request& rhs);

    virtual ~server_request() noexcept;

    server_request& operator = (const server_request& rhs);

    void read_request(cppkit::ck_stream_io& socket);

    int get_method() const;

    uri get_uri() const;

    std::string get_content_type() const;

    cppkit::ck_nullable<std::string> get_header( const std::string& key ) const;
    std::map<std::string,std::string> get_headers() const;

    const uint8_t* get_body() const;
    size_t get_body_size() const;
    std::string get_body_as_string() const;

    std::map<std::string,std::string> get_post_vars() const;

    bool is_patch_request() const;
    bool is_put_request() const;
    bool is_post_request() const;
    bool is_get_request() const;
    bool is_delete_request() const;

private:
    void _set_header(const std::string& name, const std::string& value);
    void _clean_socket(cppkit::ck_stream_io& socket, char** writer);
    void _read_header_line(cppkit::ck_stream_io& socket, char* writer, bool firstLine);
    bool _add_line(std::list<std::string>& lines, const std::string& line);
    void _process_request_lines(const std::list<std::string>& requestLines);
    void _process_body(cppkit::ck_stream_io& socket);

    std::string _initialLine;
    std::map<std::string,std::string> _headerParts;
    std::map<std::string,std::string> _postVars;
    std::vector<uint8_t> _body;
    std::string _contentType;
};

}

#endif
