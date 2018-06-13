
#include "webbie/uri.h"

#include <map>
#include <iostream>

#include "cppkit/ck_string_utils.h"
#include "webbie/http_exception.h"

using namespace std;
using namespace cppkit;
using namespace webbie;

uri::uri( const string& fullURI ) :
    _fullRawURI( _verify_starting_slash( fullURI ) ),
    _getArgs( _parse_get_args( _fullRawURI ) ),
    _resource( _parse_resource( _fullRawURI ) ),
    _resourcePath( _parse_resource_path( _fullRawURI ) )
{
    _fullRawURI = _construct_full_raw_uri( _resourcePath, _resource, _getArgs );
}

uri uri::construct_uri( const string& resourcePath,
                        const string& resource )
{
    uri retVal;
    retVal.set_resource_path( resourcePath );
    retVal.set_resource( resource );

    return retVal;
}


uri uri::construct_uri( const string& resourcePath,
                        const string& resource,
                        const map<string, string>& getArgs )
{
    uri retVal;

    retVal.set_resource_path( resourcePath );
    retVal.set_resource( resource );
    retVal.set_get_args( getArgs );

    return retVal;
}

bool uri::operator == ( const uri& other ) const
{
    return _fullRawURI == other._fullRawURI;
}

bool uri::operator != ( const uri& other ) const
{
    return _fullRawURI != other._fullRawURI;
}

bool uri::operator < ( const uri& other ) const
{
    return _fullRawURI < other._fullRawURI;
}

string uri::get_full_resource_path() const
{
    return _resourcePath + _resource;
}

void uri::add_get_arg( const string& name, const string& value )
{
    if(name.empty())
        CK_STHROW(webbie_exception_generic, ("Empty get argument name."));
    if(value.empty())
        CK_STHROW(webbie_exception_generic, ("Empty get argument value."));

    _getArgs[name] = value;
    _fullRawURI = _construct_full_raw_uri(_resourcePath, _resource, _getArgs);
}


void uri::clear_get_args()
{
    map<string, string> getArgs = _getArgs;

    //Doing it roundabout like this is a bit convoluted, but it guarantees
    //that we won't end up in an inconsistent state if either Clear or
    //_ConstructFullRawuri throws.
    getArgs.clear();
    _fullRawURI = _construct_full_raw_uri( _resourcePath, _resource, getArgs );
    _getArgs = getArgs;
}


void uri::set_full_raw_uri(const string& fullURI)
{
    const string slashedURI = _verify_starting_slash(fullURI);

    map<string, string> getArgs = _parse_get_args(slashedURI);
    const string resource = _parse_resource(slashedURI);
    const string resourcePath = _parse_resource_path(slashedURI);

    //Nothing is set until all functions are called in order
    //to avoid leaving the object in an inconsistent state
    //if an exception is thrown.
    _fullRawURI = _construct_full_raw_uri(resourcePath, resource, getArgs);
    _getArgs = getArgs;
    _resource = resource;
    _resourcePath = resourcePath;
}


void uri::set_resource(const string& resource)
{
    if(resource != ck_string_utils::uri_encode(resource))
        CK_STHROW(webbie_exception_generic, ("Resource can only have alphanumeric characters."));

    //Assignment order ensures that the object stays in a valid state.
    _fullRawURI = _construct_full_raw_uri(_resourcePath, resource, _getArgs);
    _resource = resource;
}


void uri::set_resource_path(const string& resourcePath)
{
    {
        const string unslashedResourcePath = ck_string_utils::erase_all(resourcePath, '/');

        if(unslashedResourcePath != ck_string_utils::uri_encode(unslashedResourcePath))
            CK_STHROW(webbie_exception_generic, ("Resource Path can only have slashes and alphanumeric characters."));
    }

    string slashedResourcePath = _verify_starting_slash(resourcePath);

    //Verify ending slash.
    if(slashedResourcePath[slashedResourcePath.size()-1] != '/')
        slashedResourcePath.push_back('/');

    //Assignment order ensures that the object stays in a valid state.
    _fullRawURI = _construct_full_raw_uri(slashedResourcePath, _resource, _getArgs);
    _resourcePath = slashedResourcePath;
}


void uri::set_get_args(const map<string, string>& getArgs)
{
    _fullRawURI = _construct_full_raw_uri(_resourcePath, _resource, getArgs);
    _getArgs = getArgs;
}


//------------------------------------------------------------------------------
// Static Helper Functions
//------------------------------------------------------------------------------

string uri::_construct_full_raw_uri(const string& resourcePath,
                                       const string& resource,
                                       const map<string, string>& getArgs)
{
    string fullRawURI = resourcePath;

    fullRawURI.append(resource);

    if(!getArgs.empty())
    {
        fullRawURI.push_back('?');

        map<string, string> sortedGetArgs;

        for( auto iter = getArgs.begin();
             iter != getArgs.end();
             iter++ )
        {
            const string name = (*iter).first;
            const string value = (*iter).second;

            if(name.empty())
                CK_STHROW(webbie_exception_generic, ("Empty get argument name."));
            if(value.empty())
                CK_STHROW(webbie_exception_generic, ("Empty get argument value."));

            sortedGetArgs[name] = value;
        }

        size_t i = 0;
        const size_t size = sortedGetArgs.size();
        for(auto iter = sortedGetArgs.begin();
            iter != sortedGetArgs.end();
            iter++)
        {
            fullRawURI.append( ck_string_utils::uri_encode(string((*iter).first)) );
            fullRawURI.push_back('=');
            fullRawURI.append( ck_string_utils::uri_encode((*iter).second) );

            if(++i < size)
                fullRawURI.push_back('&');
        }
    }

    return fullRawURI;
}


map<string, string> uri::_parse_get_args(const string& fullURI)
{
    map<string, string> getArgs;

    const size_t questionIndex = fullURI.find('?');

    if(questionIndex != string::npos)
    {
        size_t argLen = fullURI.size() - (questionIndex + 1);
        const string wholeArgs = ck_string_utils::replace_all(fullURI.substr(questionIndex+1, argLen), '?', '&');

        const vector<string> nvPairs = ck_string_utils::split(wholeArgs, "&");

        for( auto iter = nvPairs.begin(), end  = nvPairs.end();
             iter != end;
             ++iter )
        {
            const vector<string> nvParts = ck_string_utils::split(*iter, '=');

            if(nvParts.size() == 2)
            {
                const string& name = nvParts[0];
                const string& value = nvParts[1];

                getArgs[ck_string_utils::uri_decode(name)] = ck_string_utils::uri_decode(value);
            }
        }
    }

    return getArgs;
}


string uri::_parse_resource(const string& fullRawURI)
{
    size_t questionIndex = fullRawURI.find("?");

    if(questionIndex == string::npos)
        questionIndex = fullRawURI.size();

    // The goal is to determine the index of the start of the resource name. When we FindLast()
    // from the right their are 3 possible cases:
    //   1 - We find the rightmost / in a path consisting of many sub dirs.
    //   2 - We find the rightmost / in a path that consists of only a single /.
    //   3 - We find NO /, so FindLast() returns -1.
    // In all 3 of these cases, incrementing the lastSlashIndex is the right thing to do!

    size_t lastSlashIndex = fullRawURI.substr(0, questionIndex).rfind("/") + 1;
    size_t len = 0;

    if((fullRawURI.size() > 1) && ((fullRawURI.size() == lastSlashIndex) || (lastSlashIndex == questionIndex)))
    {
        lastSlashIndex = fullRawURI.substr(0, lastSlashIndex - 1).rfind("/") + 1;
        len = questionIndex - lastSlashIndex - 1;
    }
    else
    {
        len = questionIndex - lastSlashIndex;
    }

    const string resource = fullRawURI.substr(lastSlashIndex, len);

    return resource;
}


string uri::_parse_resource_path(const string& fullRawURI)
{
    // Get Question Mark Index
    size_t questionIndex = fullRawURI.find("?");

    if(questionIndex == string::npos)
        questionIndex = fullRawURI.size();

    string tempURI = fullRawURI;
    tempURI = fullRawURI.substr(0,questionIndex);

    // If we have a slash as the last character, chop it off
    if(tempURI.size() > 1 && tempURI.size() == tempURI.rfind("/") + 1)
        tempURI = fullRawURI.substr(0,tempURI.rfind("/"));

    const string resourcePath = tempURI.substr(0, tempURI.rfind("/") + 1);

    return resourcePath;
}


string uri::_verify_starting_slash(const string& uri)
{
    if(uri.empty())
        return "/";
    else if(uri[0] != '/')
        return "/" + uri;

    return uri;
}
