/*!
 *  \file
 *  \brief Header file containing `Client` class declaration.
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
#include <string>

#include <net/address/address.hpp>
#include <net/iosocket/iosocket.hpp>
#include <protocol/protocol.hpp>
#include <utils/credentials.hpp>


namespace sfap {

    
    /*!
     *  \brief Client class for connecting and interacting with SFAP server.
     *
     *  Provides methods to connect to the server, authenticate,
     *  send commands, retrieve server info, and manage client state.
     */
    class Client {

        public:

            /*!
             *  \brief Constructs a Client object and connects to given address.
             *
             *  Optionally attempts authorization if credentials are provided.
             *
             *  \param address Server address to connect.
             *  \param credentials Optional shared pointer to credentials for authorization.
             */
            Client( const net::Address& address, std::shared_ptr<const utils::Credentials> credentials = nullptr );

            /*!
             *  \brief Destructor.
             *
             *  Closes the connection if still open.
             */
            ~Client();

            /*!
             *  \brief Sends a NOOP (no operation) command to the server.
             *
             *  Can be used to keep the connection alive or test server responsiveness.
             */
            void noop() const;
            
            /*!
             *  @brief Closes the client connection to the server.
             */
            void close();
            
            /*!
             *  \brief Authorizes the client using provided credentials.
             *
             *  Sends authentication data to the server and waits for a response.
             *
             *  \param credentials Shared pointer to credentials object.
             *  \return Authentication result enum indicating success or failure reason.
             *
             *  \throws std::invalid_argument if credentials is nullptr.
             */
            protocol::AuthResult authorize( std::shared_ptr<const utils::Credentials> credentials );

            /*!
             *  \brief Clears the client authorization state.
             *
             *  Resets session on the server and clears local cache.
             */
            void clear();

            /*!
             *  \brief Gets the authorized username if available.
             *
             *  \return Optional string containing username if authorized, std::nullopt otherwise.
             */
            const std::optional<std::string>& get_username() const noexcept;

            /*!
             *  \brief Retrieves the list of commands supported by the server.
             *
             *  \return ServerCommandsTable mapping command IDs to command names.
             */
            protocol::ServerCommandsTable get_server_commands() const;

            /*!
             *  \brief Retrieves server information key-value pairs.
             *
             *  \return ServerInfoTable containing server info entries.
             */
            protocol::ServerInfoTable get_server_info() const;

            /*!
             *  \brief Creates a copy of the current client.
             *
             *  The new client maintains the same address and credentials but opens a new connection.
             *
             *  \return Cloned Client instance.
             */
            Client clone() const;

            /*!
             *  \brief Checks if the client connection is open.
             *
             *  \return true if connected; false otherwise.
             */
            bool is_opened() const noexcept;

            /*!
             *  \brief Boolean conversion operator.
             *
             *  Allows using the Client instance in boolean context to check connection status.
             *
             *  \return true if connected; false otherwise.
             */
            explicit operator bool() const noexcept;

            /*!
             *  \brief Checks if the client is authorized.
             *
             *  \return true if authorized; false otherwise.
             */
            bool is_authorized() const noexcept;

            /*!
             *  \brief Checks if the connection is secure.
             *
             *  \return true if the underlying socket uses secure connection; false otherwise.
             */
            bool is_secure() const noexcept;


        private:

            /*!
             *  \brief Sends a command ID to the server and waits for the command result.
             *
             *  Throws on error results.
             *
             *  \param command Command ID as word_t.
             *
             *  \throws std::logic_error if the client is closed.
             *  \throws std::runtime_error for command errors returned by the server.
             */
            void _request_command( word_t command ) const;

            /*!
             *  \brief Overload that takes a protocol::Command enum.
             *
             *  \param command Command enum value.
             */
            void _request_command( protocol::Command command ) const;

            std::optional<std::string> _username;   ///< Currently authorized username, if any.

            net::Address _address;      ///< Server address.
            std::shared_ptr<const utils::Credentials> _credentials;     ///< Credentials used for authorization.

            net::IOSocket _socket;      ///< Socket used for communication with the server.

    };


}