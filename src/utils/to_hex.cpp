/*!
 *  \file
 *  \brief Utility functions definitions for converting binary data to hexadecimal string representation.
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


#include <atomic>
#include <iomanip>
#include <sstream>
#include <string>

#include <sfap.hpp>
#include <utils/to_hex.hpp>


using namespace sfap;
using namespace sfap::utils;


std::atomic<HexFormatMode> hex_format_mode{ HexFormatMode::LOWERCASE };


void utils::set_hex_format_mode( HexFormatMode mode ) {

    hex_format_mode.store( mode );

}


std::string utils::to_hex( const void* data, std::size_t size ) {

    std::ostringstream oss;
    oss << std::hex << std::setfill( '0' );

    switch ( hex_format_mode.load() ) {

        case HexFormatMode::LOWERCASE:
            break;

        case HexFormatMode::UPPERCASE:
            oss << std::uppercase;
            break;

        default:
            throw std::invalid_argument( "invalid hex format" );

    }

    const auto bytes = static_cast<const byte_t*>( data );

    for ( std::size_t i = 0; i < size; i++ ) {

        oss << std::setw( 2 ) << static_cast<unsigned int>( bytes[i] );

    }

    return oss.str();

}


std::string utils::to_hex( const data_t& data ) {

    return to_hex( data.data(), data.size() );

}


std::string utils::to_hex( const std::string& data ) {

    return to_hex( data.data(), data.size() );

}