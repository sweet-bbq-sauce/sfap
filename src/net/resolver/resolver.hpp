/*!
 *  \file
 *  \brief Main header for DNS resolution types and classes.
 *
 *  This header declares the Resolver and ResolverError classes used for hostname resolution
 *  with IPv4 and IPv6 support. It provides a high-level API to retrieve addresses
 *  in different formats and with configurable resolution modes.
 *
 *  \copyright Copyright (c) 2025 Wiktor Sołtys
 *
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
 */


#pragma once


#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include <net/address/detect.hpp>


namespace sfap {

    namespace net {


        /*!
         *  \class ResolverError
         *  \brief Exception class used to signal errors during name resolution.
         *
         *  This class represents an error that occurs during host name resolution.
         */
        class ResolverError : public std::runtime_error {

            public:

                /*!
                 *  \brief Constructs a ResolverError with the specified error code.
                 *  \param code Error code returned from the resolver.
                 */
                ResolverError( int code ) noexcept;

                /*!
                 *  \brief Returns the error code.
                 *  \return Integer representing the resolver error code.
                 */
                int code() const noexcept;

                /*!
                 *  \brief Returns a descriptive error message for a given code.
                 *  \param code The error code.
                 *  \return A string message describing the error.
                 */
                static std::string get_message( int code ) noexcept;

            
            private:

                int _code;          //!< Error code.
                std::string _what;  //!< Descriptive error message.

        };


        /*!
         *  \class Resolver
         *  \brief Provides DNS resolution functionality with IPv4 and IPv6 support.
         *
         *  This class allows resolution of hostnames into IP addresses (IPv4/IPv6) with configurable preference modes.
         */
        class Resolver {

            public:

                /*!
                 *  \enum Mode
                 *  \brief Specifies IP version resolution preference.
                 */
                enum class Mode {

                    IPV4,           //!< Prefer IPv4 addresses.
                    IPV6,           //!< Prefer IPv6 addresses.
                    PREFER_IPV6     //!< Prefer IPv6 but fallback to IPv4 if unavailable.

                };

                #ifndef LIBSFAP_DEFAULT_RESOLVE_MODE_ID

                    #define LIBSFAP_DEFAULT_RESOLVE_MODE_ID 0   // Default is IPv4

                #endif

                static const Mode default_mode = static_cast<Mode>( LIBSFAP_DEFAULT_RESOLVE_MODE_ID );      //!< Default resolve mode defined by LIBSFAP_DEFAULT_RESOLVE_MODE_ID macro.

                /*!
                 *  \brief Constructs a Resolver for the given hostname.
                 *  \param hostname Hostname to resolve.
                 *  \param mode Resolution mode (default is `IPV4`).
                 */
                explicit Resolver( const std::string& hostname, Mode mode = default_mode );

                /*!
                 *  \brief Sets the resolution mode.
                 *  \param mode New resolution mode.
                 */
                void set_mode( Mode mode );


                /*!
                 *  \brief Checks if IPv4 addresses were found.
                 *  \return `true if at least one IPv4 address is available.
                 */
                bool has_ipv4() const noexcept;

                /*!
                 *  \brief Checks if IPv6 addresses were found.
                 *  \return `true` if at least one IPv6 address is available.
                 */
                bool has_ipv6() const noexcept;

                /*!
                 *  \brief Returns a random IPv4 address.
                 *  \return A randomly selected IPv4 address as a string.
                 *  \throws ResolverError if no IPv4 addresses are available.
                 */
                std::string get_random_ipv4() const;

                /*!
                 *  \brief Returns a random IPv6 address.
                 *  \return A randomly selected IPv6 address as a string.
                 *  \throws ResolverError if no IPv6 addresses are available.
                 */
                std::string get_random_ipv6() const;

                /*!
                 *  \brief Returns all resolved IPv4 addresses.
                 *  \return Vector of IPv4 address strings.
                 */
                std::vector<std::string> get_all_ipv4() const noexcept;

                /*!
                 *  \brief Returns all resolved IPv6 addresses.
                 *  \return Vector of IPv6 address strings.
                 */
                std::vector<std::string> get_all_ipv6() const noexcept;

                /*!
                 *  \brief Returns all resolved addresses (IPv4 + IPv6).
                 *  \return Vector of all IP address strings.
                 */
                std::vector<std::string> get_all() const noexcept;

                /*!
                 *  \brief Returns an address based on the currently set mode.
                 *  \return Best-suited IP address as a string.
                 */
                std::string get_auto() const;


            private:

                /*!
                 *  \brief Resolves the given hostname and populates result containers.
                 *  \param hostname Hostname to resolve.
                 */
                void _resolve( const std::string& hostname );


                Mode _mode;                                     //!< Current resolve mode.
                std::map<std::string, address_type> _result;    //!< Map of resolved addresses.
                bool _has_ipv4, _has_ipv6;                      //!< Flags indicating presence of IPv4 and IPv6 addresses.

        };


    }

}