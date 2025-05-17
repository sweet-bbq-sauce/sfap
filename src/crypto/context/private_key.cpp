/*!
 *  \file
 *  \brief TLS Private Key Loader for OpenSSL-based Context Management.
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
#include <optional>
#include <string>
#include <cstring>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <sfap.hpp>
#include <crypto/context/context.hpp>
#include <crypto/exception.hpp>
#include <utils/encoding.hpp>


using namespace sfap;
using namespace sfap::crypto;


/*!
 *  \brief Callback used by OpenSSL to retrieve the password for encrypted keys.
 *
 *  \param buffer       Buffer where the password will be stored.
 *  \param size         Size of the buffer.
 *  \param rwflag       Not used.
 *  \param user_data    Pointer to a std::string containing the password.
 *  \return Length of the password written to the buffer, or 0 if user_data is null.
 */
constexpr auto password_callback = []( char* buffer, int size, int, void* user_data ) -> int {

    if ( user_data == nullptr ) return 0;

    const auto* password_pointer = static_cast<const std::string*>( user_data );
    const int length = std::min( static_cast<int>( password_pointer->size() ), size );

    std::memcpy( buffer, password_pointer->c_str(), length );

    return length;

};


void TLSContext::_load_key_bio( std::unique_ptr<BIO, decltype( &BIO_free )> bio, std::optional<std::reference_wrapper<const std::string>> pass, const std::string& source ) {

    std::unique_lock lock( _modify_mutex );

    std::unique_ptr<EVP_PKEY, decltype( &EVP_PKEY_free )> private_key( PEM_read_bio_PrivateKey(

        bio.get(),
        nullptr,
        +password_callback,
        pass ? static_cast<void*>( const_cast<std::string*>( &pass->get() ) ) : nullptr

    ), &EVP_PKEY_free );

    if ( !private_key ) {

        const auto err = ERR_peek_last_error();
        const auto reason = ERR_GET_REASON( err );

        if (

            (
                reason == PEM_R_BAD_PASSWORD_READ ||
                reason == PEM_R_PROBLEMS_GETTING_PASSWORD ||
                reason == PEM_R_BAD_DECRYPT
            ) && !pass

        ) throw std::runtime_error( "key is encrypted and no password was provided" );

        throw OpenSSLError( "failed to parse private key from " + source + " (PEM_read_bio_PrivateKey)" );

    }

    if ( SSL_CTX_use_PrivateKey( _ctx.get(), private_key.get() ) != 1 ) {

        throw OpenSSLError( "can't set private key (SSL_CTX_use_PrivateKey)" );

    }

}


void TLSContext::load_key_string( const std::string& key, std::optional<std::reference_wrapper<const std::string>> pass ) {

    std::unique_ptr<BIO, decltype( &BIO_free )> bio( BIO_new_mem_buf( key.data(), static_cast<int>( key.size() ) ), &BIO_free );
    if ( !bio ) {

        throw OpenSSLError( "can't create BIO (BIO_new_mem_buf)" );

    }

    _load_key_bio( std::move( bio ), pass, "string" );

}


void TLSContext::load_key_file( const path_t& key, std::optional<std::reference_wrapper<const std::string>> pass ) {

    std::unique_ptr<BIO, decltype( &BIO_free )> bio( BIO_new_file( utils::path_to_string( key ).c_str(), "r" ), &BIO_free );
    if ( !bio ) {

        throw OpenSSLError( "can't create BIO from file '" + key.string() + "' (BIO_new_file)" );

    }

    _load_key_bio( std::move( bio ), pass, "'" + key.string() + "'" );

}