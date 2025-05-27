/*!
 *  \file
 *  \brief Header file containing Address class declaration.
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
#include <vector>

#include <crypto/context/context.hpp>
#include <net/address/host.hpp>
#include <net/proxy/proxy.hpp>


namespace sfap {

    namespace net {


        /*!
         *  \class Address
         *  \brief Represents a target endpoint with optional SSL context and proxy chain.
         *
         *  The Address class holds the connection target (host), an optional TLS context,
         *  and an optional list of proxy servers to reach the target.
         */
        class Address {

            public:

                /*!
                 *  \brief Constructs an Address with a target host and no SSL or proxies.
                 *  \param target Target host.
                 */
                explicit Address( const Host& target ) noexcept;

                /*!
                 *  \brief Constructs an Address with a target host and a list of proxies.
                 *  \param target Target host.
                 *  \param proxies List of proxies to use.
                 */
                explicit Address( const Host& target, const std::vector<Proxy>& proxies ) noexcept;

                /*!
                 *  \brief Constructs an Address with a target host and an SSL context.
                 *  \param target Target host.
                 *  \param ssl_context TLS context reference.
                 */
                explicit Address( const Host& target, const crypto::TLSContext& ssl_context ) noexcept;

                /*!
                 *  \brief Constructs an Address with a target host, SSL context, and a list of proxies.
                 *  \param target Target host.
                 *  \param ssl_context TLS context reference.
                 *  \param proxies List of proxies to use.
                 */
                explicit Address( const Host& target, const crypto::TLSContext& ssl_context, const std::vector<Proxy>& proxies ) noexcept;

                /*!
                 *  \brief Sets the target host.
                 *  \param target New target host.
                 */
                void set_target( const Host& target ) noexcept;

                /*!
                 *  \brief Sets the SSL context.
                 *  \param ssl_context TLS context to use.
                 */
                void set_ssl_context( const crypto::TLSContext& ssl_context ) noexcept;

                /*!
                 *  \brief Replaces the proxy list.
                 *  \param proxies New proxy list.
                 */
                void set_proxies( const std::vector<Proxy>& proxies ) noexcept;

                /*!
                 *  \brief Adds a single proxy to the proxy list.
                 *  \param proxy Proxy to add.
                 */
                void add_proxy( const Proxy& proxy ) noexcept;

                /*!
                 *  \brief Gets the target host.
                 *  \return Reference to the target host.
                 */
                const Host& get_target() const noexcept;

                /*!
                 *  \brief Gets the SSL context if set.
                 *  \return Optional reference to the TLS context.
                 */
                const std::optional<std::reference_wrapper<const crypto::TLSContext>> get_ssl_context() const noexcept;

                /*!
                 *  \brief Gets the proxy list.
                 *  \return Constant reference to the proxy list.
                 */
                const std::vector<Proxy>& get_proxies() const noexcept;

                /*!
                 *  \brief Checks if the address has an SSL context.
                 *  \return True if SSL context is set, false otherwise.
                 */
                bool has_ssl_context() const noexcept;

                /*!
                 *  \brief Checks if the address has any proxies configured.
                 *  \return True if at least one proxy is present, false otherwise.
                 */
                bool has_proxy() const noexcept;

            private:

                Host _target;
                std::optional<std::reference_wrapper<const crypto::TLSContext>> _ssl_context;
                std::vector<Proxy> _proxies;
        };


    }

}