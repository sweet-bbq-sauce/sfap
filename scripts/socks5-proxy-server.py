import socket
import threading


USERNAME = b"sfap"
PASSWORD = b"isthebest"


def recv_all( sock, size ):
    data = b''
    while len( data ) < size:
        chunk = sock.recv( size - len( data ) )
        if not chunk:
            raise ConnectionError( "Incomplete recv" )
        data += chunk
    return data


def forward( src, dst ):
    try:
        while True:
            data = src.recv( 4096 )
            if not data:
                break
            dst.sendall( data )
    except:
        pass
    finally:
        try: src.shutdown( socket.SHUT_RD )
        except: pass
        try: dst.shutdown( socket.SHUT_WR )
        except: pass


def handle_client( client ):
    remote = None
    try:
        # handshake
        ver, nmethods = recv_all( client, 2 )
        methods = recv_all( client, nmethods )
        if 2 not in methods:
            client.sendall( b"\x05\xFF" )   # no acceptable method
            return
        client.sendall( b"\x05\x02" )       # username/password

        # auth
        ver = recv_all( client, 1 )
        ulen = recv_all( client, 1 )[0]
        uname = recv_all( client, ulen )
        plen = recv_all( client, 1 )[0]
        passwd = recv_all( client, plen )
        if uname != USERNAME or passwd != PASSWORD:
            client.sendall( b"\x01\x01" )   # failure
            return
        client.sendall( b"\x01\x00" )       # success

        # request
        ver, cmd, rsv, atyp = recv_all( client, 4 )
        if cmd != 1:
            client.sendall( b"\x05\x07\x00\x01" + b"\x00"*6 )   # command not supported
            return

        if atyp == 1:       # IPv4
            addr = socket.inet_ntoa( recv_all( client, 4 ) )
        elif atyp == 3:     # domain
            alen = recv_all( client, 1 )[0]
            addr = recv_all( client, alen ).decode()
        else:
            client.sendall( b"\x05\x08\x00\x01" + b"\x00"*6 )   # address type not supported
            return

        port = int.from_bytes( recv_all( client, 2 ), 'big' )

        remote = socket.create_connection( ( addr, port ) )
        bind_addr = remote.getsockname()
        client.sendall( b"\x05\x00\x00\x01" + socket.inet_aton( bind_addr[0] ) + bind_addr[1].to_bytes( 2, 'big' ) )

        # tunneling
        t1 = threading.Thread( target=forward, args=( client, remote ) )
        t2 = threading.Thread( target=forward, args=( remote, client ) )
        t1.start()
        t2.start()
        t1.join()
        t2.join()

    except Exception as e:
        print( "Error:", e )
    finally:
        client.close()
        if remote:
            remote.close()


# Server start
server = socket.socket()
server.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1 )
server.bind( ( "127.0.0.1", 42000 ) )
server.listen( 5 )
print( "SOCKS5 server listening on 127.0.0.1:42000..." )

while True:
    client, _ = server.accept()
    threading.Thread( target=handle_client, args=( client, ), daemon=True ).start()
