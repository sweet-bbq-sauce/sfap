/*!
 *  \file
 *  \brief Header file containing `CommandRegistry` class declaration.
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


#include <functional>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <map>

#include <sfap.hpp>
#include <protocol/protocol.hpp>


namespace sfap {

    namespace net {

        class IOSocket;

    }

    namespace protocol {


        class Session;

        /*!
         *  \brief Registry class for SFAP protocol commands.
         *
         *  Manages a thread-safe registry of commands identified by an ID and name,
         *  each associated with a callable procedure to execute the command.
         */
        class CommandRegistry {

            public:

                /*!
                 *  \brief Type alias for command handler functions.
                 *
                 *  The command handler receives a reference to the current Session
                 *  and the connected IOSocket.
                 */
                using CommandProcedure = std::function<void( Session& session, const net::IOSocket& socket )>;

                /*!
                 *  \brief Default constructor.
                 */
                CommandRegistry() noexcept;

                /*!
                 *  \brief Copy constructor.
                 *
                 *  Performs a thread-safe copy of another CommandRegistry.
                 *  \param other Another CommandRegistry instance to copy from.
                 */
                CommandRegistry( const CommandRegistry& other ) noexcept;

                /*!
                 *  \brief Copy assignment operator.
                 *
                 *  Performs a thread-safe assignment from another CommandRegistry.
                 *  \param other Another CommandRegistry instance to assign from.
                 *  \return Reference to this instance.
                 */
                CommandRegistry& operator=( const CommandRegistry& other ) noexcept;

                /*!
                 *  \brief Add a new command to the registry.
                 *
                 *  \param id Unique numeric ID of the command.
                 *  \param name Unique string name of the command.
                 *  \param procedure Callable command handler.
                 *
                 *  \throws std::runtime_error if command with given ID or name already exists.
                 */
                void add( word_t id, const std::string& name, CommandProcedure procedure );

                /*!
                 *  \brief Add a new command using protocol::Command enum as ID.
                 *
                 *  \param id Command enum value.
                 *  \param name Unique string name of the command.
                 *  \param procedure Callable command handler.
                 *
                 *  \throws std::runtime_error if command with given ID or name already exists.
                 */
                void add( protocol::Command id, const std::string& name, CommandProcedure procedure );

                /*!
                 *  \brief Add all commands from another registry into this one.
                 *
                 *  \param other Another CommandRegistry to merge.
                 */
                void add( const CommandRegistry& other );

                /*!
                 *  \brief Remove a command by its numeric ID.
                 *
                 *  If the ID does not exist, the method does nothing.
                 *
                 *  \param id Command ID to remove.
                 */
                void remove( word_t id );

                /*!
                 *  \brief Remove a command by its name.
                 *
                 *  \param name Name of the command to remove.
                 *
                 *  \throws std::runtime_error if command with the given name does not exist.
                 */
                void remove( const std::string& name );

                /*!
                 *  \brief Remove a command by its protocol::Command enum ID.
                 *
                 *  \param id Command enum value to remove.
                 */
                void remove( protocol::Command id );

                /*!
                 *  \brief Get the number of registered commands.
                 *
                 *  \return Number of commands currently registered.
                 */
                word_t size() const noexcept;

                /*!
                 *  \brief Get a snapshot of command IDs and names.
                 *
                 *  Returns a map of command IDs to their corresponding names.
                 *
                 *  \return ServerCommandsTable mapping IDs to names.
                 */
                ServerCommandsTable get_command_list() const noexcept;

                /*!
                 *  \brief Check if the registry is empty.
                 *
                 *  \return true if no commands are registered; false otherwise.
                 */
                bool empty() const noexcept;

                /*!
                 *  \brief Get command ID by its name.
                 *
                 *  \param name Name of the command.
                 *  \return Command ID associated with the name.
                 *
                 *  \throws std::runtime_error if command with the given name does not exist.
                 */
                word_t get_by_name( const std::string& name ) const;

                /*!
                 *  \brief Check if a command exists by its numeric ID.
                 *
                 *  \param id Command ID to check.
                 *  \return true if command exists; false otherwise.
                 */
                bool exists( word_t id ) const;

                /*!
                 *  \brief Check if a command exists by its name.
                 *
                 *  \param name Command name to check.
                 *  \return true if command exists; false otherwise.
                 */
                bool exists( const std::string& name ) const;

                /*!
                 *  \brief Access command procedure by numeric ID.
                 *
                 *  \param id Command ID.
                 *  \return Reference to the command procedure.
                 *
                 *  \throws std::invalid_argument if command with the given ID does not exist.
                 */
                const CommandProcedure& operator[]( word_t id ) const;

                /*!
                 *  \brief Access command procedure by protocol::Command enum ID.
                 *
                 *  \param id Command enum value.
                 *  \return Reference to the command procedure.
                 *
                 *  \throws std::invalid_argument if command with the given ID does not exist.
                 */
                const CommandProcedure& operator[]( protocol::Command id ) const;

                /*!
                 *  \brief Access command procedure by command name.
                 *
                 *  \param name Command name.
                 *  \return Reference to the command procedure.
                 *
                 *  \throws std::runtime_error if command with the given name does not exist.
                 */
                const CommandProcedure& operator[]( const std::string& name ) const;


            private:

                std::map<word_t, std::pair<std::string, CommandProcedure>> _registry;   ///< Map from command ID to pair(name, procedure).
                mutable std::shared_mutex _registry_mutex;      ///< Mutex protecting _registry.
        };


    }

}