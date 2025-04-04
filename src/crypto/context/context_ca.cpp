#include <stdexcept>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>

#include <openssl/ssl.h>

#include <sfap.hpp>
#include <crypto/context/context.hpp>
#include <crypto/utils.hpp>
#include <utils/misc.hpp>


using namespace sfap;
using namespace sfap::crypto;


void SSLContext::load_ca_certificate( const std::string& ca_certificate ) {

    check_if_valid();

    std::unique_ptr<BIO, decltype( &BIO_free )> bio( BIO_new_mem_buf( ca_certificate.data(), ca_certificate.size() ), BIO_free );
    if ( !bio ) throw SSLException();

    std::unique_ptr<X509, decltype( &X509_free )> cert( PEM_read_bio_X509( bio.get(), nullptr, nullptr, nullptr ), X509_free );
    if ( !cert ) throw SSLException();

    X509_STORE* ca_store = SSL_CTX_get_cert_store( _ctx.get() );
    if ( !ca_store ) throw SSLException();

    if ( X509_STORE_add_cert( ca_store, cert.get() ) != 1 ) throw SSLException();

}


void SSLContext::load_ca_certificate_file( path_t ca_certificate ) {

    check_if_valid();

    if ( SSL_CTX_load_verify_file( _ctx.get(), utils::path_to_string( ca_certificate ).c_str() ) != 1 ) throw SSLException();

}


void SSLContext::load_ca_certificate_list( const std::vector<std::string>& ca_certificate ) {

    check_if_valid();

    for ( const auto& item : ca_certificate ) load_ca_certificate( item );

}


void SSLContext::load_ca_certificate_file_list( const std::vector<path_t>& ca_certificate ) {

    check_if_valid();

    for ( const auto& item : ca_certificate ) load_ca_certificate_file( item );

}


void SSLContext::load_ca_certificate_directory( path_t ca_directory ) {

    check_if_valid();

    if ( !std::filesystem::is_directory( ca_directory ) ) throw std::invalid_argument( ca_directory.string() + " is not a directory" );

    if ( SSL_CTX_load_verify_dir( _ctx.get(), utils::path_to_string( ca_directory ).c_str() ) != 1 ) throw SSLException();

}