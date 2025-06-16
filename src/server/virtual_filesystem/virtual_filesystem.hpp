/*!
 *  \file
 *  \brief Header file containing `VirtualFilesystem` class declaration.
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


#include <optional>
#include <stdexcept>
#include <string>


#include <sfap.hpp>
#include <protocol/protocol.hpp>
#include <utils/encoding.hpp>


namespace sfap {

    namespace protocol {

        /*!
         *  \class VirtualFilesystem
         *  \brief Represents a virtual filesystem rooted at a specified directory.
         * 
         *  Provides operations on virtual paths relative to the root directory, 
         *  manages the home directory and current working directory, and converts 
         *  between system and virtual paths.
         */
        class VirtualFilesystem {

            public:

                /// Alias for the type representing a virtual path.
                using virtual_path_t = path_t;


                /*!
                 *  \brief Constructs a VirtualFilesystem with the specified root directory.
                 *  \param root_directory The root directory, must be an absolute path.
                 *  \throws std::invalid_argument if root_directory is not absolute.
                 */
                explicit VirtualFilesystem( const path_t& root_directory );


                /*!
                 *  \brief Returns the root directory of the virtual filesystem.
                 *  \return The root directory as a system path.
                 */
                path_t get_root() const noexcept;


                /*!
                 *  \brief Checks whether the given system path is inside the virtual filesystem root.
                 *  \param path The system path to check.
                 *  \return AccessResult::OK if the path is inside the root, otherwise AccessResult::OUTSIDE_ROOT.
                 */
                protocol::AccessResult check_access( const path_t& path ) const;


                /*!
                 *  \brief Sets the home directory within the virtual filesystem.
                 *  \param home The new home directory as a virtual path.
                 *  \return AccessResult::OK if successful, AccessResult::ACCESS_DENIED otherwise.
                 */
                protocol::AccessResult set_home( const virtual_path_t& home ) noexcept;


                /*!
                 *  \brief Gets the current home directory.
                 *  \return The home directory as a virtual path.
                 */
                virtual_path_t get_home() const noexcept;
                

                /*!
                 *  \brief Changes the current working directory.
                 *  \param directory The new directory as a virtual path.
                 *  \return AccessResult::OK if successful, AccessResult::ACCESS_DENIED otherwise.
                 */
                protocol::AccessResult cd( const virtual_path_t& directory );


                /*!
                 *  \brief Returns the current working directory.
                 *  \return The current working directory as a virtual path.
                 */
                virtual_path_t pwd() const noexcept;


                /*!
                 *  \brief Converts a virtual path to a system path.
                 *  \param path The virtual path to convert.
                 *  \return The corresponding system path.
                 */
                path_t to_system( const virtual_path_t& path ) const;


                /*!
                 *  \brief Converts a system path to a virtual path.
                 *  \param path The system path to convert.
                 *  \throws std::invalid_argument if the path is outside of the root.
                 *  \return The corresponding virtual path.
                 */
                virtual_path_t to_virtual( const path_t& path ) const;


                /*!
                 *  \brief Attempts to convert a system path to a virtual path.
                 *  \param path The system path to convert.
                 *  \return An optional virtual path; std::nullopt if conversion fails.
                 */
                std::optional<virtual_path_t> try_to_virtual( const path_t& path ) const noexcept;


                /*!
                 *  \brief Normalizes a virtual path by converting it to system and back to virtual.
                 *  \param path The virtual path to normalize.
                 *  \return The normalized virtual path.
                 */
                virtual_path_t normalize( const virtual_path_t& path ) const;


                /*!
                 *  \brief Attempts to normalize a system path.
                 *  \param path The system path to normalize.
                 *  \return An optional normalized virtual path; std::nullopt if normalization fails.
                 */
                std::optional<virtual_path_t> try_normalize( const virtual_path_t& path ) const noexcept;

            
            private:

                path_t _root_directory;     ///< The root directory in system path form.
                path_t _home_directory;     ///< The home directory as a virtual path.
                path_t _current_working_directory;      ///< The current working directory as a virtual path.
                

        };


    }

}