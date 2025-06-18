/*!
 *  \file
 *  \brief Header file with SFAP protocol specification and structures.
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
#include <unordered_map>
#include <set>
#include <vector>

#include <sfap.hpp>


namespace sfap {

    namespace protocol {


        constexpr dword_t sync_watchdog = 0x53464150;   ///< Magic value `"SFAP"` used as a prefix for every command to detect desynchronization.

        using descriptor_t = dword_t;       ///< Represents descriptor.
        using session_id_t = dword_t;       ///< Respresents session ID.

        
        /*!
        *  \enum Command
        *  \brief Vanilla SFAP protocol commands.
        */
        enum class Command : word_t {

            // -------------- Connection Control --------------
            NONE = 0x00,    ///< Does nothing on the server, but initiates the standard command request flow. Can be used as a ping or keep-alive mechanism.
            BYE,            ///< Closes the connection.
            SERVER_INFO,    ///< Retrieves the server info table. \see ServerInfoTable
            COMMANDS,       ///< Retrieves the list of commands supported by the server.
            DESCRIPTORS,    ///< Retrieves the list of open descriptors.
            AUTH,           ///< On success, clears the current session and sets the session username to the provided value. Executes authenticating middleware (`AuthMiddleware`) on the server.
            CLEAR,          ///< Clears the current session: resets username to null and closes all opened descriptors.

            CD,
            PWD,
            HOME

        };


        /*!
         *  \brief Allowed always commands.
         *
         *  These commands MUST be executed ALWAYS regardless of the session status. This is what the protocol requires.
         */
        static const std::set<Command> allowed_always = {

            Command::NONE,
            Command::BYE,
            Command::SERVER_INFO,
            Command::COMMANDS,
            Command::DESCRIPTORS,
            Command::AUTH,
            Command::CLEAR

        };


        /*!
         *  \enum CommandResult
         *  \brief Value returned from server after command authorization.
         */
        enum class CommandResult : byte_t {

            OK,                 ///< Command is accepted and will be executed.
            ACCESS_DENIED,      ///< Command is denied by the server's access control list (ACL).
            UNAVAILABLE,        ///< Command is temporarily unavailable (possibly due to server overload).
            DISABLED,           ///< Command is permanently disabled by server configuration.
            UNSUPPORTED,        ///< Command is not supported on this server (possibly due to limited build or missing dependencies).
            MIDDLEWARE_ERROR,   ///< An exception occurred in the command's middleware layer.
            UNKNOWN             ///< Command does not exist in the server's command registry (possibly an invalid command ID).

        };


        /*!
         *  \typedef ServerInfoTable
         *  \brief Key-value table with server metadata (e.g., server version, extensions).
         */
        using ServerInfoTable = std::unordered_map<std::string, std::string>;


        /*!
         *  \typedef ServerCommandsTable
         *  \brief List of commands supported by the server with their names.
         */
        using ServerCommandsTable = std::unordered_map<word_t, std::string>;


        /*!
         *  \enum AuthResult
         *  \brief Value returned from server after user authorization.
         * 
         *  \note The values `UNKNOWN_USER`, `WRONG_PASSWORD`, and `USER_DISABLED` are returned
         *        only by the server's authentication middleware (`AuthMiddleware`) implementation.
         *        This library does not implement authentication itself,
         *        as login mechanisms are often handled externally.
         */
        enum class AuthResult : byte_t {

            OK,                     ///< New session will be created.
            UNKNOWN_USER,           ///< The specified username is not found in the user registry.
            WRONG_PASSWORD,         ///< The username is valid, but the password is incorrect.
            USER_DISABLED,          ///< The username and password are correct, but the user account is disabled.
            USER_LIMIT_REACHED,     ///< Too many active sessions are already logged in under this user.
            MIDDLEWARE_ERROR        ///< An exception occurred in the authentication middleware.

        };


        /*!
         *  \enum AccessResult
         *  \brief Represents the result of an access-related operation in the virtual filesystem.
         */
        enum class AccessResult : byte_t {

            OK,     ///< The operation was successful.
            ACCESS_DENIED,      ///< Access to the requested path was denied (e.g., invalid path or not allowed).
            OUTSIDE_ROOT,       ///< The requested path is outside the allowed virtual filesystem root.
            IS_NOT_DIRECTORY,   

        };


        class CommandRegistry;
        extern const CommandRegistry vanilla_commands;      ///< Static SFAP vanilla commands readonly registry.


    }

}