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


void SSLContext::load_private_key( const std::string& private_key ) {

    check_if_valid();

    std::unique_ptr<BIO, decltype( &BIO_free )> bio( BIO_new_mem_buf( private_key.data(), private_key.size() ), BIO_free );
    if ( !bio ) throw SSLException();

    std::unique_ptr<EVP_PKEY, decltype( &EVP_PKEY_free )> pkey( PEM_read_bio_PrivateKey( bio.get(), nullptr, nullptr, nullptr ), EVP_PKEY_free );
    if ( !pkey ) throw SSLException();

    if ( SSL_CTX_use_PrivateKey( _ctx.get(), pkey.get() ) != 1 ) throw SSLException();
}


void SSLContext::load_private_key_file( path_t private_key ) {

    check_if_valid();

    if ( SSL_CTX_use_PrivateKey_file( _ctx.get(), utils::path_to_string( private_key ).c_str(), SSL_FILETYPE_PEM ) != 1 ) throw SSLException();

}