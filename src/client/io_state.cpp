/*!
 *  \file
 *  \brief Defines the IOState class representing basic I/O stream state flags.
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


#include <array>

#include <sfap.hpp>
#include <client/io_state.hpp>


using namespace sfap;
using namespace sfap::utils;


IOState::IOState() noexcept :
    _fail_flag( false ),
    _bad_flag( false ),
    _eof_flag( false ) {}


IOState::IOState( byte_t serialized ) noexcept {

    deserialize( serialized );

}


IOState::IOState( bool _fail, bool _bad, bool _eof ) noexcept :
    _fail_flag( _fail ),
    _bad_flag( _bad ),
    _eof_flag( _eof )
{}


bool IOState::fail() const noexcept {

    return _fail_flag;

}


bool IOState::bad() const noexcept {

    return _bad_flag;

}


bool IOState::eof() const noexcept {

    return _eof_flag;

}


byte_t IOState::serialize() const noexcept {

    return static_cast<byte_t>(

        ( _fail_flag << 2 ) |
        ( _bad_flag  << 1 ) |
        ( _eof_flag )

    );

}


void IOState::deserialize( byte_t raw ) noexcept {

    _fail_flag = ( raw >> 2 ) & 0x01;
    _bad_flag  = ( raw >> 1 ) & 0x01;
    _eof_flag  = raw & 0x01;

}