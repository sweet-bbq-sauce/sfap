/*!
 *  \file
 *  \brief Header file containing CRC utils definition.
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

    namespace utils {


        /*!
         *  \namespace sfap::utils::CRC
         *  \brief Contains utility functions for CRC32 checksum calculations.
         *
         *  This namespace provides a collection of static-like functions for computing
         *  CRC32 checksums of raw memory buffers or files using the standard
         *  CRC-32 algorithm (polynomial 0x04C11DB7, reflected input/output).
         *
         *  These functions serve as lightweight, stateless helpers for validating
         *  data integrity, commonly used in communication protocols or file handling.
         *
         *  Internally, the implementation is based on zlib's crc32 routine.
         */
        namespace CRC {


            /*!
             *  \brief Calculates an incremental CRC32 checksum.
             *
             *  Allows extending an existing CRC calculation with more data.
             *
             *  \param current Current CRC32 value (initial should be 0).
             *  \param data Pointer to data buffer.
             *  \param size Size of the buffer in bytes.
             *  \return Updated CRC32 value after processing the given buffer.
             *
             *  \throws std::invalid_argument If data is null and size is non-zero.
             */
            crc_t data_incremental( crc_t current, const void* data, dword_t size );


            /*!
             *  \brief Calculates a full CRC32 checksum for a memory buffer.
             *
             *  Internally processes the buffer in safe chunks.
             *
             *  \param data Pointer to the memory buffer.
             *  \param size Size of the buffer in bytes.
             *  \return CRC32 value.
             *
             *  \throws std::invalid_argument If data is null and size is non-zero.
             */
            crc_t data( const void* data, std::size_t size );


            /*!
             *  \brief Calculates a CRC32 checksum for a file.
             *
             *  Opens and reads the file in binary mode, block by block.
             *
             *  \param path Path to the file to be checked.
             *  \return CRC32 value.
             *
             *  \throws std::invalid_argument If the file doesn't exist or is not a regular file.
             *  \throws std::runtime_error If the file can't be opened.
             */
            crc_t file( const path_t& path );


        }

    }

}