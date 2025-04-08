# Simple Filesystem Access Protocol

## Introduction
**libSFAP** is a **cross-platform**, **modern C++** library, protocol, and toolset for implementing a network protocol that extends functionality similar to **SFTP** or **SCP**. The main goals of the project are:
- Lightweight code
- Intuitive and fast API
- Rich functionality
- Easy implementation
- Modular capability
- Ready-to-use end applications:
  - **sfapd** - daemon
  - **sfap** - CLI client
  - **sfap-gui** - FLTK-based GUI client
  - **sfapfs** - mount remote filesystem using **libFUSE** and **WinFsp**
- Minimal required libraries for core functionality (**libSSL**, **zlib**, and optionally **libMagic**)

The project is new and very raw. Currently implemented features:

- [x] **libSSL** (connection encryption, certificate verification)
- [x] Low-level network layer (I/O socket, listener, connector, proxy, DNS)
- [x] High-level classes (`Client`, `Server`)
- [x] Command system
- [x] Client connection creates a server-side session
- [x] Server creates a virtual filesystem for client navigation
- [x] Middlewares (`ConnectionMiddleware`, `LoginMiddleware`, `CommandMiddleware`) for access control
- [x] User commands (`LOGIN`, `WHOAMI`, `HOME`, `CD`, `PWD`, etc.)
- [x] Commands for remote filesystem information (`LS`, `SPACE`, `EXISTS`, etc.)
- [ ] Filesystem manipulation commands (`MKDIR`, `TOUCH`, `RM`, `COPY`, etc.)
- [ ] Remote file I/O commands (`OPEN`, `CLOSE`, `WRITE`, `SEEKG`, etc.)
- [ ] Administrator commands
- [x] Server can provide extended file information via **libMagic**
- [ ] SOCKS5 and HTTP CONNECT proxy support (even chained)
- [ ] `RemoteFile` - std::fstream-like object representing remote files

## Building
Library compilation requires `libssl-dev`, `zlib-dev`, and `libmagic-dev`.
Building is simple using **CMake**:
- Clone the **SFAP** repository
- Enter the repo root directory
- `mkdir build`
- `cd build`
- `cmake ..`
- `ninja` / `make`

## Example
See below **API** > **High-level classes**

## How it works?

### Communication Model
The **SFAP** protocol is a binary command-based protocol. The client sends commands to the server, which executes procedures and returns results. Commands can be executed sequentially over the same connection.
A key element is the **session** created by the server for each client. The session contains:
- **User** and **logged** flag - commands execute on behalf of a user or anonymous user (see *LoginMiddleware*, *CommandMiddleware*)
- **Root path** - host directory accessible to the client (chroot-like)
- **Current working path** - client can navigate using **cd**, **pwd**
- **List of currently open I/O files** - client can request server to open local files for streaming I/O
- **Reference to parent server**

Connection establishment flow:
1. Client connects to server (SSL handshake occurs if enabled)
2. Server uses `ConnectionMiddleware` to accept/reject connection
3. Server creates a session for the client

Subsequent communication follows a command-result loop:
1. Server waits for command
2. Client sends command ID
3. Server uses `CommandMiddleware` to accept/reject command
4. If accepted, command procedure executes (client sends args, server returns result)
5. After completion, server returns to step 1

If rejected, server returns to step 1 after sending rejection.

### Communication Errors
Since communication is binary and synchronous, both sides must precisely understand the context of each byte received. Implementation errors in command procedures may cause **desynchronization** (e.g., binary data misinterpreted as text). Some protocol sections include watchdogs to detect such errors, which force disconnect when triggered.

## API
All `libSFAP` code resides in the `sfap` namespace, with sub-namespaces:
- `net` - low-level networking
- `crypto` - SSL and Crypto
- `utils` - utilities
- `protocol` - communication model

`libSFAP` uses aliases like:
- `byte_t`, `word_t`, `dword_t`, `qword_t` - network communication helpers
- `path_t` - alias for `std::filesystem::path`
- `data_t` - alias for `std::vector<byte_t>`

### High-level Classes
Core client/server implementation.

**Server** is multi-threaded (listener runs in separate thread, each connection has dedicated thread). Configurable:
- Listen address
- SSL context (optional)
- Middlewares (optional)
- libMagic context (optional)

```cpp
// Server
#include <server/server.hpp>
using namespace sfap;

int main() {
    init(); // Initialize OpenSSL/WinSock2 on Windows
    
    try {
        // Without SSL
        Server server(net::Address(net::Host("0.0.0.0:6767")));

        // With SSL
        auto ssl = std::make_shared<crypto::SSLContext>("../cert/key.pem", "../cert/cert.pem");
        Server server(net::Address(net::Host("0.0.0.0:6767")), ssl);

        server.join(); // Block main thread
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

```

**Client** can send commands after successful connection (and optional login). API resembles **Bash**.

```cpp
#include <client/client.hpp>
using namespace sfap;

int main() {

    sfap::init();
    
    try {
        // Without SSL
        auto client = Client(net::Address("somehost.net:6767"));

        // With SSL
        auto client = Client(net::Address("somehost.net:6767", 
                            crypto::SSLContext::default_client_context));

        client.login({"login", "password"});
        std::cout << client.pwd() << std::endl;
        client.mkdir("folder1");
        client.cd("folder1");
        std::cout << client.pwd() << std::endl;
        
        for (const auto& entry : client.ls("..")) {
            std::cout << "Entry: " << entry.path << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
```

### Middlewares
Middlewares are the core access control mechanism. `libSFAP` provides base classes for:
 - **Firewall** (`ConnectionMiddleware`) - Accept/reject connections
 - **Command authorization** (`CommandMiddleware`) - Verify user permissions
 - **User login** (`LoginMiddleware`) - Handle authentication and set root path
 
The library doesn't implement these directly - developers must customize them (e.g., for predefined users, database integration, or system user mapping). Default middlewares accept all connections/commands and authenticate with any credentials, setting root path to the system user's home directory.