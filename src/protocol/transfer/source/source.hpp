/*!
 *  \file
 *  \brief Transfer Source types declarations.
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


namespace sfap {

    namespace protocol {


        /*!
         *  \class TransferSource
         *  \brief Abstract base class representing a generic data source for transfer.
         *
         *  Provides an interface for reading chunks of data sequentially, 
         *  querying the size, position, and checking the end-of-data state.
         */
        class TransferSource {

            public:

                /*!
                 *  \brief Constructs a TransferSource with a specified chunk size.
                 *
                 *  \param chunk_size Size of data chunks returned by get_chunk(). Must be greater than 0.
                 *  \throws std::invalid_argument if \p chunk_size is 0.
                 */
                explicit TransferSource( dword_t chunk_size );

                TransferSource( const TransferSource& ) = delete;
                TransferSource& operator=( const TransferSource& ) = delete;
                TransferSource( TransferSource&& ) = default;
                TransferSource& operator=( TransferSource&& ) = default;

                /*!
                 *  \brief Virtual destructor.
                 */
                virtual ~TransferSource() noexcept = default;

                /*!
                 *  \brief Returns the total size of the data source.
                 *  \return Size of the data in bytes.
                 */
                virtual qword_t size() const noexcept = 0;

                /*!
                 *  \brief Returns the chunk size.
                 *  \return Size of the chunk in bytes.
                 */
                dword_t chunk_size() const noexcept;

                /*!
                 *  \brief Returns the number of remaining bytes that can be read.
                 *  \return Remaining bytes.
                 */
                virtual qword_t remaining() const noexcept = 0;

                /*!
                 *  \brief Returns the current read position within the data source.
                 *  \return Current position in bytes.
                 */
                virtual qword_t tellg() const noexcept = 0;

                /*!
                 *  \brief Sets the current read position within the data source.
                 *  \param position New position in bytes.
                 *  \throws std::out_of_range if \p position is larger than the data size.
                 */
                virtual void seekg( qword_t position ) = 0;

                /*!
                 *  \brief Sets the current read position to begin.
                 */
                virtual void rewind() noexcept = 0;

                /*!
                 *  \brief Returns a pointer to the chunk of data.
                 *
                 *  Advances the read position by the chunk size or until the end of the data is reached.
                 *
                 *  \return A pair containing:
                 *          - Pointer to the data chunk (may be nullptr if EOF),
                 *          - Size of the chunk in bytes (0 if EOF).
                 */
                [[nodiscard]] virtual std::pair<const void*, dword_t> get_chunk() noexcept = 0;

                /*!
                 *  \brief Returns a pointer to the chunk of data without moving read position to the next chunk.
                 *
                 *  \return A pair containing:
                 *          - Pointer to the data chunk (may be nullptr if EOF),
                 *          - Size of the chunk in bytes (0 if EOF).
                 */
                [[nodiscard]] virtual std::pair<const void*, dword_t> peek_chunk() noexcept = 0;

                /*!
                 *  \brief Checks if the end of the data has been reached.
                 *  \return true if no more data is available, false otherwise.
                 */
                bool eof() const noexcept;

                /*!
                 *  \brief Checks if the data source is still valid for reading.
                 *  \return true if not at EOF, false otherwise.
                 */
                explicit operator bool() const noexcept;


            protected:

                const dword_t _chunk_size;  ///< Configured chunk size used in get_chunk().

        };


    }

}