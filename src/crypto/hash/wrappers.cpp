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


data_t Hash::hash_file( HashAlgorithm algorithm, const path_t& path ) {

    std::ifstream input( path, std::ios::binary );

    if ( !input ) {

        throw std::runtime_error( "can't open file " + path.string() + " to calculate hash" );

    }

    Hash hash( algorithm );
    char buffer[4096];

    while ( !input.eof() ) {

        input.read( buffer, sizeof( buffer ) );

        hash.update( buffer, static_cast<std::size_t>( input.gcount() ) );

    }

    return hash.finish();
                    
}