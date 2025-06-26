/*!
 *  \file
 *  \brief Header file containing CRC utils declaration.
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


#include <fstream>
#include <stdexcept>

#include <zlib.h>

#include <sfap.hpp>
#include <utils/crc.hpp>


using namespace sfap;
using namespace sfap::utils;


crc_t CRC::data_incremental( crc_t current, const void* data, dword_t size ) {

    if ( size == 0 ) {

        return current;

    }

    if ( data == nullptr ) {

        throw std::invalid_argument( "data is null" );

    }

    return crc32( current, static_cast<const Bytef*>( data ), size );

}


crc_t CRC::data( const void* data, std::size_t size ) {

    std::size_t calculated = 0ULL;
    crc_t current = 0UL;

    const auto* ptr = static_cast<const Bytef*>( data );

    while ( calculated < size ) {

        const uInt chunk = static_cast<uInt>( std::min( size - calculated, std::min( size - calculated, static_cast<std::size_t>( std::numeric_limits<uInt>::max() ) ) ) );

        current = CRC::data_incremental( current, ptr + calculated, chunk );

        calculated += chunk;

    }

    return current;

}


crc_t CRC::file( const path_t& path ) {

    if ( !std::filesystem::is_regular_file( path ) ) {

        throw std::invalid_argument( path.string() + " is not a file or not exists" );

    }

    std::ifstream stream( path, std::ios::binary );

    if ( !stream ) {

        throw std::runtime_error( "can't open file " + path.string() );

    }

    char buffer[1024];
    crc_t current = 0UL;

    while ( stream ) {

        stream.read( buffer, sizeof( buffer ) );

        const std::streamsize size = stream.gcount();

        if ( size > 0 ) {
                    
            current = CRC::data_incremental( current, buffer, size );

        }

    }

    return current;

}