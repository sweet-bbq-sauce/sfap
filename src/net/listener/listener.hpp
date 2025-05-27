/*!
 *  \file
 *  \brief Header file containing Listener class declaration.
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

#include <crypto/context/context.hpp>
#include <net/address/address.hpp>
#include <net/iosocket/iosocket.hpp>


namespace sfap {

    namespace net {


        /*!
         *  \class sfap::net::Listener
         *  \brief TCP server-side socket capable of accepting incoming connections (optionally with TLS support).
         *
         *  The `Listener` class provides a simple interface for creating a TCP (or TLS-secured) server socket
         *  that listens for incoming connections and returns `IOSocket` objects for client communication.
         *
         *  It uses an `Address` to configure the local host and (optionally) the TLS context.
         *
         *  \note   To bind to a random available port, use an address with port `0`. 
         *          Then use `get_host()` to retrieve the actual bound port.
         */
        class Listener {

            public:

                /*!
                 *  \brief Constructs a listener bound to the specified address.
                 *
                 *  If the address contains a TLS context, the listener will accept secure (TLS) connections.
                 *
                 *  \param address The local address and optional TLS context to bind to.
                 */
                explicit Listener( const Address& address );

                /*!
                 *  \brief Destructor. Closes the listener socket if it is still open.
                 */
                ~Listener() noexcept;

                /*!
                 *  \brief Accepts an incoming connection.
                 *
                 *  Blocks until a connection is available. Returns an `IOSocket` representing the accepted connection.
                 *
                 *  \param peer_address If provided, this will be set to the remote peer's host address.
                 *  \return An IOSocket for communication with the connected peer.
                 *
                 *  \throws std::runtime_error on failure to accept or if the listener is closed.
                 */
                IOSocket accept( std::optional<std::reference_wrapper<Host>> peer_address = std::nullopt ) const;

                /*!
                 *  \brief Closes the listener socket.
                 *
                 *  After calling this, the listener becomes inactive and cannot accept new connections.
                 */
                void close() noexcept;

                /*!
                 *  \brief Checks whether the listener socket is currently open.
                 *
                 *  \return true if the socket is open; false otherwise.
                 */
                bool is_open() const noexcept;

                /*!
                 *  \brief Boolean conversion operator.
                 *
                 *  Equivalent to calling `is_open()`.
                 *
                 *  \return true if the socket is open; false otherwise.
                 */
                explicit operator bool() const noexcept;

                /*!
                 *  \brief Checks whether the listener is configured for secure (TLS) communication.
                 *
                 *  \return true if a TLS context is associated with the listener; false otherwise.
                 */
                bool is_secure() const noexcept;

                /*!
                 *  \brief Returns the host address the listener is bound to.
                 *
                 *  Useful especially when binding to port 0 (random port), to discover the actual assigned port.
                 *
                 *  \return Host representing the actual bound address.
                 */
                Host get_host() const;

            
            private:

                socket_t _fd;
                std::optional<std::reference_wrapper<const crypto::TLSContext>> _ssl_context;

        };


    }

}