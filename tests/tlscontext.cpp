#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>
#include <crypto/context/context.hpp>


using namespace sfap;
using namespace sfap::crypto;


const path_t cert_file = "data/cert.pem";
const path_t key_file = "data/key.pem";
const path_t cert_encrypted_file = "data/cert_encrypted.pem";
const path_t key_encrypted_file = "data/key_encrypted.pem";
const path_t ca_dir = "data/ca-directory";


TEST( TLSContextTest, CreateAndDestroy ) {

    EXPECT_NO_THROW({ TLSContext ctx; });

}


TEST( TLSContextTest, CreateSSLInstance ) {

    TLSContext ctx;
    ssl_ptr ssl = ctx.create_ssl();
    EXPECT_NE( ssl.get(), nullptr );

}


TEST( TLSContextTest, SetVerifyModeWithoutCallback ) {

    TLSContext ctx;
    EXPECT_NO_THROW({ ctx.set_verify_mode( SSL_VERIFY_PEER ); });

}


TEST( TLSContextTest, SetVerifyModeWithCallback ) {

    TLSContext ctx;
    auto cb = []( int ok, X509_STORE_CTX* ) -> int {

        return ok;

    };

    EXPECT_NO_THROW({ ctx.set_verify_mode( SSL_VERIFY_PEER, cb ); });

}


TEST( TLSContextTest, InvalidCertificateStringThrows ) {

    TLSContext ctx;
    const std::string invalid_cert = "-----BEGIN CERTIFICATE-----\nINVALID\n-----END CERTIFICATE-----";
    EXPECT_THROW( { ctx.load_cert_string( invalid_cert ); }, std::runtime_error );

}


TEST( TLSContextTest, InvalidKeyStringThrows ) {

    TLSContext ctx;
    const std::string invalid_key = "-----BEGIN PRIVATE KEY-----\nINVALID\n-----END PRIVATE KEY-----";
    EXPECT_THROW( { ctx.load_key_string( invalid_key ); }, std::runtime_error );

}


TEST( TLSContextTest, LoadValidCertFromFile ) {

    TLSContext ctx;
    std::ifstream ifs( cert_file );
    ASSERT_TRUE( ifs.is_open() ) << "Cannot open cert file";

    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string cert_pem = ss.str();

    EXPECT_NO_THROW( ctx.load_cert_string( cert_pem ) );

}


TEST( TLSContextTest, LoadValidKeyFromFile ) {

    TLSContext ctx;
    std::ifstream ifs( key_file );
    ASSERT_TRUE( ifs.is_open() ) << "Cannot open key file";

    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string key_pem = ss.str();

    EXPECT_NO_THROW( ctx.load_key_string( key_pem ) );

}


TEST( TLSContextTest, AddCaFilesIndividually ) {

    TLSContext ctx;

    for ( int i = 1; i <= 5; ++i ) {

        path_t ca_file = ca_dir / ( "ca_cert" + std::to_string( i ) + ".pem" );
        ASSERT_TRUE( std::filesystem::exists( ca_file ) ) << "Missing CA file: " << ca_file;

        EXPECT_NO_THROW( ctx.add_ca_file( ca_file ) );

    }

}


TEST( TLSContextTest, AddCaPath ) {

    TLSContext ctx;
    ASSERT_TRUE( std::filesystem::exists( ca_dir ) ) << "Missing CA directory";

    EXPECT_NO_THROW( ctx.add_ca_path( ca_dir ) );

}


TEST( TLSContextTest, LoadEncryptedKeyFile ) {

    TLSContext ctx;

    const std::string password = "sfap";

    EXPECT_THROW({

        ctx.load_key_file( key_encrypted_file );

    }, std::runtime_error);

    EXPECT_NO_THROW({

        ctx.load_key_file( key_encrypted_file, password );

    });

}
