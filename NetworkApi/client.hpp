#include "..\..\JustAnotherLibs\lilLib\lib.hpp"
using namespace std;

#define DEFAULT_IP 18.196.139.169
#define DEFAULT_PORT 25565

// Client program example
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>


// TCP socket type
#define DEFAULT_PROTO SOCK_STREAM

string msg;

void Usage(char *progname) {
    fprintf(stderr,"Usage: %s -p [protocol] -n [server name/IP] -e [port_num] -l [iterations]\n", progname);
    fprintf(stderr,"Where:\n\tprotocol is one of TCP or UDP\n");
    fprintf(stderr,"\t- server is the IP address or name of server\n");
    fprintf(stderr,"\t- port_num is the port to listen on\n");
    fprintf(stderr,"\t- iterations is the number of loops to execute.\n");
    fprintf(stderr,"\t- (-l by itself makes client run in an infinite loop,\n");
    fprintf(stderr,"\t- Hit Ctrl-C to terminate it)\n");
    fprintf(stderr,"\t- The defaults are TCP , localhost and 2007\n");
    WSACleanup();

    exit(1);

}

void create_connection_to_server() {

    return;
}




int boot_client() {
    char Buffer[256];
    // default to localhost
    char *server_name= "18.196.139.169";
    unsigned short port = DEFAULT_PORT;
    printf("port: ");
    cin >> port;
    int retval, loopflag = 0;
    int i, loopcount, maxloop=-1;
    unsigned long addr;
    int socket_type = DEFAULT_PROTO;
    struct sockaddr_in server;
    struct hostent *hp;
    WSADATA wsaData;
    SOCKET  conn_socket;

    
    if ((retval = WSAStartup(0x202, &wsaData)) != 0) {
        fprintf(stderr,"Client: WSAStartup() failed with error %d\n", retval);
        WSACleanup();
        return -1;
    } else
       printf("Client: WSAStartup() is OK.\n");
    // Attempt to detect if we should call gethostbyname() or gethostbyaddr()
    if (isalpha(server_name[0])) {   // server address is a name
        hp = gethostbyname(server_name);
    } else { // Convert nnn.nnn address to a usable one
        addr = inet_addr(server_name);
        hp = gethostbyaddr((char *)&addr, 4, AF_INET);
    }
    if (hp == NULL ) {
        fprintf(stderr,"Client: Cannot resolve address \"%s\": Error %d\n", server_name, WSAGetLastError());
        WSACleanup();
        exit(1);
    } else
       printf("Client: gethostbyaddr() is OK.\n");
    // Copy the resolved information into the sockaddr_in structure
    memset(&server, 0, sizeof(server));
    memcpy(&(server.sin_addr), hp->h_addr, hp->h_length);
    server.sin_family = hp->h_addrtype;
    server.sin_port = htons(port);

    conn_socket = socket(AF_INET, socket_type, 0); /* Open a socket */
    if (conn_socket <0 ) {
        fprintf(stderr,"Client: Error Opening socket: Error %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    } else
       printf("Client: socket() is OK.\n");

 

    // Notice that nothing in this code is specific to whether we
    // are using UDP or TCP.
    // We achieve this by using a simple trick.
    //    When connect() is called on a datagram socket, it does not
    //    actually establish the connection as a stream (TCP) socket
    //    would. Instead, TCP/IP establishes the remote half of the
    //    (LocalIPAddress, LocalPort, RemoteIP, RemotePort) mapping.
    //    This enables us to use send() and recv() on datagram sockets,
    //    instead of recvfrom() and sendto()

    printf("Client: Client connecting to: %s.\n", hp->h_name);
    if (connect(conn_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        fprintf(stderr,"Client: connect() failed: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    } else
       printf("Client: connect() is OK.\n");

    // Test sending some string
    loopcount = 0;
    while(1) {
        
        //SENDING TO SERVER ************************************************************
        msg="okidoki this is custom test!";
        for (int x=0; x<msg.length(); x++) {
            Buffer[x]=msg[x];
        }

        //wsprintf(Buffer,"This is a test message from client #%d", loopcount++);
        retval = send(conn_socket, Buffer, 512/*sizeof(Buffer)*/, 0);
        if (retval == SOCKET_ERROR) {
            fprintf(stderr,"Client: send() failed: error %d.\n", WSAGetLastError());
            WSACleanup();
            return -1;
        } else
          printf("Client: send() is OK.\n");
        printf("Client: Sent data \"%s\"\n", Buffer);
        retval = recv(conn_socket, Buffer, sizeof(Buffer), 0);
        if (retval == SOCKET_ERROR) {
            fprintf(stderr,"Client: recv() failed: error %d.\n", WSAGetLastError());
            closesocket(conn_socket);
            WSACleanup();
            return -1;
        } else
            printf("Client: recv() is OK.\n");

        // We are not likely to see this with UDP, since there is no
        // 'connection' established.

        if (retval == 0) {
            printf("Client: Server closed connection.\n");
            closesocket(conn_socket);
            WSACleanup();

            return -1;
        }

        printf("Client: Received %d bytes, data \"%s\" from server.\n", retval, Buffer);
        if (!loopflag) {
            printf("Client: Terminating connection...\n");
            break;
        } else {
            if ((loopcount >= maxloop) && (maxloop >0))
            break;
        }
    }
    closesocket(conn_socket);
    WSACleanup();
    return 0;
}