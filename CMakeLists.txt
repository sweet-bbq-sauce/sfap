cmake_minimum_required( VERSION 3.13 )
project( libsfap CXX )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

add_compile_definitions( _GLIBCXX_USE_CXX11_ABI=1 )

find_package( OpenSSL REQUIRED )

find_package(PkgConfig REQUIRED)
pkg_search_module(ZLIB REQUIRED zlib)

include_directories( "${CMAKE_SOURCE_DIR}/src" )

set( SFAP_DNS_DEFAULT_RESOLVE_MODE 1 CACHE STRING "Set default mode for resolving DNS hostnames" )

if(NOT SFAP_DNS_DEFAULT_RESOLVE_MODE MATCHES "^[1-3]+$")
    message(FATAL_ERROR "SFAP_DNS_DEFAULT_RESOLVE_MODE must be a number")
endif()

if(SFAP_DNS_DEFAULT_RESOLVE_MODE LESS 1 OR SFAP_DNS_DEFAULT_RESOLVE_MODE GREATER 3)
    message(FATAL_ERROR "SFAP_DNS_DEFAULT_RESOLVE_MODE must be between 1 and 3")
endif()

set( SFAP_BUFFER_SIZE "( 1024 * 1024 )" CACHE STRING "Buffer size" )
set( SFAP_DEFAULT_TIMEOUT "60" CACHE STRING "Default timeout in seconds" )

file( READ "${CMAKE_SOURCE_DIR}/VERSION" SFAP_VERSION )
string( STRIP "${SFAP_VERSION}" SFAP_VERSION )

configure_file( "${CMAKE_SOURCE_DIR}/config.h.in" "${CMAKE_SOURCE_DIR}/src/config.h" )

add_subdirectory( "${CMAKE_SOURCE_DIR}/src" )
add_subdirectory( "${CMAKE_SOURCE_DIR}/test" )