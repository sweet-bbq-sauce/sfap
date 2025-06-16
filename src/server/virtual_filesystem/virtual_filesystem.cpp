/*!
 *  \file
 *  \brief Header file containing `VirtualFilesystem` class definition.
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


#include <optional>
#include <stdexcept>
#include <string>


#include <sfap.hpp>
#include <protocol/protocol.hpp>
#include <server/virtual_filesystem/virtual_filesystem.hpp>
#include <utils/encoding.hpp>


using namespace sfap;
using namespace sfap::protocol;


path_t remove_root( const path_t& p ) {

    if ( p.has_root_path() ) {

        path_t new_p;

        for ( auto it = ++p.begin(); it != p.end(); ++it ) {

            new_p /= *it;

        }

        return new_p;

    }
    else {

        return p;

    }

}


path_t remove_ending_slash( const path_t& p ) {

    std::string str = p.generic_string();

    if ( str.back() == '/' ) {

        str.pop_back();

        return path_t( str );

    }
    else {

        return path_t( str );

    }

}


VirtualFilesystem::VirtualFilesystem( const path_t& root_directory ) {

    if ( !root_directory.is_absolute() ) {

        throw std::invalid_argument( "root must be absolute" );

    }

    _root_directory = std::filesystem::weakly_canonical( root_directory ).native();
    _home_directory = to_virtual( _root_directory );
    _current_working_directory = _home_directory;

}


protocol::AccessResult VirtualFilesystem::check_access( const path_t& path ) const {

    const path_t canonical = std::filesystem::weakly_canonical( path );

    auto root_iter = _root_directory.begin();
    auto path_iter = canonical.begin();

    for ( ; root_iter != _root_directory.end() && path_iter != canonical.end(); ++root_iter, ++path_iter ) {

        if ( *root_iter != *path_iter ) {

            return protocol::AccessResult::OUTSIDE_ROOT;

        }

    }

    return ( root_iter == _root_directory.end() ) ? protocol::AccessResult::OK : protocol::AccessResult::OUTSIDE_ROOT;

}


AccessResult VirtualFilesystem::set_home( const virtual_path_t& home ) noexcept {

    try {

        _home_directory = normalize( home );

        return AccessResult::OK;
        
    }
    catch ( ... ) {

        return AccessResult::ACCESS_DENIED;

    }

}


VirtualFilesystem::virtual_path_t VirtualFilesystem::get_home() const noexcept {

    return _home_directory.generic_string();

}
                

AccessResult VirtualFilesystem::cd( const virtual_path_t& directory ) {

    try {

        _current_working_directory = normalize( directory );

        return AccessResult::OK;

    }
    catch ( ... ) {

        return AccessResult::ACCESS_DENIED;

    }

}


VirtualFilesystem::virtual_path_t VirtualFilesystem::pwd() const noexcept {

    return _current_working_directory.generic_string();

}


path_t VirtualFilesystem::to_system( const virtual_path_t& path ) const {

    const auto path_string = path.string();
    path_t buffer;

    if ( path_string.find( '~' ) == 0 ) {

        buffer = _home_directory / remove_root( path_string.substr( 1 ) );

    }
    else if ( path.is_absolute() ) {

        buffer = path;

    }
    else {

        buffer = _current_working_directory / path;

    }

    buffer = remove_ending_slash( buffer );

    return std::filesystem::weakly_canonical( _root_directory / remove_root( buffer ) ).native();

}


VirtualFilesystem::virtual_path_t VirtualFilesystem::to_virtual( const path_t& path ) const {

    if ( check_access( path ) != AccessResult::OK ) {

        throw std::invalid_argument( "can't get access to " + path.string() );

    }

    const path_t rel = std::filesystem::relative( path, _root_directory );

    if ( rel == "." ) {

        return path_t( "/" );

    }

    return path_t( "/" ) / remove_ending_slash( rel );
                    
}


std::optional<VirtualFilesystem::virtual_path_t> VirtualFilesystem::try_to_virtual( const path_t& path ) const noexcept {

    try {

        return to_virtual( path );

    }
    catch ( ... ) {

        return std::nullopt;

    }
                    
}


VirtualFilesystem::virtual_path_t VirtualFilesystem::normalize( const virtual_path_t& path ) const {

    return to_virtual( to_system( path ) );

}


std::optional<VirtualFilesystem::virtual_path_t> VirtualFilesystem::try_normalize( const path_t& path ) const noexcept {

    return try_to_virtual( to_system( path ) );

}


path_t VirtualFilesystem::get_root() const noexcept {

    return _root_directory;

}