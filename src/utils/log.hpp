#include <map>
#include <mutex>
#include <string>

#include <sfap.hpp>
#include <utils/misc.hpp>

#ifdef _WIN32

    #undef ERROR

#endif


namespace sfap {

    namespace utils {


        class Log {

            public:

                Log() = delete;
                Log( const Log& ) = delete;
                Log& operator=( const Log& ) = delete;
                Log( Log&& ) = delete;
                Log& operator=( Log&& ) = delete;

                enum class Level {

                    DEBUG,
                    NOTE,
                    INFO,
                    WARNING,
                    ERROR,
                    CRITICAL

                };

                static void set_log_file( const path_t& file );
                static void unset_log_file();
                
                static void plain( Level level, const std::string& message );

                static bool write_to_terminal;
                static bool use_color;
                static bool debug_mode;


            private:

                static std::mutex _mutex;
                static std::ofstream _log_file;
                static const std::map<Level, std::string> _strings;
                static const std::map<Level, std::string> _colors;

        };


        template<typename... Args>
        void debug( Args&&... args ) {

            Log::plain( Log::Level::DEBUG, utils::glue( std::forward<Args>( args )... ) );

        }


        template<typename... Args>
        void note( Args&&... args ) {

            Log::plain( Log::Level::NOTE, utils::glue( std::forward<Args>( args )... ) );

        }


        template<typename... Args>
        void info( Args&&... args ) {

            Log::plain( Log::Level::INFO, utils::glue( std::forward<Args>( args )... ) );

        }


        template<typename... Args>
        void warning( Args&&... args ) {

            Log::plain( Log::Level::WARNING, utils::glue( std::forward<Args>( args )... ) );

        }


        template<typename... Args>
        void error( Args&&... args ) {

            Log::plain( Log::Level::ERROR, utils::glue( std::forward<Args>( args )... ) );

        }


        template<typename... Args>
        void critical( Args&&... args ) {

            Log::plain( Log::Level::CRITICAL, utils::glue( std::forward<Args>( args )... ) );

        }


    }

}