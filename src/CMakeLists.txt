file( GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )

add_library( sfap ${SOURCES} )

if( WIN32 )

    target_link_libraries( sfap PRIVATE "ws2_32" )

endif()