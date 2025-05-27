import socket
import threading
import base64


EXPECTED_AUTH = "sfap:isthebest"
EXPECTED_HEADER = "Proxy-Authorization"

EXPECTED_B64 = base64.b64encode( EXPECTED_AUTH.encode() ).decode()


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
        request = client.recv( 4096 ).decode( errors='ignore' )
        lines = request.split( "\r\n" )
        if not lines or not lines[0].startswith( "CONNECT" ):
            raise Exception( "Invalid request" )

        _, target, _ = lines[0].split( " ", 2 )
        host, port = target.split( ":" )
        port = int( port )

        authorized = False
        for line in lines[1:]:
            if line.lower().startswith( EXPECTED_HEADER.lower() + ":" ):
                value = line.split( ":", 1 )[1].strip()
                if value.startswith( "Basic " ):
                    encoded = value[6:]
                    if encoded == EXPECTED_B64:
                        authorized = True
                        break

        if not authorized:
            client.sendall( b"HTTP/1.1 407 Proxy Authentication Required\r\n" )
            client.sendall( b"Proxy-Authenticate: Basic realm=\"SFAP\"\r\n\r\n" )
            return

        remote = socket.create_connection( ( host, port ) )
        client.sendall( b"HTTP/1.1 200 Connection established\r\n\r\n" )

        t1 = threading.Thread( target=forward, args=( client, remote ) )
        t2 = threading.Thread( target=forward, args=( remote, client) )
        t1.start()
        t2.start()
        t1.join()
        t2.join()

    except Exception as e:
        try:
            client.sendall( b"HTTP/1.1 502 Bad Gateway\r\n\r\n" )
        except:
            pass
        print( "Error:", e )
    finally:
        client.close()
        if remote:
            remote.close()


server = socket.socket()
server.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1 )
server.bind( ( "127.0.0.1", 42001 ) )
server.listen( 5 )
print( "HTTP CONNECT server listening on 127.0.0.1:42001..." )

while True:
    client, _ = server.accept()
    threading.Thread( target=handle_client, args=( client, ), daemon=True ).start()
