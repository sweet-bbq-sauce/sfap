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
#include <protocol/transfer/source/memory.hpp>


using namespace sfap;
using namespace sfap::protocol;


TransferSourceMemory::TransferSourceMemory( const void* data, std::size_t size, dword_t chunk_size ) :
    TransferSource( chunk_size ),
    _data( data ),
    _size( static_cast<qword_t>( size ) )
{

    if ( _data == nullptr && size > 0 ) {

        throw std::invalid_argument( "size must be 0 when data is null" );

    }

}


qword_t TransferSourceMemory::size() const noexcept {

    return _size;

}


qword_t TransferSourceMemory::remaining() const noexcept {

    return ( _position <= _size ) ? ( _size - _position ) : 0;

}


qword_t TransferSourceMemory::tellg() const noexcept {

    return _position;

}


void TransferSourceMemory::seekg( qword_t position ) {

    if ( position > _size ) {

        throw std::out_of_range( "too large position" );

    }

    _position = position;

}


void TransferSourceMemory::rewind() noexcept {

    _position = 0;

}


std::pair<const void*, dword_t> TransferSourceMemory::peek_chunk() noexcept {

    const qword_t remaining_bytes = remaining();

    if ( remaining_bytes == 0 ) {

        return { nullptr, 0 };

    }

    const dword_t current_chunk_size = static_cast<dword_t>( std::min( static_cast<qword_t>( _chunk_size ),  remaining_bytes ) );

    const auto* base = static_cast<const byte_t*>( _data );
    const auto* ptr  = base + static_cast<std::size_t>( _position );

    return { ptr, current_chunk_size };

}


std::pair<const void*, dword_t> TransferSourceMemory::get_chunk() noexcept {

    const auto result = peek_chunk();

    _position += static_cast<qword_t>( result.second );

    return result;

}