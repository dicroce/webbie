
#ifndef _webbie_uri_h
#define _webbie_uri_h

#include "cppkit/ck_exception.h"
#include "cppkit/ck_string_utils.h"
#include <map>

namespace webbie
{

class uri
{
public:
    uri() :
        _fullRawURI("/"),
        _getArgs(),
        _resource(),
        _resourcePath("/")
    {
    }

    uri( const std::string& fullURI );

    uri( const char* cstr ) :
        _fullRawURI( "/" ),
        _getArgs(),
        _resource(),
        _resourcePath( "/" )
    {
        *this = uri( std::string( cstr ) );
    }

    uri( const uri& obj ) :
        _fullRawURI( obj._fullRawURI ),
        _getArgs( obj._getArgs ),
        _resource( obj._resource ),
        _resourcePath( obj._resourcePath )
    {
    }

    virtual ~uri() noexcept {}

    static uri construct_uri( const std::string& resourcePath,
                                const std::string& resource );

    static uri construct_uri( const std::string& resourcePath,
                                const std::string& resource,
                                const std::map<std::string,std::string>& getArgs );

    bool operator == ( const uri& other ) const;

    bool operator != ( const uri& other ) const;

    bool operator < ( const uri& other ) const;

    std::string get_full_raw_uri() const { return _fullRawURI; }

    const std::map<std::string, std::string>& get_get_args() const { return _getArgs; }

    std::string get_resource() const { return _resource; }

    std::string get_resource_path() const { return _resourcePath; }

    std::string get_full_resource_path() const;

    void add_get_arg( const std::string& name, const std::string& value );

    void clear_get_args();

    void set_full_raw_uri( const std::string& fullURI );

    void set_resource( const std::string& resource );

    void set_resource_path( const std::string& resourcePath );

    void set_get_args( const std::map<std::string, std::string>& getArgs );

private:
    static std::string _construct_full_raw_uri( const std::string& resourcePath,
                                                const std::string& resource,
                                                const std::map<std::string, std::string>& getArgs );

    /// @brief Takes a uri and returns its getarguments.
    static std::map<std::string, std::string> _parse_get_args( const std::string& fullURI );

    /// @brief Takes a uri and returns the resource portion decoded.
    static std::string _parse_resource( const std::string& fullURI );

    /// @brief Takes a uri and returns the resource path portion decoded.
    static std::string _parse_resource_path( const std::string& fullURI );

    /// @brief Makes sure that the given string begins with a forward slash.
    ///
    /// @return uri with a forward slash prepended onto it if it
    ///         didn't have one before.
    static std::string _verify_starting_slash( const std::string& uri );

    std::string _fullRawURI;
    std::map<std::string, std::string> _getArgs;
    std::string _resource;
    std::string _resourcePath;
};

}

#endif
