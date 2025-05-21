/*!
 *  \file
 *  \brief Implementation of the TLSContext class for managing OpenSSL SSL_CTX.
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


using namespace sfap;
using namespace sfap::crypto;


TLSContext::TLSContext( int mode, bool system_ca ) :
    _ctx( SSL_CTX_new( TLS_method() ), &SSL_CTX_free )
{

    if ( !_ctx ) {
        
        throw OpenSSLError( "can't create SSL CTX (SSL_CTX_new)" );

    }

    set_min_proto_version( TLS1_3_VERSION );

    set_verify_mode( mode );

    if ( system_ca ) {

        add_system_ca();

    }

}


TLSContext::~TLSContext() {

    std::unique_lock lock( _callbacks_mutex );

    _callbacks.erase( _ctx.get() );

}


TLSContext::TLSContext( TLSContext&& other ) noexcept :
    _ctx( std::move( other._ctx ) )
{}


TLSContext& TLSContext::operator=( TLSContext&& other ) noexcept {

    if ( this != &other ) {

        _ctx = std::move( other._ctx );
        
    }

    return *this;

}


void TLSContext::set_verify_mode( int mode, std::optional<std::function<int( int, X509_STORE_CTX* )>> callback ) noexcept {

    std::unique_lock modify_lock( _modify_mutex );
    std::unique_lock callbacks_lock( _callbacks_mutex );

    SSL_CTX_set_verify( _ctx.get(), mode, callback ? &TLSContext::_verify_callback : nullptr );

    if ( callback ) {

        _callbacks.emplace( _ctx.get(), std::move( callback.value() ) );

    }
    else {
        
        _callbacks.erase( _ctx.get() );

    }
    
}


void TLSContext::set_options( long options ) {
    
    std::unique_lock lock( _modify_mutex );

    if ( ( SSL_CTX_set_options( _ctx.get(), options ) & options ) != options ) {

        throw OpenSSLError( "not all options could be set (SSL_CTX_set_options)" );

    }

}


void TLSContext::set_cipher_list( const std::string& ciphers ) {

    std::unique_lock lock( _modify_mutex );

    if ( SSL_CTX_set_cipher_list( _ctx.get(), ciphers.c_str() ) != 1 ) {

        throw OpenSSLError( "can't set cipher list (SSL_CTX_set_cipher_list)" );

    }

}


void TLSContext::set_cipher_suites( const std::string& ciphersuites ) {

    std::unique_lock lock( _modify_mutex );

    if ( SSL_CTX_set_ciphersuites( _ctx.get(), ciphersuites.c_str() ) != 1 ) {

        throw OpenSSLError( "can't set cipher suites (SSL_CTX_set_ciphersuites)" );

    }

}


void TLSContext::set_min_proto_version( int version ) {

    std::unique_lock lock( _modify_mutex );

    if ( SSL_CTX_set_min_proto_version( _ctx.get(), version ) != 1 ) {

        throw OpenSSLError( "can't set minimum protocol version (SSL_CTX_set_min_proto_version)" );

    }

}


void TLSContext::set_max_proto_version( int version ) {

    std::unique_lock lock( _modify_mutex );

    if ( SSL_CTX_set_max_proto_version( _ctx.get(), version ) != 1 ) {

        throw OpenSSLError( "can't set maximum protocol version (SSL_CTX_set_max_proto_version)" );

    }

}


SSL_CTX* TLSContext::get() const noexcept {

    return _ctx.get();

}


ssl_ptr TLSContext::create_ssl() const {

    std::shared_lock lock( _modify_mutex );

    ssl_ptr ssl = ssl_ptr( SSL_new( _ctx.get() ), &SSL_free );

    if ( ssl.get() == nullptr ) {

        throw OpenSSLError( "can't create SSL (SSL_new)" );

    }

    return ssl;

}


ssl_ptr TLSContext::operator()() const {

    return create_ssl();

}


int TLSContext::_verify_callback( int preverify_ok, X509_STORE_CTX* store_ctx ) {

    const SSL* ssl = reinterpret_cast<SSL*>( X509_STORE_CTX_get_ex_data( store_ctx, SSL_get_ex_data_X509_STORE_CTX_idx() ) );
    const SSL_CTX* ssl_ctx = SSL_get_SSL_CTX( ssl );

    std::shared_lock lock( _callbacks_mutex );

    const auto it = _callbacks.find( ssl_ctx );
    if ( it != _callbacks.end() ) {

        return it->second( preverify_ok, store_ctx );

    }

    return preverify_ok;

}


std::map<const SSL_CTX*, std::function<int( int, X509_STORE_CTX* )>> TLSContext::_callbacks;
std::shared_mutex TLSContext::_callbacks_mutex;
const TLSContext TLSContext::default_client_context( SSL_VERIFY_PEER );
