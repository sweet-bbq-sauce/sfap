/*!
 *  \file
 *  \brief Transfer from memory source types definitions.
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


#include <algorithm>
#include <stdexcept>

#include <sfap.hpp>
#include <protocol/transfer/source/file.hpp>


using namespace sfap;
using namespace sfap::protocol;


TransferSourceFile::TransferSourceFile( std::ifstream& file, dword_t chunk_size, std::optional<qword_t> size ) :
    TransferSource( chunk_size ),
    _file( file ),
    _begin( ( [&]() -> std::streampos {

        const auto begin = _file.tellg();

        if ( !_file || begin == std::streampos( -1 ) ) {

            throw std::runtime_error( "bad file stream" );

        }

        return begin;

    })() ),
    _position( 0 ),
    _size( ( [&]() -> qword_t {

        file.seekg( 0, std::ios::end );

        const auto end_position = file.tellg();

        if ( end_position == std::streampos( -1 ) ) {

            throw std::runtime_error( "bad file stream" );

        }

        const auto diff = end_position - _begin;

        if ( size ) {

            if ( diff < *size ) {

                throw std::invalid_argument( "size is too high for this file" );

            }
            else {

                return *size;

            }

        }
        else {

            return static_cast<qword_t>( diff );

        }

    })() ),
    _buffer( this->chunk_size() )
{}


qword_t TransferSourceFile::size() const noexcept {

    return _size;

}


qword_t TransferSourceFile::tellg() const noexcept {

    return _position;
    
}


qword_t TransferSourceFile::remaining() const noexcept {

    return ( _position <= _size ) ? ( _size - _position ) : 0;

}


void TransferSourceFile::rewind() noexcept {

    _position = 0;

}


void TransferSourceFile::seekg( qword_t position ) {

    if ( position > _size ) {

        throw std::out_of_range( "too large position" );

    }

    _position = position;

}


std::pair<const void*, dword_t> TransferSourceFile::peek_chunk() {

    if ( eof() ) {

        return { nullptr, 0 };

    }

    _file.clear();
    _file.seekg( _begin + _position );

    if ( !_file || _file.tellg() == std::streampos( -1 ) ) {

        throw std::runtime_error( "bad file stream" );

    }

    const dword_t to_read = static_cast<dword_t>( std::min( static_cast<qword_t>( chunk_size() ), remaining() ) );

    _file.read( reinterpret_cast<char*>( _buffer.data() ), static_cast<std::streamsize>( to_read ) );

    const auto readen = _file.gcount();

    if ( readen != to_read ) {

        throw std::runtime_error( "read error" );

    }

    return { _buffer.data(), readen };

}


std::pair<const void*, dword_t> TransferSourceFile::get_chunk() {

    const auto result = peek_chunk();

    _position += static_cast<qword_t>( result.second );

    return result;

}