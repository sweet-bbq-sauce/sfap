#pragma once


#include <map>
#include <functional>
#include <string>
#include <algorithm>

#include <sfap.hpp>
#include <server/session/session.hpp>
#include <protocol/protocol.hpp>
#include <net/iosocket/iosocket.hpp>


namespace sfap {

    namespace protocol {


        class Commands {

            public:

                using Procedure = std::function<void( Session&, const net::IOSocket& )>;


                Commands() = delete;
                Commands( const Commands& ) = delete;
                Commands& operator=( const Commands& ) = delete;


                static void add_command( protocol::CommandList id, const std::string& name, Procedure procedure );

                static std::string get_name( protocol::CommandList id );

                static bool has_command( protocol::CommandList id );
                static bool has_command_by_name( const std::string& name );

                static void execute( protocol::CommandList id, Session& session );

            
            private:

                static std::map<protocol::CommandList, std::pair<std::string, Procedure>> _commands;

        };


    }

}