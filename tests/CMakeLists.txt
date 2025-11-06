include( FetchContent )

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

set( gtest_force_shared_crt ON CACHE BOOL "" FORCE )
set( BUILD_GMOCK OFF CACHE BOOL "" FORCE )
set( BUILD_GTEST ON  CACHE BOOL "" FORCE )
set( INSTALL_GTEST OFF CACHE BOOL "" FORCE )

add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/net" )
add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/utils" )

FetchContent_MakeAvailable( googletest )
include( GoogleTest )

function ( add_gtest file )
    get_filename_component( name "${file}" NAME_WE )
    add_executable( ${name} "${file}" )
    target_link_libraries( ${name} PRIVATE sfap GTest::gtest_main )
    gtest_discover_tests( ${name} DISCOVERY_TIMEOUT 60 )
endfunction ()

foreach ( t IN LISTS TESTS )
    add_gtest( ${t} )
endforeach ()