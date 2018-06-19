
#ifndef webbie_range_request_generator_h
#define webbie_range_request_generator_h

#include "webbie/client_request.h"
#include "cppkit/ck_string_utils.h"

class test_range_request_generator;

namespace webbie
{

class range_request_generator final
{
    friend class ::test_range_request_generator;
public:
    range_request_generator(const std::string& host, int port, const std::string& uri, size_t totalSize, size_t requestSize, size_t pos);
    range_request_generator(const range_request_generator& obj) = default;
    range_request_generator(range_request_generator&&) noexcept = default;
    ~range_request_generator() noexcept;

    range_request_generator& operator=(const range_request_generator& obj) = default;
    range_request_generator& operator=(range_request_generator&&) noexcept = default;

    bool valid() const;
    client_request get() const;
    void next();

private:
    std::string _host;
    int _port;
    std::string _uri;
    size_t _totalSize;
    size_t _requestSize;
    size_t _pos;
};

}

#endif
