#include "webbie/methods.h"
#include "webbie/http_exception.h"

using namespace cppkit;
using namespace std;

string webbie::method_text( int method )
{
    if( method == METHOD_GET )
        return "GET";
    else if( method == METHOD_POST )
        return "POST";
    else if( method == METHOD_PUT )
        return "PUT";
    else if( method == METHOD_DELETE )
        return "DELETE";
    else if( method == METHOD_PATCH )
        return "PATCH";
    else if( method == METHOD_HEAD )
        return "HEAD";

    CK_STHROW( webbie_exception_generic, ("Unsupported method.") );
}

int webbie::method_type( const string& methodText )
{
    string lowerMethod = ck_string_utils::to_lower(methodText);

    if( lowerMethod == "get" )
        return METHOD_GET;
    else if( lowerMethod == "post" )
        return METHOD_POST;
    else if( lowerMethod == "put" )
        return METHOD_PUT;
    else if( lowerMethod == "delete" )
        return METHOD_DELETE;
    else if( lowerMethod == "patch" )
        return METHOD_PATCH;
    else if( lowerMethod == "head" )
        return METHOD_HEAD;

    CK_STHROW( webbie_exception_generic, ("Unsupported method.") );
}
