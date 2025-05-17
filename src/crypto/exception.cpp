/*!
 *  \file
 *  \brief OpenSSL errors handling.
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
#include <stdexcept>
#include <string>

#include <openssl/bio.h>
#include <openssl/err.h>

#include <crypto/exception.hpp>


using namespace sfap::crypto;


OpenSSLError::OpenSSLError( const std::string& prefix ) :
    std::runtime_error( prefix + ": " + get_openssl_errors() )
{}


std::string OpenSSLError::get_openssl_errors() {

    std::unique_ptr<BIO, decltype( &BIO_free )> bio( BIO_new( BIO_s_mem() ), &BIO_free );
    if ( !bio ) {

        throw std::runtime_error( "can't create BIO (BIO_new)" );

    }

    ERR_print_errors( bio.get() );

    char* buffer;
    const auto length = BIO_get_mem_data( bio.get(), &buffer );

    return std::string( buffer, length );

}