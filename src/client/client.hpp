#pragma once


#include <future>
#include <functional>
#include <optional>

#include <sfap.hpp>
#include <protocol/protocol.hpp>
#include <net/address/address.hpp>
#include <net/iosocket/iosocket.hpp>
#include <net/connect/connect.hpp>
#include <server/command/command.hpp>


namespace sfap {


    class RemoteFile;


    class Client {

        public:

            friend class RemoteFile;

            using remotefile_t = dword_t;

            Client( const net::Address& address, std::optional<Credentials> credentials = std::nullopt, std::optional<path_t> remote_path = std::nullopt );
            Client& operator=( const Client& other ) = delete;
            Client( const Client& other );
            Client( Client&& other );
            Client& operator=( Client&& other );

            ~Client();


            /**
             * @brief Stwórz nowe, osobne połączenie na wzór aktualnego. Przydatne dla asynchronicznych operacji.
             */
            Client clone() const;


            /**
             * @brief Zamyka połączenie.
             * @note Ta metoda jest automatycznie wywoływana w destruktorze.
             */
            void close();


            /**
             * @brief Pobierz informacje serwera.
             * @see ServerInfo
             */
            ServerInfo info() const;

            
            /**
             * @brief Pusta komenda, nie robi nic. Może być użyta do pingowania, utrzymywania połączenia itd.
             */
            void none() const;

            /**
             * @brief Zaloguj się.
             * @returns Wynik logowania.
             */
            protocol::LoginResult login( const Credentials& credentials );

            /**
             * @brief Wyloguj się.
             */
            void logout();

            /**
             * @brief Czy klient jest zalogowany?
             */
            bool is_logged() const;

            /**
             * @brief Dostań nazwę użytkownika.
             */
            std::string whoami() const;

            /**
             * @brief Dostań ścieżkę domową.
             */
            path_t home() const;

            /**
             * @brief Dostań aktualną ścieżkę.
             */
            path_t pwd() const;

            /**
             * @brief Zmień aktualną ścieżkę.
             */
            path_t cd( const path_t& directory ) const;

            /**
             * @brief Wylistuj zawartość katalogu.
             * @param directory ścieżka do wylistowania.
             * @param deep włącza listowanie rekurencyjne.
             * @return Zwraca vector nazw obiektów. Jeśli opcja `deep` jest włączony to zwraca vector z ścieżkami względem katalogu.
             */
            std::vector<path_t> ls( const path_t& directory = ".", bool deep = false ) const;

            /**
             * @brief Znajdź obiekt lub obiekty w katalogu na podstawie `pattern`.
             * @param pattern wzorzec znajdowania celu.
             * @param directory katalog w którym szukamy.
             * @param end_on_first jeśli włączony, zwraca tylko pierwszy znaleziony obiekt i kończy działanie.
             * @param deep jeśli włączony, szuka rekurencyjnie.
             * @return Zwraca vector nazw znalezionego obiektu(-ów). Jeśli opcja `deep` jest włączony to zwraca vector z ścieżkami względem katalogu.
             */
            std::vector<path_t> find( const std::string& pattern, const path_t& directory = ".", bool end_on_first = true, bool deep = false ) const;

            /**
             * @brief Dostań informacje o obiekcie
             */
            StatInfo stat( const path_t& entry ) const;

            /**
             * @brief Dostań magiczne informacje o obiekcie.
             * @note Serwer musi wspierać `libMagic`.
             */
            MagicInfo magic( const path_t& entry ) const;

            /**
             * @brief Sprawdź czy obiekt istnieje.
             */
            bool exists( const path_t& entry ) const;

            /**
             * @brief Stwórz nowy plik.
             * @param new_file ścieżka nowego pliku.
             * @param size długość nowego pliku. Domyślnie `0`
             * @param fill zawartość nowego pliku. Domyślnie `'\0'`
             * @throws Metoda rzuci wyjątek, jeśli plik już istnieje.
             */
            void touch( const path_t& new_file, qword_t size = 0, char fill = '\0' ) const;

            /**
             * @brief Stwórz nowy katalog
             * @throws Metoda rzuci wyjątek, jeśli katalog już istnieje.
             */
            void mkdir( const path_t& new_directory ) const;

            /**
             * @brief Usuń plik.
             * @throws Metoda rzuci wyjątek, jeśli obiekt nie będzie plikiem.
             * @see rmdir() do usuwania katalogów.
             */
            void rm( const path_t& file ) const;

            /**
             * @brief Usuń katalog.
             * @param recursive jeśli włączony, katalog zostanie usunięty razem z zawartością. Domyślnie `false`.
             * @throws Metoda rzuci wyjątek, jeśli obiekt nie będzie katalogiem, lub gdy katalog ma zawartość a `recursive` jest wyłączony.
             */
            void rmdir( const path_t& directory, bool recursive = false ) const;

            /**
             * @brief Przenieś obiekt do innej lokalizacji
             */
            void move( const path_t& source, const path_t& target ) const;

            /**
             * @brief Kopiuj obiekt do innej lokalizacji
             */
            void copy( const path_t& source, const path_t& target ) const;

            /**
             * @brief Stwórz link.
             * @param soft jeśli `true`, tworzony jest link symboliczny, jeśli `false` to twardy
             */
            void symlink( const path_t& source, const path_t& target, bool soft = false ) const;

            /**
             * @brief Zmień uprawnienia obiektu.
             */
            void chmod( const path_t& entry, int privileges, bool recursive = false ) const;

            using ProgressMiddleware = std::function<void( qword_t current, qword_t all )>;

            /**
             * @brief Pobierz obiekt z zdalnego systemu plików na lokalny.
             * @param source ścieżka zdalnego obiektu.
             * @param target ścieżka docelowa na lokalnym systemie.
             * @param progress funkcja która będzie dostawać informacje o postępie transferu.
             * @note Jeśli obiekt to katalog to zostanie pobrany rekurencyjnie.
             */
            void get( const path_t& source, const path_t& target, std::optional<ProgressMiddleware> progress = std::nullopt ) const;

            /**
             * @brief Prześlij obiekt lokalny do zdalnego systemu plików.
             * @param source ścieżka lokalna obiektu.
             * @param target ścieżka docelowa na zdalnym systemie.
             * @param progress funkcja która będzie dostawać informacje o postępie transferu.
             * @note Jeśli obiekt to katalog to zostanie przesłany rekurencyjnie.
             */
            void put( const path_t& source, const path_t& target, std::optional<ProgressMiddleware> progress = std::nullopt ) const;

            /**
             * @brief Pobierz obiekt zdalny do lokalnego systemu plików ale asynchronicznie. Metoda jest wywoływana w oddzielnym połączeniu.
             * @param source ścieżka zdalna obiektu.
             * @param target ścieżka docelowa na lokalnym systemie.
             * @param progress funkcja która będzie dostawać informacje o postępie transferu.
             * @note Jeśli obiekt to katalog to zostanie pobrany rekurencyjnie.
             */
            std::future<void> aget( const path_t& source, const path_t& target, std::optional<ProgressMiddleware> progress = std::nullopt ) const;

            /**
             * @brief Prześlij obiekt lokalny do zdalnego systemu plików ale asynchronicznie. Metoda jest wywoływana w oddzielnym połączeniu.
             * @param source ścieżka lokalna obiektu.
             * @param target ścieżka docelowa na zdalnym systemie.
             * @param progress funkcja która będzie dostawać informacje o postępie transferu.
             * @note Jeśli obiekt to katalog to zostanie przesłany rekurencyjnie.
             */
            std::future<void> aput( const path_t& source, const path_t& target, std::optional<ProgressMiddleware> progress = std::nullopt ) const;


            remotefile_t open_file( const path_t& file, std::ios::openmode mode ) const;
            void close_file( remotefile_t file ) const;

            void read( remotefile_t file, char* data, qword_t size ) const;
            std::streamsize gcount( remotefile_t file ) const;

            void write( remotefile_t file, const char* data, qword_t size ) const;

            std::streampos tellg( remotefile_t file ) const;
            void seekg( remotefile_t file, std::streamoff offset, std::ios::seekdir dir ) const;
            void seekg( remotefile_t file, std::streampos position ) const;

            std::streampos tellp( remotefile_t file ) const;
            void seekp( remotefile_t file, std::streamoff offset, std::ios::seekdir dir ) const;
            void seekp( remotefile_t file, std::streampos position ) const;

            bool eof( remotefile_t file ) const;

            /**
             * @brief Otwórz zdalny plik w stylu std::fstream jako osobne połączenie.
             * @see RemoteFile.
             * @return Zwraca obiekt `RemoteFile`, który reprezentuje plik zdalny.
             */
            RemoteFile open_file_standalone( const path_t& file, std::ios::openmode mode ) const;

        
        private:

            /**
             * @brief Negocjuj komende z serwerem. Podstawa każdej innej komendy.
             * @throws Serwer może nie puścić komendy z różnych względów.
             */
            void negotiate_command( protocol::CommandList cmd ) const;

            net::IOSocket _sock;
            std::optional<Credentials> _credentials;
            net::Address _address;

    };


}