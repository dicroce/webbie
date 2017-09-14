
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

#include "webbie/webbie_utils.h"
#include "webbie/uri.h"
#include "webbie/webbie_exception.h"

using namespace cppkit;
using namespace std;
using namespace webbie;

void webbie::parse_url_parts( const ck_string url, ck_string& host, int& port, ck_string& protocol, ck_string& uri )
{
    // Initially set port based on protocol if present...
    if(url.contains("https://"))
    {
        protocol = "https";
        port = 443;
    }
    else
    {
        protocol = "http";
        port = 80;
    }

    size_t slashSlash = url.find("//");

    size_t urlStart = (slashSlash==string::npos)?0:slashSlash+2;

    size_t uriStart = url.find( "/", urlStart );

    if( uriStart != string::npos )
    {
        host = url.substr( urlStart, uriStart-urlStart );
        uri = url.substr( uriStart );
    }
    else
    {
        host = url.substr(urlStart);
        uri = "/";
    }

    // If our host part contains a colon, then override our earlier port number...
    if( host.contains( ":" ) )
    {
        auto hostParts = host.split( ":" );
        port = hostParts[1].to_int();
        host = hostParts[0];
    }
}

ck_string webbie::adjust_header_name( const ck_string& name )
{
    for( size_t i = 0, e = name.size(); i < e; ++i )
    {
        const char c = name[i];
        if( c < 33 || c > 126 || c == ':' )
            CK_STHROW( webbie_exception, ("Invalid character in header value: [%c]", c) );
    }

    return name.to_lower();
}

ck_string webbie::adjust_header_value( const ck_string& value )
{
    const size_t left = value.find_first_not_of(" \t");
    const size_t right = value.find_last_not_of(" \t");

    if( left == string::npos )
        return "";

    return value.substr( left, right - left + 1 );
}
