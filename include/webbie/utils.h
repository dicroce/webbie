#ifndef webbie_cppkit_h
#define webbie_cppkit_h

#include "cppkit/ck_string_utils.h"

namespace webbie
{

void parse_url_parts( const std::string url, std::string& host, int& port, std::string& protocol, std::string& uri );

std::string adjust_header_name( const std::string& value );

std::string adjust_header_value( const std::string& value );

}

#endif