#pragma once


#include <unordered_map>
#include <functional>
#include <vector>
#include <shared_mutex>

#include <net/address/host.hpp>
#include <utils/misc.hpp>


namespace std {

    template<>
    struct hash<sfap::net::Host> {

        size_t operator()( const sfap::net::Host& h ) const {

            return std::hash<std::string>{}( h.get_hostname() ) ^ std::hash<sfap::port_t>{}( h.get_port() );

        }

    };

}


namespace sfap {

    namespace utils {


        class Firewall {

            public:

                enum class Action {

                    ALLOW,
                    DENY

                };

                using RuleTable = std::unordered_map<net::Host, Action>;

                Firewall( Action default_action = Action::ALLOW );

                void reset() noexcept;
                void set_default_action( Action action ) noexcept;

                bool test( const net::Host& host ) const noexcept;

                void set_rule( const net::Host& host, Action action ) noexcept;
                void set_ruletable( const RuleTable& table ) noexcept;

                void load_from_file( const path_t& file, Action action );

                void whitelist( const std::vector<net::Host>& hosts ) noexcept;
                void whitelist( const path_t& file );

                void blacklist( const std::vector<net::Host>& hosts ) noexcept;
                void blacklist( const path_t& file );
                

            private:

                Action _default_action;
                RuleTable _table;
                mutable std::shared_mutex _mutex;

        };


    }

}