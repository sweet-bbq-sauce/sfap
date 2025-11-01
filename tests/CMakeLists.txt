include( FetchContent )

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set( BUILD_GMOCK OFF CACHE BOOL "" FORCE )
set( BUILD_GTEST ON  CACHE BOOL "" FORCE )
set( INSTALL_GTEST OFF CACHE BOOL "" FORCE )

FetchContent_MakeAvailable( googletest )
include( GoogleTest )

function ( add_gtest name )
    add_executable( ${name} "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp" )
    target_link_libraries( ${name} PRIVATE sfap GTest::gtest_main )
    gtest_discover_tests( ${name} DISCOVERY_TIMEOUT 60 )
endfunction ()

set( TESTS
    "ringbuffer"
)

foreach ( t IN LISTS TESTS )
    add_gtest( ${t} )
endforeach ()