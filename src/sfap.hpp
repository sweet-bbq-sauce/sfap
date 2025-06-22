/*!
 *  \file
 *  \brief Main header file containing global types and definitions for the project.
 *
 *  This file defines global types, constants, and declarations that are used across the libSFAP.
 *  It serves as the central interface for other components to interact with shared types.
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


#include <filesystem>
#include <vector>
#include <iostream>

#ifdef _WIN32

    #include <winsock2.h>
    #include <ws2tcpip.h>

#else

    #include <sys/socket.h>

    #define INVALID_SOCKET -1
    #define closesocket ::close

#endif

#include <config.hpp>


namespace sfap {


    /*!
     *  \typedef byte_t
     *  \brief Alias for uint8_t to represent a single byte.
     */
    using byte_t = uint8_t;


    /*!
     *  \typedef word_t
     *  \brief Alias for `uint16_t` to represent a 16-bit value.
     */
    using word_t = uint16_t;


    /*!
     *  \typedef dword_t
     *  \brief Alias for `uint32_t` to represent a 32-bit value.
     */
    using dword_t = uint32_t;


    /*!
     *  \typedef qword_t
     *  \brief Alias for `uint64_t` to represent a 64-bit value.
     */
    using qword_t = uint64_t;


    /*!
     *  \typedef data_t
     *  \brief Alias for `std::vector<byte_t>` to represent a binary array.
     * 
     *  This type is used to represent an array of bytes, typically used for buffers or raw data manipulation.
     */
    using data_t = std::vector<byte_t>;


    /*!
     *  \typedef path_t
     *  \brief Alias for `std::filesystem::path`.
     */
    using path_t = std::filesystem::path;


    /*!
     *  \typedef virtual_path_t
     *  \brief Alias for `path_t` to represent virtual path.
     */
    using virtual_path_t = path_t;


    /*!
     *  \typedef port_t
     *  \brief Alias for `uint16_t` to represent network port.
     */
    using port_t = word_t;


    /*!
     *  \typedef crc_t
     *  \brief Alias for `dword_t` to represent CRC value.
     */
    using crc_t = dword_t;


    #ifdef _WIN32

        /*!
         *  \typedef socket_t
         *  \brief Alias for `SOCKET` to represent network socket.
         * 
         *  On Windows platforms, `SOCKET` is used as the type for network socket handles.
         */
        using socket_t = SOCKET;

    #else

        /*!
         *  \typedef socket_t
         *  \brief Alias for `int` to represent network socket.
         * 
         *  On Unix-like platforms, an `int` is used to represent a network socket.
         */
        using socket_t = int;

    #endif


}