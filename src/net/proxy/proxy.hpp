/*!
 *  \file
 *  \brief Header file containing Proxy class declaration.
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


#include <optional>
#include <string>

#include <net/address/host.hpp>
#include <net/iosocket/iosocket.hpp>
#include <utils/credentials.hpp>


namespace sfap {

    namespace net {


        /*!
         *  \enum proxy_type
         *  \brief Enumerates supported proxy types.
         */
        enum class proxy_type {

            HTTP_CONNECT,   ///< HTTP CONNECT proxy type
            SOCKS5          ///< SOCKS5 proxy type

        };


        /*!
         *  \class Proxy
         *  \brief Represents a network proxy connection handler.
         *
         *  Supports opening connections through HTTP CONNECT or SOCKS5 proxies,
         *  with optional credentials for authentication.
         */
        class Proxy {

            public:

                /*!
                 *  \brief Constructs a Proxy object.
                 *  \param type Type of proxy (HTTP_CONNECT or SOCKS5).
                 *  \param host Proxy server address and port.
                 *  \param credentials Optional credentials for proxy authentication.
                 */
                Proxy(
                    
                    proxy_type type,
                    const Host& host,
                    const std::optional<std::reference_wrapper<const utils::Credentials>>& credentials = std::nullopt
                
                ) noexcept;

                /*!
                 *  \brief Opens a proxied connection to the target host using the given socket.
                 *  \param target Target server host and port to connect to through the proxy.
                 *  \param sock IOSocket representing the underlying socket connected to the proxy.
                 *  \throws on failure to open or authenticate the proxy connection.
                 */
                void open(
                    
                    const Host& target,
                    const IOSocket& sock
                
                ) const;

                const Host& get_host() const noexcept;


            private:

                /*!
                 *  \brief Opens an HTTP CONNECT proxy connection.
                 *  \param target Target server host and port.
                 *  \param sock Socket connected to the proxy server.
                 */
                void _open_http_connect( const Host& target, const IOSocket& sock ) const;

                /*!
                 *  \brief Opens a SOCKS5 proxy connection.
                 *  \param target Target server host and port.
                 *  \param sock Socket connected to the proxy server.
                 */
                void _open_socks5( const Host& target, const IOSocket& sock ) const;

                /*!
                 *  \brief Performs SOCKS5 authentication using the provided socket.
                 *  \param sock Socket connected to the proxy server.
                 */
                void _authenticate_socks5( const IOSocket& sock ) const;

                /*!
                 *  \brief Connects to the target through SOCKS5 proxy after authentication.
                 *  \param target Target server host and port.
                 *  \param sock Socket connected to the proxy server.
                 */
                void _connect_socks5( const Host& target, const IOSocket& sock ) const;

                proxy_type _type;       ///< Proxy type
                Host _host;             ///< Proxy server host
                std::optional<std::reference_wrapper<const utils::Credentials>> _credentials;   ///< Optional credentials

        };


    }

}