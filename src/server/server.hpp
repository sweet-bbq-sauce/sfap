/*!
 *  \file
 *  \brief Header file containing `Server` class declaration.
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


#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <unordered_map>


#include <net/address/address.hpp>
#include <net/listener/listener.hpp>
#include <protocol/protocol.hpp>
#include <server/command_registry/command_registry.hpp>
#include <server/session/session.hpp>
#include <utils/credentials.hpp>


namespace sfap {


    /*!
     *  \class Server
     *  \brief SFAP protocol server class handling connections and sessions.
     *
     *  The Server class manages incoming client connections, sessions lifecycle,
     *  authentication, command dispatching, and maintains server-wide information.
     */
    class Server {

        public:

            friend class protocol::Session;

            /*!
             *  \brief Command middleware function signature.
             *
             *  This function is called to process commands that require server-side handling.
             *
             *  \param id Command ID.
             *  \param user Optional username associated with the session.
             *  \return Result code of command processing.
             */
            using CommandMiddleware = std::function<protocol::CommandResult( word_t id, const std::optional<std::string>& user )>;

            /*!
             *  \brief Authentication middleware function signature.
             *
             *  This function is called to authenticate users.
             *
             *  \param credentials Credentials object containing username and password.
             *  \param user Reference to string where authenticated username is stored on success.
             *  \return Result code of authentication.
             */
            using AuthMiddleware = std::function<protocol::AuthResult( const utils::Credentials& credentials, std::string& user, path_t& root, std::optional<path_t>& home )>;

            /*!
             *  \brief Constructs a Server instance bound to the specified address.
             *  \param address Network address to listen on.
             *  \param command_registry Reference to command registry (default: vanilla commands).
             */
            Server( const net::Address& address, const protocol::CommandRegistry& command_registry = protocol::vanilla_commands );
            
            /*!
             *  \brief Destroys the Server, closes all sessions and stops listening.
             */
            ~Server();

            /*!
             *  \brief Sets the server's information table.
             *  \param info_table Server information key-value pairs.
             */
            void set_info_table( const protocol::ServerInfoTable& info_table );

            /*!
             *  \brief Sets the middleware handler for command processing.
             *  \param command_middleware Callable to handle commands.
             */
            void set_command_middleware( const CommandMiddleware& command_middleware );

            /*!
             *  \brief Sets the middleware handler for user authentication.
             *  \param auth_middleware Callable to handle authentication.
             */
            void set_auth_middleware( const AuthMiddleware& auth_middleware );

            /*!
             *  \brief Stops accepting new incoming connections.
             */
            void stop_accepting();

            /*!
             *  \brief Closes the server, disconnecting all clients and stopping all threads.
             */
            void close();

            /*!
             *  \brief Waits for all internal threads to finish execution.
             */
            void hang_on();

            /*!
             *  \brief Returns the current number of active sessions.
             *  \return Number of active sessions.
             */
            std::size_t get_session_count() const noexcept;

            /*!
             *  \brief Returns the count of sessions that have finished.
             *  \return Number of finished sessions.
             */
            std::size_t get_finished_session_count() const noexcept;

            /*!
             *  \brief Returns the number of active sessions for a specific user.
             *  \param user Username to query.
             *  \return Number of active sessions for given user.
             */
            std::size_t get_users_session_count( const std::string& user ) const noexcept;

            /*!
             *  \brief Sets a limit on concurrent sessions for a given user.
             *  \param user Username to set limit for.
             *  \param limit Optional limit on maximum sessions (no limit if std::nullopt).
             */
            void set_user_sessions_limit( const std::string& user, std::optional<std::size_t> limit );

            /*!
             *  \brief Checks if the server is currently open and accepting connections.
             *  \return True if server is open, false otherwise.
             */
            bool is_open() const noexcept;

            /*!
             *  \brief Returns the total number of sessions (active + finished).
             *  \return Total sessions count.
             */
            std::size_t get_all_sessions_count() const noexcept;

            static const AuthMiddleware default_authorize_middleware;       ///< Default authorization middleware.
            static const CommandMiddleware default_command_middleware;      ///< Default command middleware.


        private:

            std::atomic<bool> _running;                     ///< Indicates if the server is running.
            mutable std::atomic<std::size_t> _finished_sessions;    ///< Count of finished sessions.

            net::Listener _listener;        ///< Network listener object.
            std::thread _listener_thread;   ///< Thread running the listener.

            std::thread _cleaner_thread;            ///< Thread responsible for cleaning up finished sessions.
            std::condition_variable _cleaner_cv;    ///< Condition variable used by cleaner thread.
            mutable std::mutex _cleaner_mutex;      ///< Mutex for cleaner condition variable.

            std::unordered_map<protocol::session_id_t, std::unique_ptr<protocol::Session>> _sessions;   ///< Active sessions map.
            mutable std::shared_mutex _sessions_mutex;      ///< Mutex protecting _sessions.

            const protocol::CommandRegistry& _command_registry;     ///< Reference to command registry.

            CommandMiddleware _command_middleware;      ///< Command middleware handler.
            AuthMiddleware _auth_middleware;            ///< Authentication middleware handler.

            protocol::ServerInfoTable _info_table;          ///< Server information table.
            mutable std::shared_mutex _info_table_mutex;    ///< Mutex protecting _info_table.

            std::unordered_map<std::string, std::size_t> _users_limit;      ///< Map of per-user session limits.
            mutable std::shared_mutex _users_limit_mutex;                   ///< Mutex protecting _users_limit.

    };


}