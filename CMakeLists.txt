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


# Default resolve mode
set( LIBSFAP_DEFAULT_RESOLVE_MODE "Prefer IPv6" CACHE STRING "Choose the default DNS mode: 'IPv4', 'IPv6', 'Prefer IPv6'" )
set_property( CACHE LIBSFAP_DEFAULT_RESOLVE_MODE PROPERTY STRINGS "IPv4" "IPv6" "Prefer IPv6" )

message( "Default resolve mode is: ${LIBSFAP_DEFAULT_RESOLVE_MODE}" )

if( LIBSFAP_DEFAULT_RESOLVE_MODE STREQUAL "IPv4" )

    set( LIBSFAP_DEFAULT_RESOLVE_MODE_ID 0 )

elseif( LIBSFAP_DEFAULT_RESOLVE_MODE STREQUAL "IPv6" )

    set( LIBSFAP_DEFAULT_RESOLVE_MODE_ID 1 )

elseif( LIBSFAP_DEFAULT_RESOLVE_MODE STREQUAL "Prefer IPv6" )

    set( LIBSFAP_DEFAULT_RESOLVE_MODE_ID 2 )

else()

    message( FATAL_ERROR "Invalid LIBSFAP_DEFAULT_RESOLVE_MODE. Allowed: IPv4, IPv6, Prefer IPv6" )

endif()


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


# Doxygen support
option( BUILD_DOCS "Build documentation with Doxygen" OFF )

if ( BUILD_DOCS )

    find_package( Doxygen REQUIRED )

    set( DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/docs" )
    set( DOXYGEN_INPUT "${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/README.md" )
    set( DOXYGEN_IMAGE_PATH "${CMAKE_SOURCE_DIR}/docs/assets" )

    set( DOXYGEN_OUTPUT_LANGUAGE "English" CACHE STRING "Language used in Doxygen documentation" )

    configure_file(

        "${CMAKE_SOURCE_DIR}/Doxyfile.in"
        "${CMAKE_BINARY_DIR}/Doxyfile"
        @ONLY

    )

    set( DOXYGEN_CONFIG "${CMAKE_BINARY_DIR}/Doxyfile" )

    add_custom_target(

        doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating Doxygen documentation ..."
        VERBATIM

    )

endif()


# Google Test support
option( BUILD_TESTS "Build gtests" ON )

if ( BUILD_TESTS )

    find_package( GTest REQUIRED )

    enable_testing()

    add_subdirectory( "${CMAKE_SOURCE_DIR}/tests" )

endif()