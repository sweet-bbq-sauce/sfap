/*!
 *  \file
 *  \brief Implementation of sfap::crypto::Hash class.
 *
 *  Provides method definitions for incremental hashing, digest finalization and context reset.
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
#include <stdexcept>

#include <openssl/evp.h>

#include <sfap.hpp>
#include <crypto/exception.hpp>
#include <crypto/hash/hash.hpp>


using namespace sfap;
using namespace sfap::crypto;


Hash::Hash( HashAlgorithm algorithm ) :
    _ctx( EVP_MD_CTX_new(), &EVP_MD_CTX_free ),
    _algorithm( algorithm )
{

    if ( _ctx == nullptr ) {

        throw crypto::OpenSSLError( "digest context init error" );

    }

    _init( algorithm );

}


void Hash::update( const void* data, std::size_t size ) {

    if ( _digest ) {

        throw std::logic_error( "digest is already finished" );

    }

    if ( EVP_DigestUpdate( _ctx.get(), data, size ) != 1 ) {

        throw crypto::OpenSSLError( "digest update error" );

    }

}


data_t Hash::finish() {

    if ( _digest ) {

        return *_digest;

    }
    else {

        auto length = static_cast<unsigned int>( EVP_MD_size( EVP_MD_CTX_get0_md( _ctx.get() ) ) );
        data_t digest( length );

        if ( EVP_DigestFinal_ex( _ctx.get(), digest.data(), &length ) != 1 ) {

            throw crypto::OpenSSLError( "digest final error" );

        }

        _digest = digest;
        return digest;

    }

}


void Hash::reset() {

    _init( _algorithm );
                    
}


void Hash::reset( HashAlgorithm algorithm ) {

    _algorithm = algorithm;

    _init( algorithm );
                    
}


void Hash::_init( HashAlgorithm algorithm ) {

    if ( _digest ) {

        _digest.reset();

    }

    const EVP_MD* evp;

    switch ( algorithm ) {

        case HashAlgorithm::MD5:        evp = EVP_md5();        break;
        case HashAlgorithm::SHA1:       evp = EVP_sha1();       break;
        case HashAlgorithm::SHA256:     evp = EVP_sha256();     break;
        case HashAlgorithm::SHA512:     evp = EVP_sha512();     break;

        default:
            throw std::invalid_argument( "unknown hash algorithm" );

    }

    if ( EVP_DigestInit_ex( _ctx.get(), evp, nullptr ) != 1 ) {

        throw crypto::OpenSSLError( "digest init error" );

    }

}