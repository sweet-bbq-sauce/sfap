#include <gtest/gtest.h>


#include <server/virtual_filesystem/virtual_filesystem.hpp>


using namespace sfap;
using namespace sfap::protocol;


// Helper for root path setup
static path_t get_root_dir() {
    return std::filesystem::current_path();
}


// Constructor: should throw on relative path
TEST( VirtualFilesystem, Constructor_Throws_On_Relative_Path ) {

    EXPECT_THROW( VirtualFilesystem( path_t( "relative/path" ) ), std::invalid_argument );

}


// Constructor: should succeed on absolute path
TEST( VirtualFilesystem, Constructor_Succeeds_On_Absolute_Path ) {

    EXPECT_NO_THROW( VirtualFilesystem vfs( get_root_dir() ) );

}


// check_access: inside root
TEST( VirtualFilesystem, CheckAccess_InsideRoot ) {

    VirtualFilesystem vfs( get_root_dir() );

    EXPECT_EQ( (int)vfs.check_access( get_root_dir() ), (int)AccessResult::OK );

}


// check_access: outside root
TEST( VirtualFilesystem, CheckAccess_OutsideRoot ) {

    VirtualFilesystem vfs( get_root_dir() );

    path_t outside = get_root_dir().parent_path().parent_path();  // go up to ensure it's outside
    EXPECT_EQ( (int)vfs.check_access( outside ), (int)AccessResult::OUTSIDE_ROOT );

}


// set_home: valid path
TEST( VirtualFilesystem, SetHome_Valid ) {

    VirtualFilesystem vfs( get_root_dir() );

    EXPECT_EQ( (int)vfs.set_home( path_t( "/" ) ), (int)AccessResult::OK );

}


// cd: valid path change
TEST( VirtualFilesystem, CD_Valid ) {

    VirtualFilesystem vfs( get_root_dir() );
    EXPECT_EQ( (int)vfs.cd( path_t( "/" ) ), (int)AccessResult::OK );
}


// to_virtual and to_system: round-trip
TEST( VirtualFilesystem, Path_RoundTrip_ToSystemToVirtual ) {

    VirtualFilesystem vfs( get_root_dir() );

    path_t subpath = get_root_dir() / "subdir";
    std::filesystem::create_directories( subpath );

    auto virtual_path = vfs.to_virtual( subpath );
    auto system_path = vfs.to_system( virtual_path );

    EXPECT_EQ( std::filesystem::weakly_canonical( system_path ), std::filesystem::weakly_canonical( subpath ) );

    std::filesystem::remove_all( subpath );

}


// normalize: should resolve and validate virtual paths
TEST( VirtualFilesystem, Normalize_ValidPath ) {

    VirtualFilesystem vfs( get_root_dir() );

    auto result = vfs.normalize( path_t( "/" ) );

    EXPECT_EQ( result.generic_string(), "/" );

}


// try_to_virtual: invalid -> nullopt
TEST( VirtualFilesystem, TryToVirtual_Invalid ) {

    VirtualFilesystem vfs( get_root_dir() );

    path_t out = get_root_dir().parent_path();
    auto res = vfs.try_to_virtual( out );

    EXPECT_EQ( res.has_value(), false );

}


// get_home and pwd
TEST( VirtualFilesystem, GetHomeAndPwd_Default ) {

    VirtualFilesystem vfs( get_root_dir() );

    EXPECT_EQ( vfs.get_home(), "/" );
    EXPECT_EQ( vfs.pwd(), "/" );

}
