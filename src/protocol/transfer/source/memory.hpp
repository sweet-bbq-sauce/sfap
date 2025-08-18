/*!
 *  \file
 *  \brief Transfer from memory source types declarations.
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


#include <sfap.hpp>
#include <protocol/transfer/source/source.hpp>


namespace sfap {

    namespace protocol {


        /*!
         *  \class TransferSourceMemory
         *  \brief Concrete implementation of TransferSource for memory buffers.
         *
         *  Provides read access to a preallocated memory region in fixed-size chunks.
         */
        class TransferSourceMemory : public TransferSource {

            public:

                /*!
                 *  \brief Constructs a TransferSourceMemory object.
                 *
                 *  \param data Pointer to the memory buffer (may be nullptr if size is 0).
                 *  \param size Size of the buffer in bytes.
                 *  \param chunk_size Size of each chunk to be returned by get_chunk().
                 *
                 *  \throws std::invalid_argument if \p data is nullptr and \p size > 0.
                 *  \throws std::invalid_argument if \p chunk_size is 0.
                 */
                explicit TransferSourceMemory( const void* data, std::size_t size, dword_t chunk_size );

                TransferSourceMemory( const TransferSourceMemory& ) = delete;
                TransferSourceMemory& operator=( const TransferSourceMemory& ) = delete;
                TransferSourceMemory( TransferSourceMemory&& ) = default;
                TransferSourceMemory& operator=( TransferSourceMemory&& ) = default;

                /*!
                 *  \copydoc TransferSource::size()
                 */
                qword_t size() const noexcept override;

                /*!
                 *  \copydoc TransferSource::remaining()
                 */
                qword_t remaining() const noexcept override;

                /*!
                 *  \copydoc TransferSource::tellg()
                 */
                qword_t tellg() const noexcept override;

                /*!
                 *  \copydoc TransferSource::seekg()
                 */
                void seekg( qword_t position ) override;

                /*!
                 *  \copydoc TransferSource::rewind()
                 */
                void rewind() noexcept override;

                /*!
                 *  \copydoc TransferSource::get_chunk()
                 */
                [[nodiscard]] std::pair<const void*, dword_t> get_chunk() noexcept override;

                /*!
                 *  \copydoc TransferSource::peek_chunk()
                 */
                [[nodiscard]] std::pair<const void*, dword_t> peek_chunk() noexcept override;


            private:

                const void* const _data;    ///< Pointer to the beginning of the memory buffer.
                const qword_t _size;        ///< Total size of the memory buffer in bytes.
                qword_t _position = 0;      ///< Current read position in bytes.

        };


    }

}