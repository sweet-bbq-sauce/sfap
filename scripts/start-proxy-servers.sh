#!/bin/sh


SCRIPT_DIR="$(dirname "$0")"

python3 "$SCRIPT_DIR/socks5-proxy-server.py" &
echo $! > /tmp/socks5_proxy_pid.txt

python3 "$SCRIPT_DIR/http-proxy-server.py" &
echo $! > /tmp/http_proxy_pid.txt

exit 0