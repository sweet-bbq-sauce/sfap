#pragma once


#include <mutex>

extern "C" {

    #include <magic.h>

}

#include <sfap.hpp>
#include <utils/misc.hpp>


namespace sfap {

    namespace utils {


        class Magic {

            public:

                Magic() {

                    _cookie = magic_open( base );

                    if ( !_cookie ) throw std::runtime_error( "Can't open libMagic context" );

                    if ( magic_load( _cookie, nullptr ) != 0 ) {

                        const std::string error = magic_error( _cookie );

                        magic_close( _cookie );

                        throw std::runtime_error( "magic_load() error: " + error );

                    }

                }


                ~Magic() {

                    magic_close( _cookie );

                }


                MagicInfo scan_file( const path_t& file ) const {

                    const std::unique_lock lock( _mutex );

                    MagicInfo info;
                    const std::string path = utils::path_to_string( file );

                    magic_setflags( _cookie, base );
                    info.description = _split( magic_file( _cookie, path.c_str() ), "\\012- " );

                    magic_setflags( _cookie, base | MAGIC_MIME_TYPE );
                    info.mime = _split( magic_file( _cookie, path.c_str() ), "\\012- " );

                    magic_setflags( _cookie, base | MAGIC_MIME_ENCODING );
                    info.encoding = _split( magic_file( _cookie, path.c_str() ), "\\012- " );

                    return info;

                }


            private:

                static constexpr int base = MAGIC_COMPRESS | MAGIC_DEVICES | MAGIC_CONTINUE;

                static std::vector<std::string> _split(const std::string& str, const std::string& delimiter) {

                    std::vector<std::string> tokens;
                    size_t start = 0;
                    size_t end = str.find(delimiter);
                    
                    while (end != std::string::npos) {
                        tokens.push_back(str.substr(start, end - start));
                        start = end + delimiter.length();
                        end = str.find(delimiter, start);
                    }
                    tokens.push_back(str.substr(start));
                    
                    return tokens;
                }

                magic_t _cookie;
                mutable std::mutex _mutex;

        };


    }

}