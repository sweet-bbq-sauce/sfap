if ( SUPPORTED_IOURING )

set( TESTS
    ${TESTS}
    "${CMAKE_CURRENT_SOURCE_DIR}/iouring.cpp"
    PARENT_SCOPE
)

endif ()