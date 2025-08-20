/*!
 *  \file
 *  \brief Static helper functions for one-shot data and file hashing.
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

#include <sfap.hpp>
#include <crypto/hash/hash.hpp>


using namespace sfap;
using namespace sfap::crypto;


data_t Hash::hash_data( HashAlgorithm algorithm, const void* data, std::size_t size ) {

    Hash hash( algorithm );

    hash.update( data, size );

    return hash.finish();

}


data_t Hash::hash_data( HashAlgorithm algorithm, const data_t& data ) {

    Hash hash( algorithm );

    hash.update( data.data(), data.size() );

    return hash.finish();
                    
}


data_t Hash::hash_data( HashAlgorithm algorithm, const std::string& data ) {

    Hash hash( algorithm );

    hash.update( data.data(), data.size() );

    return hash.finish();
                    
}


data_t Hash::hash_file( HashAlgorithm algorithm, const path_t& path, std::size_t buffer_size ) {

    std::ifstream input( path, std::ios::binary );

    if ( !input ) {

        throw std::runtime_error( "can't open file " + path.string() + " to calculate hash" );

    }

    Hash hash( algorithm );
    std::vector<char> buffer( buffer_size );

    while ( !input.eof() ) {

        input.read( buffer.data(), sizeof( buffer.size() ) );

        hash.update( buffer.data(), static_cast<std::size_t>( input.gcount() ) );

    }

    return hash.finish();
                    
}


data_t Hash::hash_stream( HashAlgorithm algorithm, std::istream& input, std::optional<qword_t> size, std::size_t buffer_size ) {

    const auto begin = input.tellg();
    if ( begin == std::streampos( -1 ) ) throw std::runtime_error( "bad stream" );

    input.seekg( 0, std::ios::end );
    const auto end = input.tellg();
    if ( end == std::streampos( -1 ) ) throw std::runtime_error( "bad stream" );

    const std::streamoff diff = end - begin;

    if ( diff < 0 ) throw std::runtime_error( "negative range" );

    qword_t remaining;

    if ( size ) {

        if ( *size > static_cast<qword_t>( diff ) ) {

            throw std::runtime_error( "too high size for this stream" );

        }

        remaining = *size;

    } else {

        remaining = static_cast<qword_t>( diff );

    }

    input.clear();
    input.seekg( begin );

    std::vector<char> buffer( buffer_size );
    Hash hash( algorithm );

    while ( remaining > 0 ) {

        const std::streamsize want = static_cast<std::streamsize>( std::min<qword_t>( remaining, buffer.size() ) );

        input.read( buffer.data(), want );
        const std::streamsize got = input.gcount();

        if ( got <= 0 ) throw std::runtime_error( "short read" );

        hash.update( buffer.data(), static_cast<std::size_t>( got ) );
        remaining -= static_cast<qword_t>( got );

    }

    return hash.finish();

}