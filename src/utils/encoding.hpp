/*!
 *  \file
 *  \brief Converting from UTF8 to UTF16 and backwards.
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
         *  \brief Converts a UTF-8 encoded std::string to a platform-specific path type.
         *
         *  On Windows, the string is converted to a wide Unicode path. On other platforms,
         *  the string is used directly as a UTF-8 path.
         *
         *  \param value UTF-8 encoded string representing a filesystem path.
         *  \return path_t A platform-appropriate path object.
         */
        path_t string_to_path( const std::string& value );


        /**
         *  \brief Converts a platform-specific path type to a UTF-8 encoded std::string.
         *
         *  On Windows, the path is converted from wide Unicode to UTF-8. On other platforms,
         *  the generic string representation is returned.
         *
         *  \param value A filesystem path to be converted to string.
         *  \return std::string UTF-8 encoded string representing the path.
         */
        std::string path_to_string( const path_t& value );


    }

}