
#ifndef webbie_range_request_generator_h
#define webbie_range_request_generator_h

#include "webbie/client_request.h"
#include "cppkit/ck_string.h"

namespace webbie
{

class range_request_generator final
{
public:
    range_request_generator(const cppkit::ck_string& uri, size_t totalSize, size_t requestSize, size_t pos, const cppkit::ck_string& host, int port);
    range_request_generator(const range_request_generator& obj) = default;
    ~range_request_generator() throw();

    range_request_generator& operator=(const range_request_generator& obj) = default;

    bool valid() const;
    client_request get() const;
    void next();

private:
    cppkit::ck_string _uri;
    size_t _totalSize;
    size_t _requestSize;
    size_t _pos;
    cppkit::ck_string _host;
    int _port;
};

}

#endif