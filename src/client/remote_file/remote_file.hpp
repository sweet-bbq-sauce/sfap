#pragma once


#include <sstream>

#include <client/client.hpp>


namespace sfap {


    class RemoteFile {

        public:

            friend class Client;

            RemoteFile& operator=( const RemoteFile& other ) = delete;
            RemoteFile( const RemoteFile& other ) = delete;

            RemoteFile( RemoteFile&& other );
            RemoteFile& operator=( RemoteFile&& other );

            ~RemoteFile();

            void close();

            remotefile_t get_id() const;

            void flush() const;

            char get() const;
            void put( char data ) const;

            void read( char* data, std::streamsize size ) const;
            void read( data_t& data, std::streamsize size ) const;
            std::streamsize gcount() const;

            void write( const char* data, std::streamsize size ) const;
            void write( const std::string& data ) const;
            void write( const data_t& data ) const;

            std::streampos tellg() const;
            void seekg( std::streamoff offset, std::ios::seekdir dir ) const;
            void seekg( std::streampos position ) const;

            std::streampos tellp() const;
            void seekp( std::streamoff offset, std::ios::seekdir dir ) const;
            void seekp( std::streampos position ) const;

            bool eof() const;


            template<typename T>
            RemoteFile& operator<<( const T& object ) const;

            RemoteFile& operator<<( const std::string& data ) const;
            RemoteFile& operator<<( const data_t& data ) const;

            template<typename T>
            RemoteFile& operator>>( T& object ) const;

            RemoteFile& operator>>( std::string& word ) const;
            void getline( std::string& line ) const;

            void set_buffer_threshold( std::optional<std::streamsize> threshold );

        
        private:

            RemoteFile( const Client& client, remotefile_t file_id );

            std::optional<std::streamsize> _buffer_threshold;
            mutable std::ostringstream _output_buffer;
            mutable qword_t _gcount;
            Client _client;
            remotefile_t _file_id;
            bool _opened;

    };


}