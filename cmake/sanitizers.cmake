option( USE_SANITIZERS "Build with sanitizers" OFF )

if ( USE_SANITIZERS )
    set( SANITIZERS "" CACHE STRING "Sanitizers separated by `,`" )
    if ( NOT SANITIZERS STREQUAL "" )
        add_compile_options( "-fsanitize=${SANITIZERS}" )
        add_link_options( "-fsanitize=${SANITIZERS}" ) 
    endif ()
endif ()