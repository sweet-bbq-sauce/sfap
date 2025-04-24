cmake_minimum_required( VERSION 3.10 )


project(

    sfap
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "A modern alternative to FTP/SFTP in C++ with middleware, security layers, and remote file I/O."
    HOMEPAGE_URL "https://github.com/sweet-bbq-sauce/sfap"

)


# Set minimum C++ standard to 17
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )


# Flush configuration to config.hpp
configure_file(

    "${CMAKE_SOURCE_DIR}/config.hpp.in"
    "${CMAKE_BINARY_DIR}/config.hpp"
    @ONLY

)


include_directories(

    "${CMAKE_BINARY_DIR}"       # for config.hpp
    "${CMAKE_SOURCE_DIR}/src"   # for own headers

)


add_subdirectory( "${CMAKE_SOURCE_DIR}/src" )