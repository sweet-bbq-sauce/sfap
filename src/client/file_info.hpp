/*!
 *  \file
 *  \brief Header file containing `FileInfo` class declaration.
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
#include <optional>

#include <sfap.hpp>


namespace sfap {

    class Client;

    namespace utils {


        /*!
         *  \class FileInfo
         *  \brief Represents information about a file or directory in the virtual filesystem.
         *
         *  This class stores basic metadata such as path, file type and size.
         *  It is used internally by the SFAP client to represent remote file system entries.
         */
        class FileInfo {

            public:

                friend class sfap::Client;


                /*!
                 *  \brief Returns the path to the file or directory.
                 *  \return Reference to the file path.
                 */
                const path_t& get_path() const noexcept;


                /*!
                 *  \brief Returns the file name.
                 *  \return File name.
                 */
                path_t get_filename() const noexcept;


                /*!
                 *  \brief Returns the type of the file (regular file, directory, symlink, etc.).
                 *  \return File type as defined in `std::filesystem::file_type`.
                 */
                std::filesystem::file_type get_type() const noexcept;


                /*!
                 *  \brief Returns the file size in bytes.
                 *  \return File size. For directories or special files, may be undefined or zero.
                 */
                qword_t get_size() const noexcept;


            private:

                /*!
                 *  \brief Constructs an empty FileInfo object.
                 *
                 *  Intended to be used only by the friend class `Client`.
                 */
                explicit FileInfo() noexcept;

                path_t _path;   ///< Full path to the file.
                std::filesystem::file_type _type;       ///< Type of the file.
                qword_t _size;      ///< Size of the file in bytes.

        };


    }

}