/*!
 *  \file
 *  \brief Header file containing IOSocket class declaration.
 * 
 *  `IOSocket` is wrapper around TCP socket.
 *  Supports SSL and makes easy data serialization.
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


#include <atomic>
#include <mutex>
#include <optional>
#include <string>

#include <sfap.hpp>
#include <crypto/context/context.hpp>
#include <net/address/host.hpp>
#include <utils/endianess.hpp>


namespace sfap {

    namespace net {

        /*!
        *   \class IOSocket
        *   \brief Wrapper around TCP socket providing convenient send/receive methods.
        * 
        *   This class encapsulates a socket descriptor and optional SSL context to support
        *   both plain and secure TCP communication. It provides methods for sending and
        *   receiving raw bytes, integral types with automatic endian conversion, strings,
        *   and data buffers.
        */
        class IOSocket {

            public:

                /*!
                *   \brief Constructs IOSocket from existing socket descriptor and optional SSL context.
                *   \param fd Socket descriptor.
                *   \param ssl Optional SSL pointer (defaults to nullptr).
                */
                IOSocket(

                    socket_t fd = INVALID_SOCKET,
                    crypto::ssl_ptr ssl = { nullptr, &SSL_free }
                    
                );

                ~IOSocket();

                IOSocket( IOSocket&& other ) noexcept;
                IOSocket& operator=( IOSocket&& other ) noexcept;

                IOSocket( const IOSocket& other ) = delete;
                IOSocket& operator=( const IOSocket& other ) = delete;

                /*!
                *   \brief Closes the socket and releases associated resources.
                */
                void close();

                /*!
                 *  \brief Sets or replaces the SSL context used by this socket.
                 *  \param ssl SSL pointer.
                 */
                void set_ssl( crypto::ssl_ptr ssl);

                /*!
                 *  \brief Checks if the socket is currently open.
                 *  \return True if socket is open, false otherwise.
                 */
                bool is_open() const noexcept;

                /*!
                 *  \brief Conversion to bool to check if socket is valid and open.
                 */
                explicit operator bool() const noexcept;

                /*!
                 *  \brief Checks if this socket uses SSL encryption.
                 *  \return True if SSL is enabled, false otherwise.
                 */
                bool is_secure() const noexcept;

                /*!
                 *  \brief Gets the local address of the socket.
                 *  \return Host object representing the local address.
                 */
                Host get_local_address() const;

                /*!
                 *  \brief Gets the remote address of the socket.
                 *  \return Host object representing the remote address.
                 */
                Host get_remote_address() const;

                /*!
                 *  \brief Returns the total number of bytes sent through this socket.
                 *  \return Number of bytes sent.
                 */
                qword_t get_sent_bytes_count() const noexcept;

                /*!
                 *  \brief Returns the total number of bytes received through this socket.
                 *  \return Number of bytes received.
                 */
                qword_t get_received_bytes_count() const noexcept;

                /*!
                 *  \brief Returns the underlying socket descriptor.
                 *  \return Socket descriptor.
                 */
                socket_t get_socket() const noexcept;

                /*!
                 *  \brief Sends raw data over the socket.
                 *  \param data Pointer to the data buffer.
                 *  \param size Number of bytes to send.
                 */
                void send( const void* data, dword_t size ) const;  // General send

                /*!
                 *  \brief Sends an integral value in network byte order.
                 * 
                 *  This method converts the integral value to big-endian format before sending.
                 *  \tparam T Integral type (must be 1, 2, 4, or 8 bytes).
                 *  \param object Integral value to send.
                 */
                template<typename T>
                void sendo( const T& object ) const {

                    static_assert( std::is_integral_v<T>, "T must be an integral type (IOSocket::sendo)" );
                    static_assert( sizeof( T ) == 1 || sizeof( T ) == 2 || sizeof( T ) == 4 || sizeof( T ) == 8, "unsupported integral size (IOSocket::sendo)" );

                    const T big_endian = utils::hton( object );

                    send( &big_endian, sizeof( big_endian ) );

                }

                template<typename E>
                void sende( E e ) const {

                    static_assert( std::is_enum_v<E>, "sende: E must be an enum type" );

                    sendo( static_cast<std::underlying_type_t<E>>( e ) );

                }

                /*!
                 *  \brief Sends a boolean value over the socket.
                 *  \param value Boolean value to send.
                 * 
                 *  \note Do not use sendo() for bool, as bool size and representation may vary between systems.
                 */
                void sendb( bool value ) const;



                /*!
                 *  \brief Sends a byte value over the socket.
                 *
                 *  It's wrapper around `sendo<byte_t>()`.
                 *  \param value Byte to send.
                 */
                void sendc( byte_t value ) const;

                /*!
                 *  \brief Sends data prefixed with its size (4-byte length prefix).
                 *  \param data Pointer to data to send.
                 *  \param size Size of data in bytes.
                 */
                void sendh( const void* data, dword_t size ) const;

                /*!
                 *  \brief Sends a data_t object.
                 *  \param data Data object to send.
                 *  \see data_t
                 */
                void sendd( const data_t& data ) const;

                /*!
                 *  \brief Sends a string over the socket.
                 *  \param data String to send.
                 */
                void sends( const std::string& data ) const;

                /*!
                 *  \brief Receives raw data from the socket.
                 *  \param data Pointer to buffer where received data will be stored.
                 *  \param size Number of bytes to receive.
                 */
                void recv( void* data, dword_t size ) const;

                /*!
                 *  \brief Receives an integral value from the socket.
                 * 
                 *  This method receives bytes and converts from network byte order to host order.
                 *  \tparam T Integral type (must be 1, 2, 4, or 8 bytes).
                 *  \return Received integral value.
                 */
                template<typename T>
                T recvo() const {

                    static_assert( std::is_integral_v<T>, "T must be an integral type (IOSocket::recvo)" );
                    static_assert( sizeof( T ) == 1 || sizeof( T ) == 2 || sizeof( T ) == 4 || sizeof( T ) == 8, "unsupported integral size (IOSocket::recvo)" );

                    T object;
                    recv( &object, sizeof( object ) );

                    return utils::ntoh( object );

                }

                template<typename E>
                E recve() const {

                    static_assert( std::is_enum_v<E>, "sende: E must be an enum type" );

                    return static_cast<E>( recvo<std::underlying_type_t<E>>() );

                }

                /*!
                 *  \brief Receives a boolean value from the socket.
                 *  \return Received boolean value.
                 */
                bool recvb() const;

                /*!
                 *  \brief Receives a byte value from the socket.
                 *
                 *  It's wrapper around `recvo<byte_t>()`.
                 *  \return Received byte.
                 */
                byte_t recvc() const;

                /*!
                 *  \brief Receives data into a data_t object.
                 *  \param data Reference to data_t where received data will be stored.
                 */
                void recvd( data_t& data ) const;

                /*!
                 *  \brief Receives data into a string.
                 *  \param data Reference to string where received data will be stored.
                 */
                void recvs( std::string& data ) const;

                /*!
                 *  \brief Receives a string and returns it.
                 *  \return Received string.
                 */
                std::string recvss() const;


            private:

                socket_t _fd;       ///< Underlying socket descriptor
                std::optional<crypto::ssl_ptr> _ssl;    ///< Optional SSL context

                mutable std::atomic<qword_t> _received_bytes, _sent_bytes;      ///< Counters for bytes sent/received
                mutable std::mutex _send_mutex, _recv_mutex;    ///< Mutexes for thread-safe send/recv

        };


    }

}