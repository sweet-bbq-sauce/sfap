#include <stdexcept>
#include <memory>
#include <string>
#include <vector>

#include <openssl/ssl.h>
#include <openssl/tls1.h>

#include <sfap.hpp>
#include <crypto/context/context.hpp>
#include <crypto/utils.hpp>


using namespace sfap::crypto;


SSLContext::SSLContext() : _ctx( SSL_CTX_new( TLS_method() ), SSL_CTX_free ) {

    if ( !_ctx ) throw SSLException();

    SSL_CTX_set_options( _ctx.get(), SSL_OP_IGNORE_UNEXPECTED_EOF );

}


SSLContext::SSLContext( SSLContext&& other ) noexcept : _ctx( std::move( other._ctx ) ) {}


SSLContext::SSLContext( const std::string& private_key, const std::string& certificate, const std::vector<std::string>& ca ) : SSLContext() {

    load_private_key( private_key );
    load_certificate( certificate );
    load_ca_certificate_list( ca );

}


SSLContext::SSLContext( path_t private_key, path_t certificate, const std::vector<path_t>& ca ) : SSLContext() {

    load_private_key_file( private_key );
    load_certificate_file( certificate );
    load_ca_certificate_file_list( ca );

}


void SSLContext::set_verify_mode( bool verify ) {

    check_if_valid();

    SSL_CTX_set_verify( _ctx.get(), verify ? SSL_VERIFY_PEER : SSL_VERIFY_NONE, nullptr );

}


bool SSLContext::valid_key_and_cert() const {

    check_if_valid();

    return SSL_CTX_check_private_key( _ctx.get() ) == 1;

}


inline bool SSLContext::valid() const {

    return _ctx != nullptr;

}


inline SSLContext::operator bool() const {

    return this->valid();

}


SSL_t SSLContext::create_ssl() const {

    check_if_valid();

    SSL* ssl = SSL_new( _ctx.get() );

    if ( ssl == nullptr ) throw SSLException();

    return SSL_t( ssl, SSL_free );

}


inline void SSLContext::check_if_valid() const {

    if ( !valid() ) throw std::runtime_error( "This object is null" );

}


const std::shared_ptr<SSLContext> SSLContext::default_client_context = std::make_shared<SSLContext>();