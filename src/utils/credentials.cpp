/*!
 *  \file
 *  \brief Source file containing Credentials class definitions.
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


#include <string>
#include <cstring>

#include <utils/credentials.hpp>


using namespace sfap::utils;


void secure_zero_string( std::string& str ) noexcept {

    if ( !str.empty() ) {

        volatile char* p = reinterpret_cast<volatile char*>( str.data() );
        std::size_t len = str.size();

        while ( len-- ) {
            *p++ = 0;
        }

    }

}


Credentials::Credentials( const std::string& user, const std::string& password ) noexcept {

    set( user, password );

}


Credentials::~Credentials() noexcept {

    secure_zero_string( _user );
    secure_zero_string( _password );

}


void Credentials::set( const std::string& user, const std::string& password ) noexcept {

    set_user( user );
    set_password( password );

}


void Credentials::set_user( const std::string& user ) noexcept {

    _user = user;

}


void Credentials::set_password( const std::string& password ) noexcept {

    _password = password;

}


const std::string& Credentials::get_user() const noexcept {

    return _user;

}


const std::string& Credentials::get_password() const noexcept {

    return _password;

}