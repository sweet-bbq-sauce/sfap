/*!
 *  \file
 *  \brief Header file containing `Session` class declaration.
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
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <shared_mutex>
#include <thread>

#include <net/iosocket/iosocket.hpp>
#include <protocol/protocol.hpp>
#include <server/virtual_filesystem/virtual_filesystem.hpp>


namespace sfap {


    class Server;


    namespace protocol {


        /*!
         *  \class Session
         *  \brief Represents a single client session within the SFAP protocol server.
         *
         *  The Session class handles communication with a connected client,
         *  manages session state, authentication, and open file descriptors.
         */
        class Session {

            public:

                /*!
                 *  \brief Grants Server access to private members of Session.
                 */
                friend class sfap::Server;

                /*!
                 *  \brief Constructs a new Session.
                 *  \param socket Connected IOSocket for communication with client.
                 *  \param parent Reference to the parent Server instance.
                 *  \param id Unique session identifier.
                 */
                Session( net::IOSocket& socket, Server& parent, session_id_t id );

                /*!
                 *  \brief Destroys the Session, closing resources and stopping the session thread.
                 */
                ~Session();

                /*!
                 *  \brief Closes the session.
                 *  \param clean Indicates whether to clean up resources (default true).
                 */
                void close( bool clean = true );


                /*!
                 *  \brief Retrieves the username associated with the session, if authorized.
                 *  \return Optional username string; std::nullopt if anonymous or unauthorized.
                 */
                std::optional<std::string> get_user() const noexcept;

                /*!
                 *  \brief Returns the set of open file descriptors for this session.
                 *  \return Set of open descriptor IDs.
                 */
                std::set<descriptor_t> get_descriptors() const noexcept;

                /*!
                 *  \brief Adds std::fstream to descriptors table and returns new descriptor ID.
                 *
                 *  \param stream Reference to `std::fstream`.
                 *  \return New descriptor ID.
                 */
                descriptor_t add_descriptor( std::fstream& stream ) noexcept;

                /*!
                 *  \brief Removes descriptor from table.
                 *
                 *  \param stream Descriptor ID to remove.
                 */
                void close_descriptor( descriptor_t descriptor ) noexcept;

                /*!
                 *  \brief Returns the reference to virtual filesystem object.
                 *  \return Mutable VirtualFilesystem reference.
                 * 
                 *  \throws std::logic_error when virtual filesystem is not initialized (e.g. before `AUTH` command).
                 */
                VirtualFilesystem& get_filesystem() const;


            private:

                const session_id_t _id;     ///< Unique session ID.
                net::IOSocket _socket;      ///< Socket connected to the client.
                std::thread _thread;        ///< Thread running the session's command loop.
                const Server& _parent;      ///< Reference to the parent server instance.
                std::atomic<bool> _finished;        ///< Flag indicating session has finished and will be cleaned up.

                std::unique_ptr<VirtualFilesystem> _filesystem;     ///< Virtual filesystem space.

                std::optional<std::string> _user;           ///< Username if authenticated; std::nullopt if anonymous.
                mutable std::shared_mutex _user_mutex;      ///< Mutex protecting access to _user.

                std::map<descriptor_t, std::fstream> _descriptors;      ///< Map of open file descriptors to their streams.
                mutable std::shared_mutex _descriptors_mutex;           ///< Mutex protecting access to _descriptors.
                descriptor_t _last_descriptor;      ///< Last used descriptor ID.

                /*!
                 *  \enum State
                 *  \brief Possible states of a Session.
                 */
                enum class State : uint8_t {

                    WAITING,    ///< Session is idle, waiting for a client command.
                    PROCESSING  ///< Session is currently processing a client command.

                };
                
                std::atomic<State> _state;      ///< Current state of the session.

                /*!
                 *  \brief Main loop processing client commands.
                 *
                 *  This function runs in the session thread.
                 */
                void _command_loop();

        };


    }

}