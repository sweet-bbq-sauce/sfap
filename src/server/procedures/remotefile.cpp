#include <fstream>
#include <memory>

#include <server/procedures/procedures.hpp>
#include <server/session/session.hpp>
#include <net/iosocket/iosocket.hpp>
#include <server/command/command.hpp>
#include <server/server.hpp>


using namespace sfap;
using namespace sfap::net;
using namespace sfap::protocol;


void Procedures::_file_io_group() {


    Commands::add_command( CommandList::OPEN, "open", []( Session& session, const IOSocket& sock ) {

        path_t wanted = sock.recvss();
        std::ios::openmode mode = static_cast<std::ios::openmode>( sock.recvo<dword_t>() );

        try {

            wanted = session.serverify( wanted );

        }
        catch ( const std::exception& e ) {

            sock.sendo( FilesystemResult::NOT_ACCESSIBLE );

            return;

        }

        if ( std::filesystem::exists( wanted ) && !std::filesystem::is_regular_file( wanted ) ) {

            sock.sendo( FilesystemResult::IS_NOT_FILE );

            return;

        }

        std::fstream file( wanted, mode );

        if ( !file ) {

            sock.sendo( FilesystemResult::OTHER );

            return;

        }

        const remotefile_t id = session.create_file_id();
        const bool eof = file.eof();
        const bool good = file.good();

        session.files[id] = std::move( file );

        sock.sendo( FilesystemResult::OK );
        sock.sendo( id );
        sock.sendo( eof );
        sock.sendo( good );

    });


    Commands::add_command( CommandList::CLOSE, "close", []( Session& session, const IOSocket& sock ) {

        const remotefile_t wanted = sock.recvo<remotefile_t>();

        if ( session.files.find( wanted ) == session.files.end() ) sock.sendo( FileIOResult::ID_NOT_EXISTS );
        else {

            sock.sendo( FileIOResult::OK );

            session.files[wanted].close();
            session.files.erase( wanted );

        }

    });


    Commands::add_command( CommandList::IS_EOF, "eof", []( Session& session, const IOSocket& sock ) {

        const remotefile_t wanted = sock.recvo<remotefile_t>();

        if ( session.files.find( wanted ) == session.files.end() ) sock.sendo( FileIOResult::ID_NOT_EXISTS );
        else {

            sock.sendo( FileIOResult::OK );

            sock.sendo( session.files[wanted].eof() );

        }

    });


    Commands::add_command( CommandList::GOOD, "good", []( Session& session, const IOSocket& sock ) {

        const remotefile_t wanted = sock.recvo<remotefile_t>();

        if ( session.files.find( wanted ) == session.files.end() ) sock.sendo( FileIOResult::ID_NOT_EXISTS );
        else {

            sock.sendo( FileIOResult::OK );

            sock.sendo( session.files[wanted].good() );

        }

    });


    Commands::add_command( CommandList::GCOUNT, "gcount", []( Session& session, const IOSocket& sock ) {

        const remotefile_t wanted = sock.recvo<remotefile_t>();

        if ( session.files.find( wanted ) == session.files.end() ) sock.sendo( FileIOResult::ID_NOT_EXISTS );
        else {

            sock.sendo( FileIOResult::OK );

            sock.sendo( session.files[wanted].gcount() );

        }

    });


    Commands::add_command( CommandList::READ, "read", []( Session& session, const IOSocket& sock ) {

        const remotefile_t wanted = sock.recvo<remotefile_t>();

        if ( session.files.find( wanted ) == session.files.end() ) sock.sendo( FileIOResult::ID_NOT_EXISTS );
        else {

            sock.sendo( FileIOResult::OK );

            auto& file = session.files[wanted];

            const qword_t to_read = sock.recvo<qword_t>();
            const auto buffer = std::make_unique<char[]>( to_read );

            session.files[wanted].read( buffer.get(), to_read );

            sock.sendo( static_cast<qword_t>( file.gcount() ) );
            sock.sendo( static_cast<bool>( file.eof() ) );
            sock.sendo( static_cast<bool>( file.good() ) );

            sock.send( buffer.get(), file.gcount() );

        }

    });


    Commands::add_command( CommandList::WRITE, "write", []( Session& session, const IOSocket& sock ) {

        const remotefile_t wanted = sock.recvo<remotefile_t>();

        if ( session.files.find( wanted ) == session.files.end() ) sock.sendo( FileIOResult::ID_NOT_EXISTS );
        else {

            sock.sendo( FileIOResult::OK );

            auto& file = session.files[wanted];

            const qword_t to_write = sock.recvo<qword_t>();
            const auto buffer = std::make_unique<char[]>( to_write );

            sock.recv( buffer.get(), to_write );
            file.write( buffer.get(), to_write );

            sock.sendo( file.eof() );
            sock.sendo( file.good() );

        }

    });


    Commands::add_command( CommandList::TELLG, "tellg", []( Session& session, const IOSocket& sock ) {

        const remotefile_t wanted = sock.recvo<remotefile_t>();

        if ( session.files.find( wanted ) == session.files.end() ) sock.sendo( FileIOResult::ID_NOT_EXISTS );
        else {

            sock.sendo( FileIOResult::OK );

            auto& file = session.files[wanted];

            sock.sendo( static_cast<int64_t>( file.tellg() ) );

        }

    });


    Commands::add_command( CommandList::SEEKG, "seekg", []( Session& session, const IOSocket& sock ) {

        const remotefile_t wanted = sock.recvo<remotefile_t>();

        if ( session.files.find( wanted ) == session.files.end() ) sock.sendo( FileIOResult::ID_NOT_EXISTS );
        else {

            sock.sendo( FileIOResult::OK );

            auto& file = session.files[wanted];

            if ( sock.recvo<bool>() ) file.seekg( static_cast<std::streamoff>( sock.recvo<int64_t>() ), static_cast<std::ios::seekdir>( sock.recvo<dword_t>() ) );
            else file.seekg( static_cast<std::streampos>( sock.recvo<int64_t>() ) );

        }

    });


    Commands::add_command( CommandList::TELLP, "tellp", []( Session& session, const IOSocket& sock ) {

        const remotefile_t wanted = sock.recvo<remotefile_t>();

        if ( session.files.find( wanted ) == session.files.end() ) sock.sendo( FileIOResult::ID_NOT_EXISTS );
        else {

            sock.sendo( FileIOResult::OK );

            auto& file = session.files[wanted];

            sock.sendo( static_cast<int64_t>( file.tellp() ) );

        }

    });


    Commands::add_command( CommandList::SEEKP, "seekp", []( Session& session, const IOSocket& sock ) {

        const remotefile_t wanted = sock.recvo<remotefile_t>();

        if ( session.files.find( wanted ) == session.files.end() ) sock.sendo( FileIOResult::ID_NOT_EXISTS );
        else {

            sock.sendo( FileIOResult::OK );

            auto& file = session.files[wanted];

            if ( sock.recvo<bool>() ) file.seekp( static_cast<std::streamoff>( sock.recvo<int64_t>() ), static_cast<std::ios::seekdir>( sock.recvo<dword_t>() ) );
            else file.seekp( static_cast<std::streampos>( sock.recvo<int64_t>() ) );

        }

    });


}