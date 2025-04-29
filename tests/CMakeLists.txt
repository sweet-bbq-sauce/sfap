# Detect address type
add_executable( detect_address_type detect_address_type.cpp )
target_link_libraries( detect_address_type sfap GTest::GTest GTest::Main )
add_test( NAME DetectAddressType COMMAND detect_address_type )