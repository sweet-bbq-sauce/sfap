/*!
 *  \file
 *  \brief Utility functions for converting binary data to hexadecimal string representation.
 *
 *  This header defines helper functions and types used to convert raw binary buffers,
 *  data containers and arbitrary objects into human-readable hexadecimal strings.
 *  It also provides global configuration for selecting lowercase or uppercase output format.
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


#include <string>

#include <sfap.hpp>


namespace sfap {

    namespace utils {


        /*!
         *  \enum HexFormatMode
         *  \brief Defines the output case format for hexadecimal strings.
         */
        enum class HexFormatMode {

            LOWERCASE,  ///< Lowercase hexadecimal digits (e.g. "0a3f").
            UPPERCASE   ///< Uppercase hexadecimal digits (e.g. "0A3F").

        };


        /*!
         *  \brief Sets the global hexadecimal output format mode.
         *
         *  \param mode The desired hexadecimal format (lowercase or uppercase).
         */
        void set_hex_format_mode( HexFormatMode mode );


        /*!
         *  \brief Converts a raw binary buffer to a hexadecimal string.
         *
         *  \param data Pointer to the raw binary data.
         *  \param size Size of the buffer in bytes.
         *  \return Hexadecimal string representation of the input data.
         */
        std::string to_hex( const void* data, std::size_t size );

        /*!
         *  \brief Converts a binary data container to a hexadecimal string.
         *
         *  \param data Binary data container (e.g. vector of bytes).
         *  \return Hexadecimal string representation of the input data.
         */
        std::string to_hex( const data_t& data );

        /*!
         *  \brief Converts a std::string's raw bytes to a hexadecimal string.
         *
         *  \param data String whose contents will be converted to hexadecimal.
         *  \return Hexadecimal string representation of the input string's data.
         */
        std::string to_hex( const std::string& data );

        /*!
         *  \brief Converts an arbitrary object to a hexadecimal string based on its raw memory layout.
         *
         *  \tparam T Type of the object.
         *  \param object Object instance to convert.
         *  \return Hexadecimal string representation of the object's memory.
         */
        template<typename T>
        inline std::string to_hex_object( const T& object ) {

            return to_hex( &object, sizeof( object ) );

        }


    }

}