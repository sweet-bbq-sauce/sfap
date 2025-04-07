#pragma once


#include <thread>
#include <utility>
#include <unordered_map>
#include <fstream>

#include <net/iosocket/iosocket.hpp>
#include <protocol/protocol.hpp>


namespace sfap {

    class Server;

    namespace protocol {


        class Session {

            public:

                friend class Commands;

                Session( net::IOSocket&& sock, Server& parent );

                Session( const Session& other ) = delete;
                Session& operator=( const Session& other ) = delete;

                bool logged;
                std::string username;

                path_t root_directory, home_directory, cwd_directory;

                Server& parent;

                void shutdown() noexcept;

                bool accessible( const path_t& target ) const;
                path_t serverify( const path_t& path ) const;
                path_t userify( const path_t& path ) const;

                std::unordered_map<dword_t, std::fstream> files;

                dword_t create_file_id() {

                    return _last_file_id++;

                }


            private:

                std::thread _session_thread;

                void _session_loop();

                dword_t _last_file_id;
                bool _shutdown;

                net::IOSocket _sock;

                // User info
                

        };


    }

}