
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

#ifndef webbie_web_socket_h
#define webbie_web_socket_h

#include "webbie/web_socket_frame.h"
#include <thread>

namespace webbie
{

template<class SOCK_T>
class web_socket
{
public:
    CK_API enum web_socket_mode
    {
        kweb_socket_mode_server = 0,
        kweb_socket_mode_client = 1
    };

    static const size_t MAX_PAYLOAD_LENGTH = 1073741824;//1GB

    CK_API web_socket(SOCK_T&& sok) :
        _sok(std::move(sok)),
        _running(false)
    {
    }

    CK_API web_socket(web_socket&& obj) throw()
    {
        _sok = std::move(obj._sok);
        _running = std::move(obj._running);
    }

    CK_API ~web_socket() throw()
    {
    }

    CK_API web_socket& operator = (web_socket&& obj) throw()
    {
        _sok = std::move(obj._sok);
        _running = std::move(obj._running);
        obj._running = false;

        return *this;
    }

#if 0
    CK_API virtual void OnOpen();
    CK_API virtual void OnClose( std::shared_ptr<web_socket_frame> frame );
    CK_API virtual void OnMessage( std::shared_ptr<web_socket_frame> frame );
    CK_API virtual void OnPing( std::shared_ptr<web_socket_frame> frame );
    CK_API virtual void OnPong( std::shared_ptr<web_socket_frame> frame );

    CK_API static void PingTimerCB(void* opaque);
    CK_API void SendPing();
    CK_API void SendPong();
    CK_API void SendClose();

    CK_API std::shared_ptr<web_socket_frame> RecvFrame();
    CK_API bool SendFrame( std::shared_ptr<web_socket_frame> frame );
    CK_API bool Connect( XSDK::XString uri );
    CK_API bool Connect( XSDK::XString uri,  const XRef<XSDK::XHash<XSDK::XString> > additionalHeaders );
    CK_API bool Accept(std::shared_ptr<WEBBY::ServerSideRequest> request, std::shared_ptr<WEBBY::ServerSideResponse> response);

    CK_API bool ValidateHandshake( std::shared_ptr<WEBBY::ClientSideResponse> response );
    CK_API void Shutdown();

    // Send creates a binary frame
    CK_API bool Send( std::shared_ptr<XSDK::XMemory> data );

    // Send creates a text frame
    CK_API bool Send( const XSDK::XString& str );
    CK_API std::shared_ptr<XSDK::XMemory> Recv();

    // can currently register callbacks for onclose, onping, onpong, ontextframe, and onbinaryframe
    CK_API void RegisterCallback(XSDK::XString type, void *opaque, PayloadCallback callback);
    CK_API void UnRegisterCallback(XSDK::XString type);

    //Raw Send/Recv Methods
    CK_API std::shared_ptr<web_socket_frame> ReadFrame( XSDK::XStreamIO& socket );
    CK_API bool WriteFrame( XSDK::XStreamIO& socket, std::shared_ptr<web_socket_frame> frame );

    CK_API XRef<XSDK::XStreamIO> GetSocket();
#endif

private:
    web_socket_frame _recv_frame()
    {
        web_socket_frame frame;

        uint8_t finOpcode;
        _sok.recv(&finOpcode, 1);
        frame.set_fin(finOpcode & 128);
        frame.set_opcode(finOpcode & 15);

        uint8_t maskPayloadLen;
        _sok.recv(&maskPayloadLen, 1);
        frame.set_mask(maskPayloadLen & 128);

        uint64_t payloadLength;
        if((maskPayloadLen & 127) <= 125)
            payloadLength = maskPayloadLen & 127;
        else if((maskPayloadLen & 127) == 126)
        {
            uint16_t temp;
            _sok.recv(&temp, 2);
            payloadLength = ntohs(temp);
        }
        else if((maskPayloadLen & 127) == 127)
        {
            uint64_t doubleWord;
            sok.recv(&doubleWord, sizeof(doubleWord));
            payloadLength = x_ntohll(doubleWord);
            if(payloadLength > MAX_PAYLOAD_LENGTH)
            {
                CK_LOG_ERROR("WEBSOCKET MAX_PAYLOAD_LENGTH HIT!!!CLOSING WEBSOCKET CONNECTION");
                web_socket_frame closeFrame;
                closeFrame->set_opcode(kweb_socket_opcode_connection_close);
                return closeFrame;
            }
        }

        uint8_t maskingKey[4];
        if(frame.is_mask())
        {
            for(int i=0; i < 4; ++i)
            {
                uint8_t temp;
                _sok.recv(&temp, 1);
                maskingKey[i] = temp;
            }
            frame->set_masking_key(maskingKey);
        }

        if(payloadLength > 0)
        {
            _sok.recv(&frame.extend(payloadLength), payloadLength);

            if(frame.is_mask())
            {
                // stopped here
            }
        }

        return frame;
    }
    void _entry_point()
    {
        int timeoutCounter = 0;
        web_socket_frame currentFrame;
        web_socket_frame fragmentFrame;

        _running = true;

        while(_running)
        {
            uint64_t timeout = 1000;
            if(_sok.recv_wont_block(timeout))
            {
                if(!_running)
                    break;

                currentFrame = RecvFrame();
            }
        }
    }

    SOCK_T _sok;
    bool _running;

#if 0
    web_socket_mode _mode;

    web_socket& operator = ( const web_socket& obj );
    web_socket( const web_socket& obj );

    bool _waitingForPong;
    bool _sentClose;
    bool _running;

    XSDK::XTimer _pingTimer;
    std::shared_ptr<XSDK::XMemory> _web_socketKey;
    XRef<XSDK::XStreamIO> _clientSocket;
    XSDK::XMutex _callbackHashMutex;
    XSDK::XHash<PayloadCBInfo> _callbackHash;

    XSDK::XMutex _recvLock;
    XSDK::XMutex _sendLock;
#endif
};

}
