add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/platform")

set( TESTS
    ${TESTS}
    "${CMAKE_CURRENT_SOURCE_DIR}/address.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/detect_address_kind.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/resolve.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/types.cpp"
    PARENT_SCOPE
)