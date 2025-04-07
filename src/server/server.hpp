#pragma once


#include <vector>
#include <thread>
#include <functional>
#include <string>
#include <optional>

#include <net/address/address.hpp>
#include <net/listener/listener.hpp>
#include <server/session/session.hpp>
#include <protocol/protocol.hpp>
#include <utils/magic/magic.hpp>


namespace sfap {

    
    class Server {

        public:

            friend class protocol::Session;


            using CommandMiddleware = std::function<protocol::CommandResult( protocol::CommandList, const std::string&, const bool& )>;
            using LoginMiddleware = std::function<protocol::LoginResult( const Credentials&, std::string&, path_t&, path_t& )>;
            using ConnectionMiddleware = std::function<protocol::ConnectionResult( const net::Host& host, const std::string& version )>;


            Server( const net::Address& address );

            void join();

            void set_magic_context( const utils::Magic& ctx ) {

                magic = std::make_optional( std::ref( ctx ) );

            }

            CommandMiddleware command_middleware;
            LoginMiddleware login_middleware;
            ConnectionMiddleware connection_middleware;

            std::optional<std::reference_wrapper<const utils::Magic>> magic;


        private:

            std::thread _listener_thread;

            void _listener_loop();

            net::Listener _listener;
            std::vector<std::unique_ptr<protocol::Session>> _session_list;
            bool _shutdown;

    };


}