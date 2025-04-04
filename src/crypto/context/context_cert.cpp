#include <stdexcept>
#include <memory>
#include <string>

#include <openssl/ssl.h>

#include <sfap.hpp>
#include <crypto/context/context.hpp>
#include <crypto/utils.hpp>
#include <utils/misc.hpp>


using namespace sfap;
using namespace sfap::crypto;


void SSLContext::load_certificate( const std::string& certificate ) {

    check_if_valid();

    std::unique_ptr<BIO, decltype( &BIO_free )> bio( BIO_new_mem_buf( certificate.data(), certificate.size() ), BIO_free );
    if ( !bio ) throw SSLException();

    std::unique_ptr<X509, decltype( &X509_free )> cert( PEM_read_bio_X509( bio.get(), nullptr, nullptr, nullptr ), X509_free );
    if ( !cert ) throw SSLException();

    if ( SSL_CTX_use_certificate( _ctx.get(), cert.get() ) != 1 ) throw SSLException();

}


void SSLContext::load_certificate_file( path_t certificate ) {

    check_if_valid();

    if ( SSL_CTX_use_certificate_file( _ctx.get(), utils::path_to_string( certificate ).c_str(), SSL_FILETYPE_PEM ) != 1 ) throw SSLException();

}