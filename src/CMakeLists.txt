file( GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp" )
list( APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sfap.cpp" )

add_library( sfap ${SOURCES} )

target_link_libraries( sfap PRIVATE ${ZLIB_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto )