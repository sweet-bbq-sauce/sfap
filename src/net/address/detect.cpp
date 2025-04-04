#include <string>
#include <regex>

#include <net/address/detect.hpp>


using namespace sfap::net;


AddressType sfap::net::detect_address_type( const std::string& address ) noexcept {

    if ( address.empty() ) return AddressType::EMPTY;
    if ( address.size() > 255 ) return AddressType::UNSUPPORTED;

    static const std::regex ipv4_regex( R"(^((25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)$)" );
    static const std::regex ipv6_regex( R"(^((([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4})|(([0-9a-fA-F]{1,4}:){1,7}:)|(([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4})|(([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2})|(([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3})|(([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4})|(([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5})|([0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6}))|(:((:[0-9a-fA-F]{1,4}){1,7}|:))|(fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,})|(::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))|(([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])))$)" );
    static const std::regex hostname_regex( R"(^([a-zA-Z0-9]([a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(\.[a-zA-Z0-9]([a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*)$)" );

    if ( std::regex_match( address, ipv4_regex ) ) return AddressType::IPV4;
    else if ( std::regex_match( address, hostname_regex ) ) return AddressType::HOSTNAME;
    else if ( std::regex_match( address, ipv6_regex ) ) return AddressType::IPV6;
    
    else return AddressType::UNSUPPORTED;

}