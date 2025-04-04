#pragma once


#include <vector>
#include <thread>
#include <functional>
#include <string>

#include <net/address/address.hpp>
#include <net/listener/listener.hpp>
#include <server/session/session.hpp>
#include <protocol/protocol.hpp>


namespace sfap {

    
    class Server {

        public:

            friend class protocol::Session;


            using CommandMiddleware = std::function<protocol::CommandResult( protocol::CommandList, const std::string&, const bool& )>;
            using LoginMiddleware = std::function<protocol::LoginResult( const Credentials&, std::string&, bool&, path_t&, path_t& )>;


            Server( const net::Address& address );

            void join();

            CommandMiddleware command_middleware;
            LoginMiddleware login_middleware;


        private:

            std::thread _listener_thread;

            void _listener_loop();

            net::Listener _listener;
            std::vector<std::unique_ptr<protocol::Session>> _session_list;
            bool _shutdown;

    };


}