#!/bin/sh


if [ -f "/tmp/socks5_proxy_pid.txt" ]; then
    kill $( cat "/tmp/socks5_proxy_pid.txt" )
    rm "/tmp/socks5_proxy_pid.txt"
fi

if [ -f "/tmp/http_proxy_pid.txt" ]; then
    kill $( cat "/tmp/http_proxy_pid.txt" )
    rm "/tmp/http_proxy_pid.txt"
fi

exit 0