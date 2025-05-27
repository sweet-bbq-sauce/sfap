/*!
 *  \file
 *  \brief Source file containing Socks5 connect procedure.
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


#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#include <openssl/bio.h>

#include <net/address/host.hpp>
#include <net/iosocket/iosocket.hpp>
#include <net/proxy/proxy.hpp>
#include <utils/credentials.hpp>


using namespace sfap;
using namespace sfap::net;


void Proxy::_open_http_connect( const Host& target, const IOSocket& sock ) const {

    std::string request =

        "CONNECT " + target.to_string() + " HTTP/1.1\r\n"
        "Host: " + target.to_string() + "\r\n"

    ;

    if ( _credentials ) {

        BIO* bio = BIO_new( BIO_s_mem() );
        BIO* b64 = BIO_new( BIO_f_base64() );

        BIO_set_flags( b64, BIO_FLAGS_BASE64_NO_NL );
        bio = BIO_push( b64, bio );

        const auto& credentials = _credentials.value().get();
        const std::string_view user = credentials.get_user();
        const std::string_view password = credentials.get_password();

        BIO_write( bio, user.data(), static_cast<int>( user.size() ) );
        BIO_write( bio, ":", 1 );
        BIO_write( bio, password.data(), static_cast<int>( password.size() ) );

        BIO_flush( bio );

        BUF_MEM* buffer_ptr;
        BIO_get_mem_ptr( bio, &buffer_ptr );

        const std::string encoded_credentials( buffer_ptr->data, buffer_ptr->length );

        BIO_free_all( bio );

        request += "Proxy-Authorization: Basic " + encoded_credentials + "\r\n";

    }

    request += "\r\n";

    sock.send( request.data(), static_cast<dword_t>( request.size() ) );

    std::string response;

    while ( response.find( "\r\n\r\n" ) == std::string::npos ) {

        response += sock.recvo<char>();

        if ( response.size() > 1024 ) {

            throw std::runtime_error( "http proxy server has too long response" );

        }

    }

    if ( response.empty() ) {

        throw std::runtime_error( "http proxy returned empty response" );

    }

    std::istringstream stream( response );
    std::string word;

    stream >> word;     // HTTP version
    if ( word.find( "HTTP/" ) != 0 ) {

        throw std::runtime_error( "unexpected behavior from http proxy server" );

    }

    stream >> word;     // Response code
    if ( word != "200" ) {

        throw std::runtime_error( "http proxy server refuses request: " + response );

    }

    // OK

}