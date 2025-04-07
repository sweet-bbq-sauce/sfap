#pragma once


namespace sfap {

    namespace protocol {


        class Procedures {

            public:

                static void load();


            private:

                static bool _loaded;

                static void _user_group();
                static void _fs_group();
                static void _fs_mnpl_group();
                static void _ftp_group();
                static void _file_io_group();
                static void _admin_group();

        };


    }

}