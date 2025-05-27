/*!
 *  \file
 *  \brief Header file containing Credentials class declaration.
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

    namespace utils {


        /*!
         *  \class Credentials
         *  \brief Represents user credentials (username and password).
         *
         *  Provides storage and access methods for username and password.
         * 
         *  \note Credentials destructor clears sensitive data by overwriting memory with zeros.
         */
        class Credentials {

            public:

                /*!
                 *  \brief Constructs Credentials with given username and password.
                 *  \param user User name string.
                 *  \param password Password string.
                 */
                explicit Credentials( const std::string& user, const std::string& password ) noexcept;

                /*!
                 *  \brief Destructor clears sensitive data by overwriting memory with zeros.
                 */
                ~Credentials() noexcept;

                /*!
                 *  \brief Sets username and password.
                 *  \param user New user name.
                 *  \param password New password.
                 */
                void set( const std::string& user, const std::string& password ) noexcept;

                /*!
                 *  \brief Sets the username.
                 *  \param user New user name.
                 */
                void set_user( const std::string& user ) noexcept;

                /*!
                 *  \brief Sets the password.
                 *  \param password New password.
                 */
                void set_password( const std::string& password ) noexcept;

                /*!
                 *  \brief Returns the username.
                 *  \return Constant reference to the user name string.
                 */
                const std::string& get_user() const noexcept;

                /*!
                 *  \brief Returns the password.
                 *  \return Constant reference to the password string.
                 */
                const std::string& get_password() const noexcept;


            private:

                std::string _user, _password;

        };


    }

}