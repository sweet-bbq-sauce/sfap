# Generate test certificates
execute_process(

    COMMAND "bash" "${CMAKE_SOURCE_DIR}/scripts/generate-test-certificates.sh" "${CMAKE_BINARY_DIR}"
    
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error

)

if( NOT result EQUAL 0 )

    message( FATAL_ERROR "Error in generating test certificates: ${result}" )

endif()

message( STATUS "${output}" )


# Detect address type
add_executable( detect_address_type detect_address_type.cpp )
target_link_libraries( detect_address_type sfap GTest::GTest GTest::Main )
add_test( NAME DetectAddressType COMMAND detect_address_type )


# Resolver
add_executable( resolve resolve.cpp )
target_link_libraries( resolve sfap GTest::GTest GTest::Main )
add_test( NAME Resolve COMMAND resolve )


# TLSContext
add_executable( tlscontext tlscontext.cpp )
target_link_libraries( tlscontext sfap GTest::GTest GTest::Main )
add_test( NAME TLSContext COMMAND tlscontext )


# Encoding
add_executable( encoding encoding.cpp )
target_link_libraries( encoding sfap GTest::GTest GTest::Main )
add_test( NAME Encoding COMMAND encoding )


# Host
add_executable( host host.cpp )
target_link_libraries( host sfap GTest::GTest GTest::Main )
add_test( NAME Host COMMAND host )


# IOSocket
add_executable( iosocket iosocket.cpp )
target_link_libraries( iosocket sfap GTest::GTest GTest::Main )
add_test( NAME IOSocket COMMAND iosocket )


# Socks5 proxy
add_executable( socks5_proxy socks5_proxy.cpp )
target_link_libraries( socks5_proxy sfap GTest::GTest GTest::Main )
add_test( NAME Socks5 COMMAND socks5_proxy )


# HTTP proxy
add_executable( http_proxy http_proxy.cpp )
target_link_libraries( http_proxy sfap GTest::GTest GTest::Main )
add_test( NAME Http_Proxy COMMAND http_proxy )


# Address
add_executable( address address.cpp )
target_link_libraries( address sfap GTest::GTest GTest::Main )
add_test( NAME Address COMMAND address )