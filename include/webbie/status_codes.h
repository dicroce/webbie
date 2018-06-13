
#ifndef webbie_status_codes_h
#define webbie_status_codes_h

namespace webbie
{

enum status_code
{
    response_continue                = 100,
    response_switching_protocols     = 101,

    /// SUCCESS RESPONSES
    response_ok                      = 200, ///< Request has succeeded.
    response_created                 = 201, ///< Request succeeded and resource created.
    response_accepted                = 202, ///< Request accepted for processing.
    response_no_content              = 204, ///< Request succeeded, but no content.
    response_reset_content           = 205, ///< Agent should reset its document view.

    /// MISC RESPONSES
    response_multiple_choices        = 300, ///< Resource has multiple choices.
    response_moved                   = 301, ///< Resource moved permanently.
    response_found                   = 302, ///< Response body should contain temporary URI.

    /// ERROR RESPONSES
    response_bad_request             = 400, ///< Request could not be understood by server.
    response_unauthorized            = 401, ///< Request requires user authentication.
    response_forbidden               = 403, ///< Request was refused.
    response_not_found               = 404, ///< Resource not found.
    response_method_not_allowed      = 405, ///< Could not use specified method for this resource.
    response_gone                    = 410, ///< Resource no longer available, no forwarding address.
    response_length_required         = 411, ///< Request requires Content-Length header field.

    /// SERVER ERROR RESPONSES
    response_internal_server_error   = 500, ///< The server encountered an unexpected condition.
    response_not_implemented         = 501, ///< Server does not support the functionality to fulfill the request.
    response_bad_gateway             = 502, ///< Server received an invalid response from upstream server.
    response_service_unavailable     = 503, ///< Server temporarily unable to handle request.
    response_http_version_not_supported = 505 ///< Server does not support requested HTTP protocol version.
};

}

#endif
