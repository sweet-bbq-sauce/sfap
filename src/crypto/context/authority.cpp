/*!
 *  \file
 *  \brief Implementation of CA certificates loading functions for TLSContext.
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


#include <mutex>
#include <string>

#include <openssl/ssl.h>

#include <sfap.hpp>
#include <crypto/context/context.hpp>
#include <crypto/exception.hpp>
#include <utils/encoding.hpp>


using namespace sfap;
using namespace sfap::crypto;


void TLSContext::add_ca_string( const std::string& ca ) {

    std::unique_lock lock( _modify_mutex );

    std::unique_ptr<BIO, decltype( &BIO_free )> bio( BIO_new_mem_buf( ca.data(), static_cast<int>( ca.size() ) ), &BIO_free );
    if ( !bio ) {

        throw OpenSSLError( "can't create BIO (BIO_new_mem_buf)" );

    }

    X509_STORE* store = SSL_CTX_get_cert_store( _ctx.get() );
    while ( true ) {

        std::unique_ptr<X509, decltype( &X509_free )> cert( PEM_read_bio_X509( bio.get(), nullptr, nullptr, nullptr ), &X509_free );
        
        if ( !cert ) {

            if ( BIO_eof( bio.get() ) ) break;

            throw OpenSSLError( "can't get CA from BIO" );

        }

        if ( X509_STORE_add_cert( store, cert.get() ) != 1 ) {

            throw OpenSSLError( "can't add CA cert to store (X509_STORE_add_cert)" );

        }

    }

}


void TLSContext::add_ca_file( const path_t& ca ) {

    std::unique_lock lock( _modify_mutex );

    if ( SSL_CTX_load_verify_locations( _ctx.get(), utils::path_to_string( ca ).c_str(), nullptr ) != 1 ) {

        throw std::runtime_error( "can't load CA cert from file '" + ca.string() + "'" );

    }

}


void TLSContext::add_ca_path( const path_t& ca_directory ) {

    std::unique_lock lock( _modify_mutex );

    if ( SSL_CTX_load_verify_locations( _ctx.get(), nullptr, utils::path_to_string( ca_directory ).c_str() ) != 1 ) {

        throw std::runtime_error( "can't load CA path '" + ca_directory.string() + "'" );

    }

}


void TLSContext::add_ca_string_array( const std::vector<std::string>& ca_list ) {

    for ( const auto& ca : ca_list ) {

        add_ca_string( ca );

    }

}


void TLSContext::add_ca_file_array( const std::vector<path_t>& ca_files ) {

    for ( const auto& path : ca_files ) {

        add_ca_file( path );

    }

}