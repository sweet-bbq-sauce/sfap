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
                static data_t hash_file( HashAlgorithm algorithm, const path_t& path, std::size_t buffer_size = 4096 );

                /*!
                 *  \brief Compute a cryptographic hash over a window of an input stream.
                 *
                 *  \details
                 *  Hashes the byte sequence in \p input starting at the current get position and spanning
                 *  either \p size bytes (if provided) or up to end-of-stream. The function determines the
                 *  available window using \c tellg() / \c seekg(), then reads in chunks of \p buffer_size
                 *  without allocating more than that temporary buffer.
                 *
                 *  The stream must be seekable and in a good state on entry. The function clears the stream’s
                 *  error flags and seeks back to the starting position before reading. It does **not** restore
                 *  the original position afterwards; on return, the get position is advanced by the number of
                 *  bytes hashed.
                 *
                 *  \param algorithm    Hash algorithm to use (e.g. \c HashAlgorithm::SHA256).
                 *  \param input        Input stream to read from. Open it in binary mode if you need byte-exact hashing.
                 *  \param size         Optional number of bytes to hash from the current position. If \c std::nullopt,
                 *                      the function hashes until EOF from the current position.
                 *  \param buffer_size  Size (in bytes) of the internal read buffer. Must be > 0.
                 *
                 *  \return The computed hash as a byte vector (\c data_t).
                 *
                 *  \throws std::runtime_error
                 *          If the stream is invalid or not seekable; if the computed range is negative; if
                 *          \p size exceeds the available bytes to EOF starting at the current position; or
                 *          on short read errors while consuming the stream.
                 *
                 *  \pre \p input is a seekable stream; \p buffer_size > 0.
                 *  \post The stream’s get position is advanced by \p size bytes (or to EOF if \p size is \c std::nullopt).
                 *
                 *  \note This function does not close the stream and does not modify its formatting flags.
                 */
                static data_t hash_stream( HashAlgorithm algorithm, std::istream& input, std::optional<qword_t> size = std::nullopt, std::size_t buffer_size = 4096 );


            private:

                void _init( HashAlgorithm algorithm );

                std::unique_ptr<EVP_MD_CTX, decltype( &EVP_MD_CTX_free )> _ctx;
                HashAlgorithm _algorithm;
                std::optional<data_t> _digest;

        };


    }

}