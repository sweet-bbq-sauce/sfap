#pragma once

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <cinttypes>

#include <config.h>


#ifdef _WIN32

    #include <winsock2.h>
    #include <ws2tcpip.h>

#else

    #include <sys/socket.h>
    #define INVALID_SOCKET ( ( int ) ( -1 ) )
    #define closesocket ::close

#endif


namespace sfap {


    using byte_t = uint8_t;
    using word_t = uint16_t;
    using dword_t = uint32_t;
    using qword_t = uint64_t;

    using port_t = word_t;
    using data_t = std::vector<byte_t>;
    using path_t = std::filesystem::path;
    using timestamp_t = qword_t;
    using crc_t = dword_t;


    #ifdef _WIN32

        using socket_t = SOCKET;

    #else

        using socket_t = int;

    #endif


    struct Credentials {

        std::string user, password;

        bool operator==( const Credentials& other ) const;

    };


    class MagicInfo {

        public:

            friend class Client;

            std::string get_description() const;
            std::string get_mime() const;
            std::string get_encoding() const;

        
        private:

            MagicInfo( const std::string& description, const std::string& mime, const std::string& encoding );
            
            std::string _description, _mime, _encoding;

    };


    struct StatInfo {

        public:

            friend class Client;

            qword_t get_size() const;
            path_t get_path() const;
            std::filesystem::file_type get_type() const;
            std::string get_fixed_size() const;


        private:

            qword_t size;
            path_t path;
            std::filesystem::file_type type;

    };


    struct ServerInfo {

        std::string version;
        std::vector<std::string> extensions;
        qword_t time;

    };


    extern const qword_t start_time;

    void init( bool winsock = true, bool openssl = true );


    namespace config {


        inline constexpr const char* version = SFAP_VERSION;
        inline constexpr std::size_t warning_size = ( 1024 * 1024 * 64 );   // 64MiB


        namespace defaults {

            inline constexpr std::size_t timeout = SFAP_DEFAULT_TIMEOUT;
            inline constexpr qword_t buffer_size = SFAP_BUFFER_SIZE;

        }


    };


};