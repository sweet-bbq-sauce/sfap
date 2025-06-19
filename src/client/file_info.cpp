/*!
 *  \file
 *  \brief Source file containing `FileInfo` class definition.
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


#include <filesystem>
#include <optional>

#include <sfap.hpp>
#include <client/file_info.hpp>


using namespace sfap;
using namespace sfap::utils;


FileInfo::FileInfo() noexcept {}


const path_t& FileInfo::get_path() const noexcept {

    return _path;
    
}


path_t FileInfo::get_filename() const noexcept {

    return _path.filename();

}


FileInfo::type FileInfo::get_type() const noexcept {

    return _type;

}


qword_t FileInfo::get_size() const noexcept {

    return _size;

}


FileInfo::type FileInfo::convert_type_from_std( std::filesystem::file_type std_type ) {

    using ft = std::filesystem::file_type;

    switch ( std_type ) {

        case ft::none:          return FileInfo::type::NONE;

        case ft::directory:     return FileInfo::type::DIRECTORY;

        case ft::regular:       return FileInfo::type::REGULAR;
        case ft::symlink:       return FileInfo::type::SYMLINK;
        case ft::block:         return FileInfo::type::BLOCK;
        case ft::character:     return FileInfo::type::CHARACTER;
        case ft::socket:        return FileInfo::type::SOCKET;

        case ft::not_found:     return FileInfo::type::NOT_FOUND;
        case ft::unknown:       return FileInfo::type::UNKNOWN;

        default:

            throw std::invalid_argument( "unknown file type" );

    }

}