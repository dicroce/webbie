
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
