#include <fstream>
#include <optional>
#include <map>
#include <mutex>

#include <utils/misc.hpp>
#include <utils/log.hpp>
#include <utils/format.hpp>


using namespace sfap;
using namespace sfap::utils;


const qword_t start_time = timestamp();
std::mutex Log::_mutex;
std::ofstream Log::_log_file;
bool Log::write_to_terminal = true;
bool Log::use_color = true;
bool Log::debug_mode = false;


const std::map<Log::Level, std::string> Log::_strings = {

    { Level::DEBUG, "DEBUG" },
    { Level::NOTE, "NOTE" },
    { Level::INFO, "INFO" },
    { Level::WARNING, "WARNING" },
    { Level::ERROR, "ERROR" },
    { Level::CRITICAL, "CRITICAL" }

};


const std::map<Log::Level, std::string> Log::_colors = {

    { Level::DEBUG, "\033[92m" },
    { Level::NOTE, "\033[36m" },
    { Level::INFO, "\033[34m" },
    { Level::WARNING, "\033[33m" },
    { Level::ERROR, "\033[31m" },
    { Level::CRITICAL, "\033[30;41m" }
        
};


void Log::plain( Level level, const std::string& message ) {

    const auto time = utils::timestamp();

    if ( ( level == Level::DEBUG ) && !debug_mode ) return;

    std::lock_guard<std::mutex> lock( _mutex );

    if ( write_to_terminal ) {

        if ( use_color ) std::cout << _colors.at( level );
        std::cout << _strings.at( level ) << ":";
        if ( use_color ) std::cout << "\033[0m";
        std::cout << " " << message << std::endl;

    }

    if ( _log_file.is_open() ) {

        const std::string line = glue( _strings.at( level ), ": ", message, utils::endline );

        _log_file.write( line.data(), line.size() );
        _log_file.flush();

    }

}


void Log::set_log_file( const path_t& file ) {

    std::lock_guard<std::mutex> lock( _mutex );

    if ( _log_file.is_open() ) _log_file.close();

    _log_file.open( file, std::ios::app );

}


void Log::unset_log_file() {

    std::lock_guard<std::mutex> lock( _mutex );

    if ( _log_file.is_open() ) _log_file.close();

}