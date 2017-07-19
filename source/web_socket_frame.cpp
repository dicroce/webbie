
#include "webbie/web_socket_frame.h"
#include <stdlib.h>

using namespace cppkit;
using namespace std;
using namespace webbie;

web_socket_frame::web_socket_frame() :
    _fin(true),
    _opcode(255),
    _mask(false),
    _maskingKey(),
    _payload()
{
    memset(_maskingKey, 0, 4);
}

web_socket_frame::web_socket_frame( web_socket_frame&& obj ) throw()
{
    _fin = std::move(obj._fin);
    obj._fin = true;
    _opcode = std::move(obj._opcode);
    obj._opcode = 255;
    _mask = std::move(obj._mask);
    obj._mask = false;
    memcpy(_maskingKey, obj._maskingKey, 4);
    memset(obj._maskingKey, 0, 4);
    _payload = std::move(obj._payload);
    obj._payload.clear();
}

web_socket_frame::~web_socket_frame() throw ()
{
}

web_socket_frame& web_socket_frame::operator = ( web_socket_frame&& obj ) throw()
{
    _fin = std::move(obj._fin);
    obj._fin = true;
    _opcode = std::move(obj._opcode);
    obj._opcode = 255;
    _mask = std::move(obj._mask);
    obj._mask = false;
    memcpy(_maskingKey, obj._maskingKey, 4);
    memset(obj._maskingKey, 0, 4);
    _payload = std::move(obj._payload);
    obj._payload.clear();

    return *this;
}

void web_socket_frame::set_masking_key( uint8_t* key )
{
    for(int i=0; i < 4; ++i)
        _maskingKey[i] = key[i];

    _mask = true;
}

void web_socket_frame::set_mask( bool mask )
{
    _mask = mask;
    _maskingKey[0] = (uint8_t)random();
    _maskingKey[1] = (uint8_t)random();
    _maskingKey[2] = (uint8_t)random();
    _maskingKey[3] = (uint8_t)random();
}

const uint8_t& web_socket_frame::extend(size_t sz)
{
    auto originalSize = _payload.size();
    _payload.resize(originalSize + sz);
    return _payload.at(originalSize);
}

void web_socket_frame::append_payload( const uint8_t* extraPayload, size_t extraPayloadSize )
{
    auto originalSize = _payload.size();
    _payload.resize(originalSize + extraPayloadSize);
    memcpy(&_payload.at(originalSize), extraPayload, extraPayloadSize);
}

ck_string web_socket_frame::get_payload_as_string() const
{
    return ck_string( (char*)&_payload->at(0), _payload->size() );
}
