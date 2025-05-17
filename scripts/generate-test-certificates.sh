#!/bin/bash


set -e


if [ -d "$1" ]; then

    tests_data="$1/tests/data"

    echo -n "Generating test certificates to $tests_data ... "

    mkdir -p "$tests_data/ca-directory"

    # Certificate and private key
    openssl req -x509 -newkey rsa:2048 -nodes \
        -keyout "$tests_data/key.pem" \
        -out "$tests_data/cert.pem" \
        -days 365 \
        -subj "//CN=Test Cert"

    # Certificate and encrypted private key (password: sfap)
    openssl req -x509 -newkey rsa:2048 \
        -keyout "$tests_data/key_encrypted.pem" \
        -out "$tests_data/cert_encrypted.pem" \
        -days 365 \
        -passout pass:sfap \
        -subj "//CN=Test Cert Encrypted"

    # Authority certificates
    for i in {1..5}; do

        openssl req -x509 -newkey rsa:2048 -nodes \
            -keyout "$tests_data/ca-directory/ca_key$i.pem" \
            -out "$tests_data/ca-directory/ca_cert$i.pem" \
            -days 365 \
            -subj "//CN=Test CA $i" \
            -addext "basicConstraints=CA:TRUE" \
            -addext "keyUsage=critical,digitalSignature,cRLSign,keyCertSign"

    done

    echo -n "OK"
    exit 0

else

    echo "$1 is not a directory"
    exit 1

fi