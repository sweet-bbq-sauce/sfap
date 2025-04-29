/*!
 *  \file
 *  \brief Header file containing network address type detection.
 *
 *  This file defines global enum `address_type` and declares function `detect_address_function()`.
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


namespace sfap {

    namespace net {


        /*!
         *  \enum address_type
         *  \brief Enumerates possible types of network addresses.
         * 
         *  \see \ref detect_address_type()
         */
        enum class address_type {

            UNSUPPORTED,    ///< Unsupported address.
            EMPTY,          ///< Empty address.
            IPV4,           ///< IPv4 address.
            IPV6,           ///< IPv6 address.
            HOSTNAME        ///< Valid RFC 1034 hostname.

        };

        /*!
         *  \brief Detect address type.
         *
         *  Returns `address_type::IPV4` if address is valid IPv4.
         *  Returns `address_type::IPV6` if address is valid IPv6.
         *  Returns `address_type::HOSTNAME` if address is valid RFC 1034 hostname.
         *  Returns `address_type::EMPTY` if address is empty.
         *  Returns `address_type::UNSUPPORTED` if address is unknown, or address size is greater than 254 characters.
         * 
         *  It checks hostname in this order:
         *  1. is hostname empty?
         *  2. does it have >254 length?
         *  3. is it valid IPv4?
         *  4. is it valid IPv6?
         *  5. is it valid RFC 1034 hostname?
         *  6. it's unsupported
         * 
         *  \returns \ref address_type
         */
        address_type detect_address_type( const std::string& address ) noexcept;

    }
    

}