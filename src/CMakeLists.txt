include( CheckCXXSourceCompiles )
include( FetchContent )

add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/net" )
add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/utils" )

set( SOURCES
    ${SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/error.cpp"
)

add_library( sfap ${SOURCES} )

check_cxx_source_compiles(
    "#include <expected>
    int main(){ std::expected<int,int> e; return 0; }"
    HAVE_STD_EXPECTED
)

if ( NOT HAVE_STD_EXPECTED )
    FetchContent_Declare(
        tl_expected
        GIT_REPOSITORY https://github.com/TartanLlama/expected.git
        GIT_TAG v1.1.0
    )
    FetchContent_MakeAvailable( tl_expected )
    target_include_directories( sfap PUBLIC "${tl_expected_SOURCE_DIR}/include" )
endif()

if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU" )
    target_compile_options( sfap PRIVATE -fno-exceptions -fno-rtti -Wall -Wextra -Wpedantic -Werror )
endif ()

if ( MSVC )
    target_compile_options( sfap PRIVATE /GR- /W4 /WX /permissive- )
    target_compile_definitions( sfap PRIVATE PRIVATE_HAS_EXCEPTIONS=0 )
endif()

if ( WIN32 )
    target_link_libraries( sfap PRIVATE ws2_32 )
endif ()

if ( LIBURING_FOUND )
    target_include_directories( sfap INTERFACE ${LIBURING_INCLUDE_DIRS} )
    target_link_libraries( sfap INTERFACE ${LIBURING_LIBRARIES} )
endif ()