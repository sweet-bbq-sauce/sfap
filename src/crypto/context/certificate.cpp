/*!
 *  \file
 *  \brief Implementation of certificate loading functions for TLSContext.
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
#include <mutex>
#include <string>

#include <openssl/ssl.h>

#include <sfap.hpp>
#include <crypto/context/context.hpp>
#include <crypto/exception.hpp>
#include <utils/encoding.hpp>


using namespace sfap;
using namespace sfap::crypto;


void TLSContext::load_cert_string( const std::string& certificate ) {

    std::unique_lock lock( _modify_mutex );

    std::unique_ptr<BIO, decltype( &BIO_free )> bio( BIO_new_mem_buf( certificate.data(), static_cast<int>( certificate.size() ) ), &BIO_free );
    if ( !bio ) {

        throw OpenSSLError( "can't create BIO (BIO_new_mem_buf)" );

    }

    std::unique_ptr<X509, decltype( &X509_free )> cert( PEM_read_bio_X509( bio.get(), nullptr, nullptr, nullptr ), &X509_free );
    if ( !cert ) {

        throw OpenSSLError( "can't create X509 from string (PEM_read_bio_X509)" );

    }

    if ( SSL_CTX_use_certificate( _ctx.get(), cert.get() ) != 1 ) {

        throw OpenSSLError( "can't set certificate (SSL_CTX_use_certificate)" );

    }

}


void TLSContext::load_cert_file( const path_t& cert ) {

    std::unique_lock lock( _modify_mutex );

    if ( SSL_CTX_use_certificate_file( _ctx.get(), utils::path_to_string( cert ).c_str(), SSL_FILETYPE_PEM ) != 1 ) {

        throw OpenSSLError( "can't load certificate from file '" + cert.string() + "'" );

    }

}