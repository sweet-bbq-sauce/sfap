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

set( TESTS
    ${TESTS}
    "${CMAKE_CURRENT_SOURCE_DIR}/error.cpp"
)

FetchContent_MakeAvailable( googletest )
include( GoogleTest )

function ( add_gtest file )
    file( RELATIVE_PATH rel "${CMAKE_SOURCE_DIR}/tests" "${file}" )
    get_filename_component( dir "${rel}" DIRECTORY )
    get_filename_component( name "${rel}" NAME_WE )

    add_executable( ${name} ${file} )
    target_link_libraries( ${name} PRIVATE sfap GTest::gtest_main )

    set_target_properties( ${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/${dir}" )

    gtest_discover_tests( ${name} DISCOVERY_TIMEOUT 60 )
endfunction ()

foreach ( t IN LISTS TESTS )
    add_gtest( ${t} )
endforeach ()