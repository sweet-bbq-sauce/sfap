#include <server/command/command.hpp>
#include <protocol/protocol.hpp>


using namespace sfap::protocol;


std::map<CommandList, std::pair<std::string, Commands::Procedure>> Commands::_commands;


void Commands::add_command( protocol::CommandList id, const std::string& name, Procedure procedure ) {

    if ( has_command( id ) ) throw std::runtime_error( "Command " + std::to_string( static_cast<word_t>( id ) ) + " already exists (" + _commands[id].first + ")" );
    if ( has_command_by_name( name ) ) throw std::runtime_error( "Command " + std::to_string( static_cast<word_t>( id ) ) + " already has \"" + _commands[id].first + "\" name" );

    _commands[id] = { name, procedure };

}


bool Commands::has_command( protocol::CommandList id ) {

    return _commands.find( id ) != _commands.end();

}


bool Commands::has_command_by_name( const std::string& name ) {

    for ( const auto& cmd : _commands ) {

        if ( cmd.second.first == name ) return true;

    }

    return false;

}


void Commands::execute( protocol::CommandList id, Session& session ) {

    if ( !has_command( id ) ) throw std::runtime_error( "Command " + std::to_string( static_cast<word_t>( id ) ) + " does not exists" );

    _commands[id].second( session );

}


std::string Commands::get_name( protocol::CommandList id ) {

    const auto cmd = _commands.find( id );

    if ( cmd == _commands.end() ) throw std::runtime_error( "Command not found" );

    else return cmd->second.first;

}