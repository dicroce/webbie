
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

#ifndef webbie_web_socket_frame_h
#define webbie_web_socket_frame_h

#include "cppkit/ck_string.h"
#include <memory>
#include <vector>

namespace webbie
{

const int8_t kWebSocketOpcodeContinuation        = 0x0;
const int8_t kWebSocketOpcodeTextFrame           = 0x1;
const int8_t kWebSocketOpcodeBinaryFrame         = 0x2;
// 0x3 - 0x7 are reserved
const int8_t kweb_socket_opcode_connection_close = 0x8;
const int8_t kWebSocketOpcodePing                = 0x9;
const int8_t kWebSocketOpcodePong                = 0xA;
// 0xB - 0xF are reserved

// This is used as an indicator in ReadFrame
// If we read > than Reserved, we error....
const int8_t kWebSocketOpcodeReserved		 	 = 0xB;

class web_socket_frame
{
public:
	CK_API web_socket_frame();
    CK_API web_socket_frame( web_socket_frame&& obj ) throw();
	CK_API web_socket_frame( const web_socket_frame& obj) = delete;

	CK_API virtual ~web_socket_frame() throw();

    CK_API web_socket_frame& operator = ( web_socket_frame&& obj ) throw();
	CK_API web_socket_frame& operator = ( const web_socket_frame& obj) = delete;

	CK_API bool is_fin() const { return _fin; }
	CK_API void set_fin(bool fin) { _fin = fin; }

	CK_API void set_opcode( uint8_t code ) { _opcode = code; }
	CK_API uint8_t get_opcode() const { return _opcode; }
	
	CK_API void set_mask( bool mask );
	CK_API bool is_mask() const { return _mask; }

	CK_API void set_masking_key( uint8_t* key ); 
	CK_API uint8_t* get_masking_key() const { return _maskingKey; }

    CK_API const uint8_t& extend(size_t sz);
    CK_API const uint8_t* get_payload() const { return &_payload[0]; }
	CK_API size_t get_payload_size() const { return _payload.size(); }

	CK_API void append_payload( const uint8_t* extraPayload, size_t extraPayloadSize ); 

	CK_API cppkit::ck_string get_payload_as_string() const;

private:
	bool _fin;
	uint8_t _opcode;
	bool _mask;
	uint8_t _maskingKey[4];
	std::vector<uint8_t> _payload;
};

}

#endif
