cmake_minimum_required( VERSION 3.10 )

project( sfap LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

include( "${CMAKE_SOURCE_DIR}/cmake/doxygen.cmake" )
include( "${CMAKE_SOURCE_DIR}/cmake/sanitizers.cmake" )

if ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
    add_compile_options( -Wno-psabi )
endif()

include_directories( "${CMAKE_SOURCE_DIR}/include" )

add_subdirectory( "${CMAKE_SOURCE_DIR}/src" )

option( BUILD_TESTS "Build gtests" ON )

if ( BUILD_TESTS )
    enable_testing()
    add_subdirectory( "${CMAKE_SOURCE_DIR}/tests" )
endif ()