#include <memory>
#include <mutex>
#include <regex>

#include <server/procedures/procedures.hpp>
#include <server/session/session.hpp>
#include <net/iosocket/iosocket.hpp>
#include <server/command/command.hpp>
#include <server/server.hpp>


using namespace sfap;
using namespace sfap::net;
using namespace sfap::protocol;


EntryInfo scan_file( const std::filesystem::directory_entry& entry, bool stat, bool magic, const Server& parent ) {

    EntryInfo info;

    info.path = entry.path();
    info.stat = stat ? std::make_optional( StatInfo() ) : std::nullopt;

    if ( stat ) {

        info.stat->size = ( entry.is_regular_file() ) ? entry.file_size() : 0;
        info.stat->type = entry.status().type();

    }

    info.magic = ( magic && parent.magic ) ? std::make_optional( parent.magic->get().scan_file( entry ) ) : std::nullopt;

    return info;

}


void list_directory( const path_t& directory, std::vector<EntryInfo>& table, bool deep, bool hidden, bool stat, bool magic, const Server& parent, std::optional<std::string> pattern, bool end_at_first, bool& found ) {

    for ( const auto& entry : std::filesystem::directory_iterator( directory ) ) {

        if ( found && end_at_first ) break;

        if ( pattern ) {

            std::string regex_pattern;
            for ( const char c : pattern.value() ) {

                static const std::string palette( ".+()[]{}|^$\\" );

                if ( c == '?' ) regex_pattern += ".";
                else if ( c == '*' ) regex_pattern += ".*";
                else {
                    
                    if ( palette.find( c ) != std::string::npos ) regex_pattern += "\\";

                    regex_pattern += c;

                }
                
            }

            const std::regex re( regex_pattern, std::regex_constants::icase );

            if ( !std::regex_match( entry.path().filename().string(), re ) ) continue;
            else found = true;

        }

        if ( hidden && entry.path().filename().string().front() == '.' ) continue;

        table.push_back( scan_file( entry, stat, magic, parent ) );

        if ( deep && entry.is_directory() ) list_directory( entry, table, deep, hidden, stat, magic, parent, pattern, end_at_first, found );

    }

}


void Procedures::_fs_group() {


    Commands::add_command( CommandList::PWD, "pwd", []( Session& session, const IOSocket& sock ) {

        sock.sends( session.userify( session.cwd_directory ).string() );

    });


    Commands::add_command( CommandList::CD, "cd", []( Session& session, const IOSocket& sock ) {

        path_t wanted;

        try {

            wanted = session.serverify( sock.recvss() );

        }
        catch ( ... ) {

            sock.sendo( FilesystemResult::NOT_ACCESSIBLE );

            return;

        }

        if ( !std::filesystem::exists( wanted ) ) sock.sendo( FilesystemResult::NOT_EXISTS );
        else if ( !std::filesystem::is_directory( wanted ) ) sock.sendo( FilesystemResult::IS_NOT_DIRECTORY);
        else {

            sock.sendo( FilesystemResult::OK );
            sock.sends( session.userify( wanted ).string() );
            session.cwd_directory = wanted;

        }

    });


    Commands::add_command( CommandList::LS, "ls", []( Session& session, const IOSocket& sock ) {

        static std::mutex mutex;

        path_t root;

        const path_t wanted = sock.recvss();
        const bool deep = sock.recvo<bool>();
        const bool hidden = sock.recvo<bool>();
        const bool stat = sock.recvo<bool>();
        const bool magic = sock.recvo<bool>();

        try {

            root = session.serverify( wanted );

        }
        catch ( const std::exception& e ) {

            sock.sendo( FilesystemResult::NOT_ACCESSIBLE );

            return;

        }

        if ( !std::filesystem::is_directory( root ) ) {

            sock.sendo( FilesystemResult::IS_NOT_DIRECTORY );

            return;

        }

        if ( magic && !session.parent.magic ) {

            sock.sendo( FilesystemResult::NOT_SUPPORTED );

            return;

        }

        sock.sendo( FilesystemResult::OK );
        
        std::vector<EntryInfo> table;

        std::unique_lock lock( mutex );

        bool found = false;

        list_directory( root, table, deep, hidden, stat, magic, session.parent, std::nullopt, false, found );

        lock.unlock();

        sock.sendo( static_cast<qword_t>( table.size() ) );

        for ( const auto& entry : table ) {

            sock.sends( session.userify( entry.path ).string() );

            if ( entry.stat ) {

                sock.sendo( true );

                sock.sendo( static_cast<qword_t>( entry.stat->size ) );
                sock.sendo( static_cast<byte_t>( entry.stat->type ) );

            }
            else sock.sendo( false );

            if ( entry.magic ) {

                sock.sendo( true );

                sock.sendo( static_cast<qword_t>( entry.magic->description.size() ) );
                for ( const auto& s : entry.magic->description ) sock.sends( s );

                sock.sendo( static_cast<qword_t>( entry.magic->mime.size() ) );
                for ( const auto& s : entry.magic->mime ) sock.sends( s );

                sock.sendo( static_cast<qword_t>( entry.magic->encoding.size() ) );
                for ( const auto& s : entry.magic->encoding ) sock.sends( s );

            }
            else sock.sendo( false );

        }

    });


    Commands::add_command( CommandList::SPACE, "space", []( Session& session, const IOSocket& sock ) {

        path_t wanted = sock.recvss();

        try {

            wanted = session.serverify( wanted );

        }
        catch ( const std::exception& e ) {

            sock.sendo( FilesystemResult::NOT_ACCESSIBLE );

            return;

        }

        if ( !std::filesystem::exists( wanted ) ) {

            sock.sendo( FilesystemResult::NOT_EXISTS );

            return;

        }

        sock.sendo( FilesystemResult::OK );

        const auto info = std::filesystem::space( wanted );

        sock.sendo( static_cast<qword_t>( info.available ) );
        sock.sendo( static_cast<qword_t>( info.capacity ) );
        sock.sendo( static_cast<qword_t>( info.free ) );

    });


}