/*!
 *  \file
 *  \brief Transfer from file source types declarations.
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


#include <fstream>
#include <optional>

#include <sfap.hpp>
#include <protocol/transfer/source/source.hpp>


namespace sfap {

    namespace protocol {


        /*!
         *  \class TransferSourceFile
         *  \brief Concrete implementation of TransferSource for file.
         *
         *  Provides read access to a preopened input file stream in fixed-size chunks.
         * 
         *  \note File stream must be opened with `binary` flag.
         */
        class TransferSourceFile : public TransferSource {

            public:

                /*!
                 *  \brief Constructs a TransferSourceFile object.
                 *
                 *  \throws std::invalid_argument if \p chunk_size is 0.
                 */
                explicit TransferSourceFile( std::ifstream& file, dword_t chunk_size, std::optional<qword_t> size = std::nullopt );

                TransferSourceFile( const TransferSourceFile& ) = delete;
                TransferSourceFile& operator=( const TransferSourceFile& ) = delete;
                TransferSourceFile( TransferSourceFile&& ) = default;
                TransferSourceFile& operator=( TransferSourceFile&& ) = default;

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
                [[nodiscard]] std::pair<const void*, dword_t> get_chunk() override;

                /*!
                 *  \copydoc TransferSource::peek_chunk()
                 */
                [[nodiscard]] std::pair<const void*, dword_t> peek_chunk() override;


            private:

                std::ifstream& _file;
                const std::streampos _begin;
                std::streamoff _position;
                const qword_t _size;
                mutable data_t _buffer;

        };


    }

}