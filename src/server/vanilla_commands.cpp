/*!
 *  \file
 *  \brief Source file containing SFAP vanilla commands procedures.
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


#include <algorithm>
#include <atomic>
#include <chrono>
#include <list>
#include <stdexcept>
#include <thread>

#include <client/file_info.hpp>
#include <net/address/address.hpp>
#include <net/listener/listener.hpp>
#include <server/command_registry/command_registry.hpp>
#include <server/server.hpp>
#include <server/session/session.hpp>
#include <utils/crc.hpp>
#include <utils/encoding.hpp>


using namespace sfap;
using namespace sfap::protocol;


const CommandRegistry protocol::vanilla_commands = ( []() -> CommandRegistry {


    CommandRegistry buffer;


    buffer.add( Command::NONE, "none", nullptr );
    buffer.add( Command::BYE, "bye", nullptr );
    buffer.add( Command::SERVER_INFO, "server_info", nullptr );
    buffer.add( Command::COMMANDS, "commands", nullptr );
    buffer.add( Command::DESCRIPTORS, "descriptors", nullptr );
    buffer.add( Command::AUTH, "auth", nullptr );
    buffer.add( Command::CLEAR, "clear", nullptr );


    buffer.add( Command::CD, "cd", []( Session& session, const net::IOSocket& socket ) {

        auto& filesystem = session.get_filesystem();

        const path_t path = socket.recvp();

        if ( std::filesystem::is_directory( filesystem.to_system( path ) ) ) {

            const auto result = filesystem.cd( path );

            socket.sende( result );

            if ( result == AccessResult::OK ) {

                socket.sendp( filesystem.pwd() );

            }

        }
        else {

            socket.sende( AccessResult::IS_NOT_DIRECTORY );

        }

    });


    buffer.add( Command::PWD, "pwd", []( Session& session, const net::IOSocket& socket ) {

        socket.sendp( session.get_filesystem().pwd() );

    });


    buffer.add( Command::HOME, "home", []( Session& session, const net::IOSocket& socket ) {

        socket.sendp( session.get_filesystem().get_home() );

    });


    buffer.add( Command::LS, "ls", []( Session& session, const net::IOSocket& socket ) {

        auto& filesystem = session.get_filesystem();

        const auto path = socket.recvp();
        const auto result = filesystem.try_normalize( path );

        if ( !result ) {

            socket.sende( AccessResult::ACCESS_DENIED );

        }
        else if ( !std::filesystem::is_directory( filesystem.to_system( result.value() ) ) ) {

            socket.sende( AccessResult::IS_NOT_DIRECTORY );

        }
        else {

            std::vector<std::filesystem::directory_entry> entries;

            try {

                for ( const auto& entry : std::filesystem::directory_iterator( filesystem.to_system( result.value() ) ) ) {

                    entries.push_back( entry );

                }

            }
            catch ( const std::exception& e ) {

                std::cout << e.what() << std::endl;

                socket.sende( AccessResult::INTERNAL_ERROR );

                return;

            }

            socket.sende( AccessResult::OK );

            socket.sendo( static_cast<dword_t>( entries.size() ) );

            for ( const auto& entry : entries ) {

                // Send entry type.
                socket.sende( utils::FileInfo::convert_type_from_std( entry.status().type() ) );

                // Send path.
                socket.sendp( filesystem.to_virtual( entry.path() ) );

                // Send file size or 0 if is not regular file.
                socket.sendo( static_cast<qword_t>( entry.is_regular_file() ? entry.file_size() : 0 ) );

            }

        }

    });


    buffer.add( Command::OPEN, "open", []( Session& session, const net::IOSocket& socket ) {

        // Get session VFS.
        auto& filesystem = session.get_filesystem();

        // Receive requested path and openmode from client.
        const auto path = socket.recvp();
        const auto mode = static_cast<std::ios::openmode>( socket.recvo<dword_t>() );

        // Try to normalize requested path ...
        const auto result = filesystem.try_normalize( path );

        // ... and abort command when it fails.
        if ( !result ) {

            // Send `AccessResult::ACCESS_DENIED` signalizing file can't be accessed.
            socket.sende( AccessResult::ACCESS_DENIED );

            return;

        }

        // Open file with requested path.
        std::fstream file( filesystem.to_system( result.value() ), mode );

        // When file is not opened or stream is invalid abort command.
        if ( !file.is_open() || file.fail() ) {

            // Send `AccessResult::CANT_OPEN_FILE` signalizing file can't be opened.
            socket.sende( AccessResult::CANT_OPEN_FILE );

        }
        else {

            // Send `AccessResult::OK` signalizing file is opened.
            socket.sende( AccessResult::OK );

            // Register file in session and send it's descriptor to client.
            socket.sendo( session.add_descriptor( file ) );

            // Send stream status flags (FAIL and EOF).
            socket.sendb( file.fail() );
            socket.sendb( file.eof() );

        }

    });


    buffer.add( Command::CLOSE, "close", []( Session& session, const net::IOSocket& socket ) {

        // Receive requested descriptor and try to close it.
        session.close_descriptor( socket.recvo<descriptor_t>() );

    });


    buffer.add( Command::WRITE, "write", []( Session& session, const net::IOSocket& socket ) {

        // Receive file descriptor.
        const auto requested_descriptor = socket.recvo<descriptor_t>();
        
        // Check if received descriptor is actually a opened descriptor.
        const auto stream = session.get_stream( requested_descriptor );
        if ( !stream ) {

            // If not found return error and abort command procedure.
            socket.sende( AccessResult::BAD_DESCRIPTOR );
            return;

        }

        // If found send `AccessResult::OK` signalizing opened descriptor is valid opened descriptor.
        socket.sende( AccessResult::OK );

        // Now we will receive payload ...
        data_t payload;
        socket.recvd( payload );

        // ... and CRC of the payload.
        const auto client_crc = socket.recvo<crc_t>();

        // Check payload integration comparing CRC from client with actually received data from client.
        if ( utils::CRC::data( payload.data(), payload.size() ) != client_crc ) {

            // If CRC doesn't match, send I/O error and abort command procedure.
            socket.sende( FileStreamResult::CRC_MISSMATCH );
            return;

        }

        // If integration is OK try to write payload to descriptor's stream.
        stream.value().get().write( reinterpret_cast<const char*>( payload.data() ), payload.size() );

        // Flush data to stream.
        stream.value().get().flush();

        // Send success result.
        socket.sende( FileStreamResult::OK );

        // Send stream status flags (FAIL and EOF).
        socket.sendb( stream.value().get().fail() );
        socket.sendb( stream.value().get().eof() );

    });


    buffer.add( Command::READ, "read", []( Session& session, const net::IOSocket& socket ) {

        // Receive file descriptor.
        const auto requested_descriptor = socket.recvo<descriptor_t>();
        
        // Check if received descriptor is actually a opened descriptor.
        const auto stream = session.get_stream( requested_descriptor );
        if ( !stream ) {

            // If not found return error and abort command procedure.
            socket.sende( AccessResult::BAD_DESCRIPTOR );
            return;

        }

        // If found send `AccessResult::OK` signalizing opened descriptor is valid opened descriptor.
        socket.sende( AccessResult::OK );

        // Receive data size wanted by client to read from descriptor's stream.
        const auto requested_size = socket.recvo<dword_t>();

        // Allocate buffer for payload.
        data_t payload( requested_size );

        // Read from stream to payload buffer.
        stream.value().get().read( reinterpret_cast<char*>( payload.data() ), payload.size() );

        // Send OK status.
        socket.sende( FileStreamResult::OK );

        // Send actually read payload ...
        const auto actually_read = static_cast<dword_t>( stream.value().get().gcount() );
        socket.sendo( actually_read );
        socket.send( payload.data(), actually_read );

        // ... and it's CRC.
        socket.sendo( utils::CRC::data( payload.data(), actually_read ) );

        // Send stream status flags (FAIL and EOF).
        socket.sendb( stream.value().get().fail() );
        socket.sendb( stream.value().get().eof() );

    });


    buffer.add( Command::IOSTATE, "iostate", []( Session& session, const net::IOSocket& socket ) {

        // Receive requested descriptor from client.
        const auto descriptor = socket.recvo<descriptor_t>();

        // Try to get stream from descriptor.
        const auto result = session.get_stream( descriptor );

        if ( result ) {

            // If found, send `AccessResult::OK` ...
            socket.sende( AccessResult::OK );

            // ... and send stream state flags.
            const auto& stream = result.value().get();
            socket.sendb( stream.fail() );
            socket.sendb( stream.eof() );

        }
        else {

            // If not found send `AccessResult::BAD_DESCRIPTOR` and abort command.
            socket.sende( AccessResult::BAD_DESCRIPTOR );

        }

    });


    return buffer;

    
})();