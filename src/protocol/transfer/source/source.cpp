/*!
 *  \file
 *  \brief Transfer Source types definitions.
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
#include <protocol/transfer/source/source.hpp>


using namespace sfap;
using namespace sfap::protocol;


TransferSource::TransferSource( dword_t chunk_size ) :
    _chunk_size( chunk_size )
{

    if ( _chunk_size == 0 ) {

        throw std::invalid_argument( "chunk size must be greater than 0" );

    }

}


dword_t TransferSource::chunk_size() const noexcept {

    return _chunk_size;

}


TransferSource::operator bool() const noexcept {

    return !eof();
    
}


bool TransferSource::eof() const noexcept {

    return remaining() == 0;

}