#pragma once


#include <sfap.hpp>


namespace sfap {

    namespace protocol {


        // 4 bytes that are sent by the client directly before the command code.
        // Checking this by the server prevents the server from accidentally
        // executing the command if the client sends unexpected data in the event of a desynchronization.
        const dword_t precommand_header = 0x53464150;

        // Username for already connected (not logged) user
        const std::string default_username = "anonymous";


        enum class CommandList : word_t {

            NONE = 0x00,    // Empty command, does nothing
            BYE,            // Close connection
            INFO,           // Get server info

            // User info
            WHOAMI = 0x10,  // Get username (or "null" if not logged)
            HOME,           // Get user home directory
            LOGIN,          // Try to login
            LOGOUT,         // Logout, sets `logged` flag to false and username to "null"
            IS_LOGGED,      // Sprawdź czy użytkownik jest zalogowany

            // Filesystem info
            PWD = 0x20,     // Get current directory
            CD,             // Change current directory
            LS,             // List directory
            FIND,           // Find entry
            STAT,           // Get entry stat info
            MAGIC,          // Get extended entry info from libMagic if supported
            EXISTS,         // Sprawdź czy obiekt istnieje
            SPACE,          // Get disk space info

            // Filesystem manipulation
            TOUCH = 0x30,   // Create file
            MKDIR,          // Create directory
            RM,             // Remove file
            RMDIR,          // Remove directory
            MOVE,           // Move entry
            COPY,           // Copy entry
            CHMOD,          // Change entry privileges
            SYMLINK,        // Create symbolic link

            // FTP-like transfer
            PUT = 0x40,     // Upload file to server
            GET,            // Download file from server

            // File stream manipulation
            OPEN = 0x50,    // Open remote file for real-time operations
            CLOSE,          // Close remote file
            SEEKG,          // Set the reading pointer
            TELLG,          // Get the reading pointer
            SEEKP,          // Set the writing pointer
            TELLP,          // Get the writing pointer
            WRITE,          // Write to remote file
            READ,           // Read from remote file
            IS_EOF,         // Get remote file EOF,
            GOOD,           // Check if file is good
            GCOUNT          // Get last bytes read

            // Admin tools
            // soon ...

        };


        enum class CommandResult : byte_t {

            OK,                 // Ok, running command procedure
            FORBIDDEN,          // Command is forbidden for this user (for example when not logged)
            DISABLED,           // Command is disabled by server config
            NOT_SUPPORTED,      // Command is not supported by server
            COMMAND_NOT_EXISTS  // Command ID is not valid

        };


        enum class LoginResult : byte_t {

            OK,                 // Ok, 'logged' flag in session is set to true
            DISABLED,           // User is disabled
            USER_NOT_EXISTS,    // User does not exist
            WRONG_PASSWORD,     // User exists but wrong password
            NEED_PASSWORD,      // Can not login with empty password
            TOO_MANY_USER_INSTANCES     // User has reached the maximum allowed sessions

        };


        enum class ConnectionResult : byte_t {

            OK,
            REJECTED,
            SERVER_BUSY

        };


        enum class FilesystemResult : byte_t {

            OK,
            IS_NOT_DIRECTORY,
            IS_NOT_FILE,
            NOT_ACCESSIBLE,
            NOT_EXISTS,
            NOT_SUPPORTED,
            OTHER

        };


        enum class FileIOResult : byte_t {

            OK,
            CAN_NOT_OPEN_FILE,
            IS_NOT_FILE,
            FILE_NOT_GOOD,
            END_OF_FILE,
            ID_NOT_EXISTS

        };


    }

}