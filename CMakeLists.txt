cmake_minimum_required( VERSION 3.10 )

project( sfap LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

include( "${CMAKE_SOURCE_DIR}/cmake/doxygen.cmake" )
include( "${CMAKE_SOURCE_DIR}/cmake/platform.cmake" )
include( "${CMAKE_SOURCE_DIR}/cmake/sanitizers.cmake" )

if ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
    add_compile_options( -Wno-psabi )
endif()

if ( MSVC )
    set( CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" )
endif ()

include_directories( "${CMAKE_SOURCE_DIR}/include" )
include_directories( "${CMAKE_BINARY_DIR}/include" )

add_subdirectory( "${CMAKE_SOURCE_DIR}/src" )

option( BUILD_TESTS "Build gtests" ON )

if ( BUILD_TESTS )
    enable_testing()
    add_subdirectory( "${CMAKE_SOURCE_DIR}/tests" )
endif ()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/sfap/config.hpp"
    @ONLY
)