option( ENABLE_DOC "Build documentation with Doxygen" OFF )

if ( ENABLE_DOC )
    find_package( Doxygen REQUIRED dot )

    set( DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/docs" )
    set( DOXYGEN_INPUT "${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/README.md" )
    set( DOXYGEN_IMAGE_PATH "${CMAKE_SOURCE_DIR}/docs/assets" )
    set( DOXYGEN_OUTPUT_LANGUAGE "English" CACHE STRING "Language used in Doxygen documentation" )
    
    configure_file( "${CMAKE_SOURCE_DIR}/Doxyfile.in" "${CMAKE_BINARY_DIR}/Doxyfile" @ONLY )
    set( DOXYGEN_CONFIG "${CMAKE_BINARY_DIR}/Doxyfile" )

    add_custom_target(

        doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating Doxygen documentation ..."
        VERBATIM

    )
endif ()