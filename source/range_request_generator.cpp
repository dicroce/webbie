
#include "webbie/range_request_generator.h"

using namespace cppkit;
using namespace webbie;

range_request_generator::range_request_generator(const ck_string& uri, size_t totalSize, size_t requestSize, size_t pos, const ck_string& host, int port) :
    _uri(uri),
    _totalSize(totalSize),
    _requestSize(requestSize),
    _pos(pos),
    _host(host),
    _port(port)
{
}

range_request_generator::~range_request_generator() throw()
{
}

bool range_request_generator::valid() const
{
    return _pos < _totalSize;
}

client_request range_request_generator::get() const
{
    client_request req(_host, _port);
    req.set_method(METHOD_GET);
    req.set_uri(_uri);

    size_t be = _pos + (_requestSize-1);
    if(be >= _totalSize)
        be = (_totalSize-1);
    size_t bs = (be - _pos)+1;

    req.add_header("Range", ck_string::format("bytes=%u-%u",_pos,be));

    return req;
}

void range_request_generator::next()
{
    size_t be = _pos + (_requestSize-1);
    if(be >= _totalSize)
        be = (_totalSize-1);

    _pos = be + 1;
}
