#ifndef webbie_methods_h
#define webbie_methods_h

#include <string>

namespace webbie
{

enum webbie_method
{
    METHOD_GET = 1,
    METHOD_POST = 2,
    METHOD_PUT = 3,
    METHOD_DELETE = 4,
    METHOD_PATCH = 5,
    METHOD_HEAD = 6
};

std::string method_text( int method );

int method_type( const std::string& methodText );

}

#endif