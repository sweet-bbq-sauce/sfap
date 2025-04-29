/*!
 *  \file
 *  \brief Source file containing network address type detection.
 *
 *  This file defines function `detect_address_function()`.
 *
 *  \copyright Copyright (c) 2025 Wiktor Sołtys
 *
 *  \cond
 *  MIT License
 * 
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  \endcond
 */


#include <regex>
#include <string>

#ifdef _WIN32

    #include <winsock2.h>
    #include <ws2tcpip.h>

#else

    #include <arpa/inet.h>
    #include <netinet/in.h>
    
#endif

#include <sfap.hpp>
#include <net/address/detect.hpp>


using namespace sfap;
using namespace sfap::net;


address_type net::detect_address_type( const std::string& address ) noexcept {

    // If address is empty return address_type::EMPTY.
    if ( address.empty() ) return address_type::EMPTY;

    // If address size is greater than 254 return address_type::UNSUPPORTED.
    if ( address.size() > 254 ) return address_type::UNSUPPORTED;   // 255 with null-byte.

    // Check if address is IPv4 with inet_pton.
    sockaddr_in sa4 {};
    if ( inet_pton( AF_INET, address.c_str(), &sa4.sin_addr ) == 1 ) return address_type::IPV4;

    // Check if address is IPv6 with inet_pton.
    sockaddr_in6 sa6 {};
    if ( inet_pton( AF_INET6, address.c_str(), &sa6.sin6_addr ) == 1 ) return address_type::IPV6;

    // Check if address is valid RFC 1034 hostname with std::regex.
    static const std::regex hostname_pattern(

        R"(^([a-zA-Z0-9]([a-zA-Z0-9\-]{0,61}[a-zA-Z0-9])?\.)*[a-zA-Z0-9]([a-zA-Z0-9\-]{0,61}[a-zA-Z0-9])?$)",
        std::regex::ECMAScript | std::regex::icase

    );
    if ( std::regex_match( address, hostname_pattern ) ) return address_type::HOSTNAME;
    
    return address_type::UNSUPPORTED;

}