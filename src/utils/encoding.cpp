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


#include <utils/encoding.hpp>


using namespace sfap;


path_t utils::string_to_path( const std::string& value ) {

    #ifdef _WIN32

        const int required_size = MultiByteToWideChar( CP_UTF8, 0, value.c_str(), -1, nullptr, 0 );
        std::wstring result( required_size - 1, 0 );

        MultiByteToWideChar( CP_UTF8, 0, value.c_str(), -1, &result[0], required_size );

        return path_t( result );

    #else

        return path_t( value );

    #endif

}


std::string utils::path_to_string( const path_t& value ) {

    #ifdef _WIN32

        const int required_size = WideCharToMultiByte( CP_UTF8, 0, value.c_str(), -1, nullptr, 0, nullptr, nullptr );
        std::string result( required_size - 1, 0 );

        WideCharToMultiByte( CP_UTF8, 0, value.c_str(), -1, &result[0], required_size, nullptr, nullptr );

        return result;

    #else

        return value.generic_string();

    #endif

}