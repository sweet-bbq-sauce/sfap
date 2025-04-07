#include <mutex>
#include <vector>
#include <unordered_map>

#include <utils/firewall/firewall.hpp>
#include <net/address/host.hpp>
#include <sfap.hpp>


using namespace sfap::utils;


Firewall::Firewall( Action default_action ) :
    _default_action( default_action ) {}


void Firewall::reset() noexcept {

    std::unique_lock lock( _mutex );

    _table.clear();

}


void Firewall::set_default_action( Action action ) noexcept {

    std::unique_lock lock( _mutex );

    _default_action = action;

}


void Firewall::set_rule( const net::Host& host, Action action ) noexcept {

    std::unique_lock lock( _mutex );

    _table[host] = action;

}


bool Firewall::test( const net::Host& host ) const noexcept {

    std::shared_lock lock( _mutex );

    const auto it = _table.find( host );

    if ( it != _table.end() ) return it->second == Action::ALLOW;
    else return _default_action == Action::ALLOW;

}


void Firewall::set_ruletable( const RuleTable& table ) noexcept {

    std::unique_lock lock( _mutex );

    for ( const auto& record : table ) _table[record.first] = record.second;

}


void Firewall::whitelist( const std::vector<net::Host>& hosts ) noexcept {

    std::unique_lock lock( _mutex );

    for ( const net::Host& host : hosts ) _table[host] = Action::ALLOW;

}


void Firewall::blacklist( const std::vector<net::Host>& hosts ) noexcept {

    std::unique_lock lock( _mutex );

    for ( const net::Host& host : hosts ) _table[host] = Action::DENY;

}