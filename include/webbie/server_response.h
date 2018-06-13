
#ifndef webbie_r_server_response_h
#define webbie_r_server_response_h

#include "webbie/status_codes.h"
#include "cppkit/interfaces/ck_stream_io.h"
#include "cppkit/ck_string_utils.h"
#include <vector>
#include <list>
#include <map>

namespace webbie
{

class r_server_response
{
public:
    r_server_response(status_code status = response_ok,
                      const std::string& contentType = "text/plain");

    r_server_response(const r_server_response& obj);

    virtual ~r_server_response() noexcept;

    r_server_response& operator=(const r_server_response& obj);

    void set_status_code(status_code status);
    status_code get_status_code() const;

    void set_connection_close(bool connectionClose) { _connectionClose = connectionClose; }
    bool get_connection_close() const { return _connectionClose; }

//
// Possible Content-Type: values...
//     "text/plain"
//     "image/jpeg"
//     "application/xhtml+xml"
//     "application/x-rtsp-tunnelled"
//     "multipart/mixed; boundary="foo"

    void set_content_type(const std::string& contentType);
    std::string get_content_type() const;

    void set_body(std::vector<uint8_t>&& body);
    void set_body(const std::string& body);
    void set_body( size_t bodySize, const void* bits );

    size_t get_body_size() const;
    const void* get_body() const;

    std::string get_body_as_string() const;

    void clear_additional_headers();
    void add_additional_header(const std::string& headerName,
                               const std::string& headerValue);
    std::string get_additional_header(const std::string& headerName);

    bool written() const { return _responseWritten; }

    void write_response(cppkit::ck_stream_io& socket);

    // Chunked transfer encoding support...
    void write_chunk(cppkit::ck_stream_io& socket, size_t sizeChunk, const void* bits);
    void write_chunk_finalizer(cppkit::ck_stream_io& socket);

    // Multipart mimetype support
    // WritePart() will automaticaly add a Content-Length header per
    // part.

    void write_part( cppkit::ck_stream_io& socket,
                     const std::string& boundary,
                     const std::map<std::string,std::string>& partHeaders,
                     void* chunk,
                     uint32_t size );

    void write_part_finalizer(cppkit::ck_stream_io& socket, const std::string& boundary);

private:
    std::string _get_status_message(status_code sc) const;
    bool _write_header(cppkit::ck_stream_io& socket);

    status_code _status;
    bool _connectionClose;
    std::string _contentType;
    mutable std::vector<uint8_t> _body;
    bool _headerWritten;
    std::map<std::string,std::string> _additionalHeaders;
    bool _responseWritten;
};

}

#endif
