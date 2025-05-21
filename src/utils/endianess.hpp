/*!
 *  \file
 *  \brief Header file containing template functions to manipulate endianess.
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


#include <type_traits>

#include <sfap.hpp>


namespace sfap {

    namespace utils {


        template<typename T>
        T hton( const T& value ) {

            static_assert( std::is_integral_v<T>, "integral type required (utils::endianess)" );
            static_assert( sizeof( T ) == 1 || sizeof( T ) == 2 || sizeof( T ) == 4 || sizeof( T ) == 8, "unsupported integral size (utils::endianess)" );

            #if defined( _WIN32 ) || ( defined( __BYTE_ORDER__ ) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ )

                if constexpr ( sizeof( T ) == 1 ) {

                    return value;

                }
                else if constexpr ( sizeof( T ) == 2 ) {

                    word_t v = static_cast<word_t>( value );

                    #if defined( __clang__ ) || defined( __GNUC__ )

                        v = __builtin_bswap16( v );

                    #else

                        v = ( v << 8 ) | ( v >> 8 );

                    #endif

                    return static_cast<T>( v );

                } else if constexpr ( sizeof( T ) == 4 ) {

                    dword_t v = static_cast<dword_t>( value );

                    #if defined( __clang__ ) || defined( __GNUC__ )

                        v = __builtin_bswap32( v );

                    #else

                        v = (

                            ( ( v & 0x000000FFU ) << 24 ) |
                            ( ( v & 0x0000FF00U ) << 8 ) |
                            ( ( v & 0x00FF0000U ) >> 8 ) |
                            ( ( v & 0xFF000000U ) >> 24 )

                        );

                    #endif

                    return static_cast<T>( v );

                } else if constexpr ( sizeof( T ) == 8 ) {

                    qword_t v = static_cast<qword_t>( value );

                    #if defined( __clang__ ) || defined( __GNUC__ )

                        v = __builtin_bswap64( v );

                    #else

                        v = (

                            ( ( v & 0x00000000000000FFULL) << 56 ) |
                            ( ( v & 0x000000000000FF00ULL) << 40 ) |
                            ( ( v & 0x0000000000FF0000ULL) << 24 ) |
                            ( ( v & 0x00000000FF000000ULL) << 8 ) |
                            ( ( v & 0x000000FF00000000ULL) >> 8 ) |
                            ( ( v & 0x0000FF0000000000ULL) >> 24 ) |
                            ( ( v & 0x00FF000000000000ULL) >> 40 ) |
                            ( ( v & 0xFF00000000000000ULL) >> 56 )

                        );

                    #endif

                    return static_cast<T>( v );

                }

            #else

                return value;

            #endif

        }


        template<typename T>
        T ntoh( const T& value ) {

            return hton( value );

        }


    }

}