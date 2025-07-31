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

#include <client/file_info.hpp>
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
             *  \brief Retrieves opened descriptors ID's
             *
             *  \return Vector containing opened descriptors ID's
             */
            std::vector<protocol::descriptor_t> get_descriptors() const;

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

            /*!
             *  \brief Changes the current working directory of the session.
             * 
             *  \param path Path to change into.
             *  \return The new absolute working directory path after change.
             */
            virtual_path_t cd( const virtual_path_t& path ) const;

            /*!
             *  \brief Retrieves the current working directory of the session.
             * 
             *  \param use_cache If true, allows using a cached value; otherwise forces a live query.
             *  \return Absolute path of the current working directory.
             */
            virtual_path_t pwd( bool use_cache = true ) const;

            /*!
             *  \brief Retrieves the user's home directory path in the remote session.
             * 
             *  \param use_cache If true, allows using a cached value; otherwise forces a live query.
             *  \return Absolute path to the user's home directory.
             */
            virtual_path_t home( bool use_cache = true ) const;

            /*!
             *  \brief Lists the contents of a directory on the remote server.
             *
             *  This command queries the server for the contents of the specified directory path
             *  and returns a list of entries with metadata such as path, file type and size.
             *
             *  \param path The directory path to list. If omitted, the current working directory is used.
             *  \return A vector of \ref utils::FileInfo objects representing the contents of the directory.
             */
            std::vector<utils::FileInfo> ls( const virtual_path_t& path = "." ) const;

            /*!
             *  \brief Tries to open remote descriptor and retrieves opened descriptor ID on success.
             * 
             *  \param path Virtual path of file.
             *  \param mode Open mode.
             *  \return Currently opened descriptor ID.
             */
            protocol::descriptor_t open_descriptor( const virtual_path_t& path, std::ios::openmode mode ) const;

            /*!
             *  \brief Closes descriptor.
             * 
             *  \param path Descriptor ID.
             */
            void close_descriptor( protocol::descriptor_t descriptor ) const;

            /*!
             *  \brief Writes a block of data to a remote file descriptor.
             *
             *  Sends the specified buffer to the server for writing into an open remote file.
             *  Automatically performs CRC integrity check on the transmitted payload and updates
             *  the stream status flags (fail/eof) for the given descriptor.
             *
             *  \param descriptor Remote file descriptor ID to write to.
             *  \param data Pointer to the buffer containing data to write.
             *  \param size Size of the buffer in bytes.
             *  \return Number of bytes successfully written.
             *
             *  \throws std::runtime_error if the descriptor is invalid or the stream write fails.
             */
            dword_t write( protocol::descriptor_t descriptor, const void* data, dword_t size ) const;

            /*!
             *  \brief Reads a block of data from a remote file descriptor.
             *
             *  Requests the server to read up to the specified number of bytes from an open remote file.
             *  Performs CRC integrity verification on the received payload and updates
             *  the stream status flags (fail/eof) for the given descriptor.
             *
             *  \param descriptor Remote file descriptor ID to read from.
             *  \param data Pointer to the destination buffer where the data will be stored.
             *  \param size Maximum number of bytes to read.
             *  \return Number of bytes actually read and stored into the buffer.
             *
             *  \throws std::runtime_error if the descriptor is invalid or the stream read fails.
             */
            dword_t read( protocol::descriptor_t descriptor, void* data, dword_t size ) const;

            /*!
             *  \brief Retrieves the current I/O state flags for a remote file descriptor.
             *
             *  Returns the last known stream status for the specified remote descriptor as
             *  a pair of boolean values corresponding to the fail and eof flags.
             *
             *  \param descriptor Remote file descriptor ID to query.
             *  \return std::pair where first = fail flag, second = eof flag.
             */
            std::pair<bool, bool> iostate( protocol::descriptor_t descriptor ) const;


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

            mutable struct {

                path_t cwd, home;

                std::map<protocol::descriptor_t, std::pair<bool, bool>> descriptors_flags;

            } _cache;

    };


}