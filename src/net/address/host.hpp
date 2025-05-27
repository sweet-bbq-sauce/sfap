/*!
 *  \file
 *  \brief Header file containing Host class declaration.
 * 
 *  `Host` stores `hostname:port` pair.
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


#include <string>

#ifdef _WIN32

    #include <winsock2.h>
    #include <ws2tcpip.h>

#else

    #include <arpa/inet.h>
    #include <netinet/in.h>
    
#endif

#include <sfap.hpp>
#include <net/address/detect.hpp>


namespace sfap {

    namespace net {


        /*!
         *  \class Host
         *  \brief Represents a network host as a hostname and port pair.
         *
         *  The Host class provides functionality to construct, parse, and interact
         *  with network hosts using their string representation or native socket address.
         */
        class Host {

            public:

                /*!
                 *  \brief Default constructor.
                 *
                 *  Initializes an empty and invalid Host object.
                 */
                explicit Host() noexcept;

                /*!
                 *  \brief Constructs a Host with the given hostname and port.
                 *
                 *  \param hostname The hostname or IP address.
                 *  \param port The port number.
                 *
                 *  Examples:
                 *  \code ```cpp
                 *  Host( '127.0.0.1:420', 0 );     // Invalid (contains port in hostname section)
                 *  Host( '127.0.0.1', 420 );       // Valid
                 *  Host( 'localhost', 420 );       // Valid
                 *  Host( '0.0.0.0', 0 );           // Valid
                 *  Host( 'some.site.net', 420 );   // Valid
                 *  Host( '::1', 420 );             // Valid
                 *  Host( '2001:0db8:85a3:0000:0000:8a2e:0370', 420 );  // Valid
                 *  Host( '2001:0db8:85a3::8a2e:0370:7334', 420 );      // Valid
                 *  Host( '2001:db8:85a3::8a2e:370:7334', 420 );        // Valid
                 *  Host( '[2001:0db8:85a3::8a2e:0370:7334]', 420 );    // Invalid (contains branckets)
                 *  ```
                 *  \endcode
                 */
                explicit Host( const std::string& hostname, port_t port );

                /*!
                 *  \brief Constructs a Host from a single "hostname:port" string.
                 *
                 *  \param host A string in the format "hostname:port".
                 *
                 *  Examples:
                 *  \code ```cpp
                 *  Host( '127.0.0.1:420' );        // Valid
                 *  Host( '127.0.0.1' );            // Invalid (missing port)
                 *  Host( 'localhost:420' );        // Valid
                 *  Host( 'some.site.net:420' );    // Valid
                 *  Host( 'some.site.net' );        // Invalid (missing port)
                 *  Host( '[::1]:420' );            // Valid
                 *  Host( '[2001:0db8:85a3:0000:0000:8a2e:0370:7334]:420' );    // Valid
                 *  Host( '[2001:0db8:85a3::8a2e:0370:7334]:420' );             // Valid
                 *  Host( '[2001:db8:85a3::8a2e:370:7334]:420' );               // Valid
                 *  Host( '2001:0db8:85a3::8a2e:0370:7334:420' );               // Invalid (missing branckets)
                 *  Host( '2001:0db8:85a3::8a2e:0370:7334' );                   // Invalid (missing branckets and port)
                 *  ```
                 *  \endcode
                 */
                explicit Host( const std::string& host );

                /**
                 *  \brief Constructs a Host from a native socket address.
                 *
                 *  \param native A sockaddr_storage structure representing a network address.
                 */
                explicit Host( const sockaddr_storage& native );

                /*!
                 *  \brief Equality operator comparing two Host objects.
                 *
                 *  \param other The other Host to compare with.
                 *  \return True if both hosts have the same hostname and port.
                 */
                bool operator==( const Host& other ) const noexcept;

                /*!
                 *  \brief Equality operator comparing the hostname to a string.
                 *
                 *  \param hostname The hostname to compare with.
                 *  \return True if the stored hostname matches the given string.
                 */
                bool operator==( const std::string& hostname ) const noexcept;

                /*!
                 *  \brief Sets the hostname.
                 *
                 *  \param hostname The new hostname.
                 */
                void set_hostname( const std::string& hostname );

                /*!
                 *  \brief Returns the current hostname.
                 *
                 *  \return The stored hostname string.
                 */
                const std::string& get_hostname() const noexcept;

                /*!
                 *  \brief Sets the port number.
                 *
                 *  \param port The new port number.
                 */
                void set_port( port_t port ) noexcept;

                /*!
                 *  \brief Returns the current port number.
                 *
                 *  \return The stored port number.
                 */
                port_t get_port() const noexcept;

                /*!
                 *  \brief Sets both the hostname and port from a single string.
                 *
                 *  \param host A string in the format `"hostname:port"`.
                 */
                void set_host( const std::string& host );

                /*!
                 *  \brief Initializes the Host from a native socket address.
                 *
                 *  \param native The native `sockaddr_storage` structure.
                 */
                void from_native( const sockaddr_storage& native );

                /*!
                 *  \brief Returns the type of IP address (IPv4 or IPv6).
                 *
                 *  \return The address_type enumeration value.
                 */
                address_type get_address_type() const noexcept;

                /*!
                 *  \brief Returns a string representation in the format "hostname:port".
                 *
                 *  \return The host as a string.
                 */
                std::string to_string() const noexcept;

                /*!
                 *  \brief Converts the host to a native socket address.
                 *
                 *  \return A sockaddr_storage representation of the host.
                 */
                sockaddr_storage to_native() const;

                /*!
                 *  \brief Checks if the Host is valid (has both hostname and port set correctly).
                 *
                 *  \return True if valid; false otherwise.
                 */
                bool is_valid() const noexcept;

                /*!
                 *  \brief Implicit conversion to bool.
                 *
                 *  \return True if the Host is valid.
                 */
                operator bool() const noexcept;

                /*!
                 *  \brief Checks if the host can be resolved and connected to.
                 *
                 *  \return True if the host is connectable.
                 */
                bool is_connectable() const noexcept;

                /*!
                 *  \brief Returns Host hostname type.
                 *
                 *  \return Address type.
                 */
                address_type get_type() const noexcept;


            private:

                std::string _hostname;      //!< Hostname or IP address.
                port_t _port;               //!< Port number.
                address_type _type;         //!< Type of the address.

        };


    }

}