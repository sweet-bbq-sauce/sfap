add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/utils" )

add_library( sfap ${SOURCES} )

if( CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU" )
    target_compile_options( sfap PRIVATE -fno-exceptions -fno-rtti -Wall -Wextra -Wpedantic -Werror )
endif()