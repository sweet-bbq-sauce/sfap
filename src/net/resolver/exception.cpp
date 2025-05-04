/*!
 *  \file
 *  \brief Implementation of the ResolverError class.
 *
 *  This file provides the implementation of the ResolverError exception class,
 *  which wraps platform-specific DNS error codes and provides meaningful messages.
 *
 *  \copyright Copyright (c) 2025 Wiktor Sołtys
 *
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
 */


#include <stdexcept>
#include <string>

#ifdef _WIN32

    #include <winsock2.h>
    #include <ws2tcpip.h>

#else

    #include <netdb.h>

#endif

#include <sfap.hpp>
#include <net/resolver/resolver.hpp>


using namespace sfap;
using namespace sfap::net;


ResolverError::ResolverError( int code ) noexcept :
    std::runtime_error( get_message( code ) ),
    _code( code )
{}


int ResolverError::code() const noexcept {

    return _code;

}


std::string ResolverError::get_message( int code ) noexcept {

    #ifdef _WIN32

        const char* msg = gai_strerrorA( code );

    #else

        const char* msg =  gai_strerror( code );

    #endif

    return msg ? msg : "unknown error";

}