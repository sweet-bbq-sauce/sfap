add_executable( client "${CMAKE_CURRENT_SOURCE_DIR}/client.cpp" )
target_link_libraries( client PUBLIC sfap )

add_executable( server "${CMAKE_CURRENT_SOURCE_DIR}/server.cpp" )
target_link_libraries( server PUBLIC sfap )

if( WIN32 )

    target_link_libraries( client PRIVATE ws2_32 )
    target_link_libraries( server PRIVATE ws2_32 )

endif()