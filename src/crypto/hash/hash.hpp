/*!
 *  \file
 *  \brief Cryptographic hashing utility class and helper functions.
 *
 *  This header defines the \ref sfap::crypto::Hash class used for computing message digests
 *  using various algorithms (MD5, SHA-1, SHA-256, SHA-512). It provides a simple RAII wrapper
 *  around OpenSSL EVP interface and static helpers to compute hashes of raw data or files.
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


#pragma once


#include <fstream>
#include <memory>
#include <optional>
#include <stdexcept>

#include <openssl/evp.h>

#include <sfap.hpp>
#include <crypto/exception.hpp>


namespace sfap {

    namespace crypto {


        /*!
         *  \enum HashAlgorithm
         *  \brief Supported hashing algorithms.
         */
        enum class HashAlgorithm : byte_t {

            MD5 = 0x01,     ///< MD5 algorithm (128-bit).
            SHA1 = 0x02,    ///< SHA-1 algorithm (160-bit).
            SHA256 = 0x03,  ///< SHA-256 algorithm (256-bit).
            SHA512 = 0x04   ///< SHA-512 algorithm (512-bit).

        };


        /*!
         *  \class Hash
         *  \brief RAII wrapper for OpenSSL message digest computations.
         *
         *  Provides a simple interface for incremental hashing of data streams,
         *  finalizing the digest and resetting or changing algorithm.
         */
        class Hash {

            public:

                /*!
                 *  \brief Constructs a new Hash instance.
                 *  \param algorithm Algorithm to be used for hashing.
                 */
                explicit Hash( HashAlgorithm algorithm );

                /*!
                 *  \brief Updates the digest with a chunk of data.
                 *  \param data Pointer to data buffer.
                 *  \param size Size of the buffer in bytes.
                 *  \throws std::logic_error if the digest is already finalized.
                 */
                void update( const void* data, std::size_t size );

                /*!
                 *  \brief Finalizes the digest and returns the hash value.
                 *  \return Computed message digest as a byte vector.
                 *  \throws crypto::OpenSSLError on failure.
                 */
                data_t finish();

                /*!
                 *  \brief Resets the context with the same algorithm.
                 */
                void reset();

                /*!
                 *  \brief Resets the context with a new algorithm.
                 *  \param algorithm New algorithm to use.
                 */
                void reset( HashAlgorithm algorithm );

                /*!
                 *  \brief Computes a digest of a raw memory buffer in one call.
                 *  \param algorithm Algorithm to use.
                 *  \param data Pointer to the data buffer.
                 *  \param size Size of the buffer.
                 *  \return Computed digest.
                 */
                static data_t hash_data( HashAlgorithm algorithm, const void* data, std::size_t size );

                /*!
                 *  \brief Computes a digest of a data container.
                 *  \param algorithm Algorithm to use.
                 *  \param data Container holding the data.
                 *  \return Computed digest.
                 */
                static data_t hash_data( HashAlgorithm algorithm, const data_t& data );

                /*!
                 *  \brief Computes a digest of a std::string.
                 *  \param algorithm Algorithm to use.
                 *  \param data Input string.
                 *  \return Computed digest.
                 */
                static data_t hash_data( HashAlgorithm algorithm, const std::string& data );

                /*!
                 *  \brief Computes a digest of a file's contents.
                 *  \param algorithm Algorithm to use.
                 *  \param path Path to the file.
                 *  \return Computed digest.
                 *  \throws std::runtime_error if the file cannot be opened.
                 */
                static data_t hash_file( HashAlgorithm algorithm, const path_t& path );


            private:

                void _init( HashAlgorithm algorithm );

                std::unique_ptr<EVP_MD_CTX, decltype( &EVP_MD_CTX_free )> _ctx;
                HashAlgorithm _algorithm;
                std::optional<data_t> _digest;

        };


    }

}