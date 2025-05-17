#include <string>
#include <filesystem>

#include <gtest/gtest.h>

#include <utils/encoding.hpp>


using namespace sfap;


TEST( EncodingTest, BasicAsciiPath ) {

    const std::string original = "C:/directory/file.txt";
    const path_t path = utils::string_to_path( original );
    const std::string result = utils::path_to_string( path );

    EXPECT_EQ( result, original );

}


TEST( EncodingTest, UnicodePolishCharacters ) {

    const std::string original = "ścieżka/z/ęóąśłżźćń.txt";
    const path_t path = utils::string_to_path( original );
    const std::string result = utils::path_to_string( path );

    EXPECT_EQ( result, original );

}


TEST( EncodingTest, EmojiCharacters ) {
    const std::string original = "directory/💾_file.txt";
    const path_t path = utils::string_to_path( original );
    const std::string result = utils::path_to_string( path );

    EXPECT_EQ( result, original );
}


TEST( EncodingTest, RelativePath ) {

    const std::string original = "./relative/path.txt";
    const path_t path = utils::string_to_path( original );
    const std::string result = utils::path_to_string( path );

    EXPECT_EQ( result, original );

}


TEST( EncodingTest, RootPath ) {

    #ifdef _WIN32

        const std::string original = "C:\\";

    #else

        const std::string original = "/";

    #endif

    const path_t path = utils::string_to_path( original );
    const std::string result = utils::path_to_string( path );

    EXPECT_EQ( result, original );

}


TEST( EncodingTest, PathToStringToPathRoundTrip ) {

    #ifdef _WIN32

        const path_t original = L"C:\\Users\\Kurt Cobain\\yeeeeeee.txt";

    #else

        const path_t original = "/home/żółw/file.txt";

    #endif

    const std::string str = utils::path_to_string( original );
    const path_t result = utils::string_to_path( str );

    EXPECT_EQ( result, original );

}
