#include <fstream>

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

            sock.sendo( FileIOResult::CAN_NOT_OPEN_FILE );

            return;

        }

        if ( std::filesystem::exists( wanted ) && std::filesystem::is_character_file( wanted ) ) {

            sock.sendo( FileIOResult::CAN_NOT_OPEN_FILE );

            return;

        }

        std::fstream file( wanted, mode );

        if ( !file ) {

            sock.sendo( FileIOResult::CAN_NOT_OPEN_FILE );

            return;

        }

        const remotefile_t id = session.create_file_id();
        const bool eof = file.eof();
        const bool good = file.good();

        session.files[id] = std::move( file );

        sock.sendo( FileIOResult::OK );
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


}