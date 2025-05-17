/*!
 *  \file
 *  \brief OpenSSL errors handling.
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


#include <memory>
#include <stdexcept>
#include <string>

#include <openssl/bio.h>
#include <openssl/err.h>


namespace sfap {

    namespace crypto {


        /*!
         *  \class OpenSSLError
         *  \brief Exception type for OpenSSL error reporting.
         *
         *  This class derives from std::runtime_error and is intended to wrap
         *  human-readable OpenSSL error messages for easier C++ exception handling.
         */
        class OpenSSLError : public std::runtime_error {

            public:

                /*!
                 *  \brief Constructs an OpenSSLError exception with an optional prefix.
                 * 
                 *  The prefix is prepended to the OpenSSL error string for additional context.
                 *
                 *  \param prefix Optional string to provide additional context for the error.
                 */
                explicit OpenSSLError( const std::string& prefix = "" );

                /*!
                 *  \brief Retrieves a string containing all current OpenSSL error messages.
                 * 
                 *  This static utility method consumes the OpenSSL error queue
                 *  and formats its contents into a readable std::string.
                 *
                 *  \return A string with concatenated OpenSSL error descriptions.
                 */
                static std::string get_openssl_errors();

        };


    }

}