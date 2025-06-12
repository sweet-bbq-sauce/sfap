/*!
 *  \file
 *  \brief TLSContext class for managing OpenSSL TLS/SSL configurations.
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


#include <functional>
#include <map>
#include <memory>
#include <shared_mutex>
#include <optional>
#include <string>
#include <vector>

#include <openssl/ssl.h>

#include <sfap.hpp>


namespace sfap {

    namespace crypto {


        /*!
         *  \brief Smart pointer type for managing OpenSSL SSL objects.
         */
        using ssl_ptr = std::unique_ptr<SSL, decltype( &SSL_free )>;


        /*!
         *  \class TLSContext
         *  \brief Encapsulates and manages an OpenSSL SSL_CTX context.
         *
         *  Provides methods to configure SSL/TLS parameters, load certificates,
         *  keys, and trusted CA certificates, and create SSL objects for connections.
         */
        class TLSContext {

            public:

                /*!
                 *  \brief Constructs a TLSContext with a given verification mode.
                 *  \param verify_mode OpenSSL verification mode (e.g., `SSL_VERIFY_NONE`).
                 */
                explicit TLSContext( int verify_mode = SSL_VERIFY_NONE, bool system_ca = true );
                ~TLSContext();

                TLSContext( const TLSContext& ) = delete;
                TLSContext& operator=( const TLSContext& ) = delete;
                TLSContext( TLSContext&& other ) noexcept;
                TLSContext& operator=( TLSContext&& other ) noexcept;

                /*!
                 *  \brief Sets the peer certificate verification mode and optional callback.
                 *  \param mode Verification mode (e.g., `SSL_VERIFY_PEER`).
                 *  \param callback Optional verification callback.
                 */
                void set_verify_mode(

                    int mode,
                    std::optional<std::function<int( int, X509_STORE_CTX* )>> callback = std::nullopt

                ) noexcept;

                /*!
                 *  \brief Sets SSL/TLS context options (e.g., `SSL_OP_NO_SSLv2`).
                 *  \param options Bitmask of options.
                 */
                void set_options( long options );

                /*!
                 * @brief Sets the cipher list for TLSv1.2 and earlier.
                 * @param ciphers OpenSSL cipher list string.
                 */
                void set_cipher_list( const std::string& ciphers );

                /*!
                 *  \brief Sets the cipher suites for TLSv1.3.
                 *  \param ciphersuites OpenSSL cipher suite string.
                 */
                void set_cipher_suites( const std::string& ciphersuites );

                /*!
                 *  \brief Sets the minimum supported TLS protocol version.
                 *  \param version OpenSSL TLS version constant (e.g., `TLS1_2_VERSION`).
                 */
                void set_min_proto_version( int version );

                /*!
                 *  \brief Sets the maximum supported TLS protocol version.
                 *  \param version OpenSSL TLS version constant (e.g., `TLS1_1_VERSION`).
                 */
                void set_max_proto_version( int version );

                /*!
                 *  \brief Loads a PEM-encoded certificate from a string.
                 *  \param certificate The certificate content.
                 */
                void load_cert_string( const std::string& certificate );

                /*!
                 *  \brief Loads a PEM-encoded certificate from a file.
                 *  \param certificate Path to the certificate file.
                 */
                void load_cert_file( const path_t& certificate );

                /*!
                 *  \brief Loads a PEM-encoded private key from a string.
                 *  \param key The private key content.
                 *  \param pass Optional password for encrypted keys.
                 */
                void load_key_string(
                    
                    const std::string& key,
                    std::optional<std::reference_wrapper<const std::string>> pass = std::nullopt
                
                );

                /*!
                 *  \brief Loads a PEM-encoded private key from a file.
                 *  \param key Path to the private key file.
                 *  \param pass Optional password for encrypted keys.
                 */
                void load_key_file(
                    
                    const path_t& key,
                    std::optional<std::reference_wrapper<const std::string>> pass = std::nullopt
                
                );

                /*!
                 *  \brief Adds a trusted Certificate Authority (CA) from a string.
                 *  \param ca The CA certificate content.
                 */
                void add_ca_string( const std::string& ca );

                /*!
                 *  \brief Adds a trusted Certificate Authority (CA) from a file.
                 *  \param ca Path to the CA certificate file.
                 */
                void add_ca_file( const path_t& ca );

                /*!
                 *  \brief Adds trusted CAs from a directory.
                 *  \param ca_directory Path to the CA directory.
                 */
                void add_ca_path( const path_t& ca_directory );

                /*!
                 *  \brief Adds multiple trusted CAs from strings.
                 *  \param ca Vector of PEM-encoded CA certificates.
                 */
                void add_ca_string_array( const std::vector<std::string>& ca );

                /*!
                 *  \brief Adds multiple trusted CAs from files.
                 *  \param ca Vector of paths to CA certificate files.
                 */
                void add_ca_file_array( const std::vector<path_t>& ca );

                void add_system_ca();

                /*!
                 *  \brief Gets the underlying SSL_CTX pointer.
                 *  \return Raw SSL_CTX pointer.
                 */
                SSL_CTX* get() const noexcept;

                /*!
                 *  \brief Creates a new SSL object using this context.
                 *  \return Unique pointer to the SSL object.
                 */
                ssl_ptr create_ssl() const;

                /*!
                 *  \brief Operator overload to create a new SSL object.
                 *  \return Unique pointer to the SSL object.
                 */
                ssl_ptr operator()() const;


                static std::shared_ptr<const TLSContext> default_client_context;


            private:

                /*!
                 *  \brief Loads a private key from a BIO stream.
                 *
                 *  \param bio BIO containing the key data.
                 *  \param pass Optional password for decryption.
                 *  \param source Description of the key source (e.g., file name or `"string"`).
                 */
                void _load_key_bio(
                    
                    std::unique_ptr<BIO, decltype( &BIO_free )> bio,
                    std::optional<std::reference_wrapper<const std::string>> pass,
                    const std::string& source
                
                );

                std::unique_ptr<SSL_CTX, decltype( &SSL_CTX_free )> _ctx;
                mutable std::shared_mutex _modify_mutex;

                static std::map<const SSL_CTX*, std::function<int( int, X509_STORE_CTX* )>> _callbacks;
                static std::shared_mutex _callbacks_mutex;

                /*!
                 *  \brief Static OpenSSL verify callback wrapper.
                 *  \param preverify_ok Pre-verification result.
                 *  \param store_ctx X509 store context.
                 *  \return Verification result (1 for success, 0 for failure).
                 */
                static int _verify_callback( int preverify_ok, X509_STORE_CTX* store_ctx );

        };


    }

}