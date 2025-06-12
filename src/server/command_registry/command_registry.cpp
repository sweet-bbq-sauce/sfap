/*!
 *  \file
 *  \brief Source file containing `CommandRegistry` class definition.
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


#include <algorithm>
#include <functional>

#include <server/command_registry/command_registry.hpp>


using namespace sfap;
using namespace sfap::protocol;


void CommandRegistry::add( word_t id, const std::string& name, CommandProcedure procedure ) {

    std::unique_lock lock( _registry_mutex );

    if ( _registry.find( id ) != _registry.cend() ) {

        throw std::runtime_error( "command with that ID already exists" );

    }

    if (
                    
        std::any_of( _registry.begin(), _registry.end(), [&name]( const auto& pair ) {

            return pair.second.first == name;

        })
                        
    ) {

        throw std::runtime_error( "command with that name already exists" );

    }

    _registry.emplace( id, std::make_pair( name, procedure ) );

}


void CommandRegistry::add( protocol::Command id, const std::string& name, CommandProcedure procedure ) {

    add( static_cast<word_t>( id ), name, procedure );

}


void CommandRegistry::remove( word_t id ) {

    std::unique_lock lock( _registry_mutex );
    _registry.erase( id );

}


void CommandRegistry::remove( const std::string& name ) {

    remove( get_by_name( name ) );

}


void CommandRegistry::remove( protocol::Command id ) {

    remove( static_cast<word_t>( id ) );

}


word_t CommandRegistry::size() const noexcept {

    std::shared_lock lock( _registry_mutex );

    return _registry.size();

}


ServerCommandsTable CommandRegistry::get_command_list() const noexcept {

    std::shared_lock lock( _registry_mutex );

    ServerCommandsTable buffer;

    for ( const auto& command : _registry ) {

        buffer.emplace( command.first, command.second.first );

    }

    return buffer;

}


bool CommandRegistry::empty() const noexcept {

    std::shared_lock lock( _registry_mutex );

    return _registry.empty();

}


word_t CommandRegistry::get_by_name( const std::string& name ) const {

    std::shared_lock lock( _registry_mutex );

    const auto it = std::find_if( _registry.begin(), _registry.end(), [&name]( const auto& pair ) {

        return pair.second.first == name;

    });

    if ( it == _registry.cend() ) {

        throw std::runtime_error( "command with that name does not exist" );

    }

    return it->first;
                
}


bool CommandRegistry::exists( word_t id ) const {

    std::shared_lock lock( _registry_mutex );
    return _registry.find( id ) != _registry.end();

}


bool CommandRegistry::exists( const std::string& name ) const {

    std::shared_lock lock( _registry_mutex );

    return std::any_of( _registry.begin(), _registry.end(), [&name]( const auto& pair ) {

        return pair.second.first == name;

    });

}


const CommandRegistry::CommandProcedure& CommandRegistry::operator[]( word_t id ) const {

    if ( !exists( id ) ) {

        throw std::invalid_argument( "command does not exists" );

    }

    std::shared_lock lock( _registry_mutex );

    return _registry.at( id ).second;

}


const CommandRegistry::CommandProcedure& CommandRegistry::operator[]( protocol::Command id ) const {

    return (*this)[static_cast<word_t>( id )];

}


const CommandRegistry::CommandProcedure& CommandRegistry::operator[]( const std::string& name ) const {

    return (*this)[get_by_name( name )];

}


CommandRegistry& CommandRegistry::operator=( const CommandRegistry& other ) noexcept {

    if ( this != &other ) {

        std::unique_lock lock( _registry_mutex );
        std::shared_lock lock_other( other._registry_mutex );
        _registry = other._registry;

    }

    return *this;

}


CommandRegistry::CommandRegistry() noexcept {}


CommandRegistry::CommandRegistry( const CommandRegistry& other ) noexcept {

    std::unique_lock lock( _registry_mutex );
    std::shared_lock lock_other( other._registry_mutex );
    _registry = other._registry;

}


void CommandRegistry::add( const CommandRegistry& other ) {

    std::shared_lock other_lock( other._registry_mutex );
    std::unique_lock this_lock( _registry_mutex );

    for ( const auto& command : other._registry ) {

        add( command.first, command.second.first, command.second.second );

    }

}