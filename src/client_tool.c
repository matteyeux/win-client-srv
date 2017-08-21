#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <include/client_tool.h>
#include <include/common.h>
 
#pragma comment(lib,"ws2_32.lib") // Winsock lin

int tcp_client(char *ipaddr, int port, char *message){
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
    
    struct hostent *remoteHost;
    struct in_addr addr;

    char server_reply[2000]; // *message
    int recv_size;

    debug("Initialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2),&wsa) != 0)
    {
        fprintf(stderr, "Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }

    debug("Initialised\n");
     
    // Create a socket
    if((s = socket(AF_INET, SOCK_STREAM, 0 )) == INVALID_SOCKET)
    {
        fprintf(stdout, "Could not create socket : %d" , WSAGetLastError());
        return 1;
    }
    debug("Socket created.\n");

    remoteHost = gethostbyname(ipaddr);
    addr.s_addr = *(u_long *) remoteHost->h_addr_list[0];
    fprintf(stdout, "IP Address : %s\n", inet_ntoa(addr));
    

    server.sin_addr.s_addr = inet_addr(inet_ntoa(addr));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
 
    // Connect to remote server
    if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        fprintf(stderr, "connect error\n");
        return 1;
    }
    
    fprintf(stdout, "connected\n");
     
    // Send some data
    if (message == NULL)
    {
        message = getenv("USERPROFILE");
    }
    if(send(s, message, strlen(message), 0) < 0)
    {
        fprintf(stderr, "send() failed bruh\n");
        return 1;
    }
    debug("data sent\n");
     
    // Receive a reply from the server
    if((recv_size = recv(s, server_reply, 2000, 0)) == SOCKET_ERROR)
    {
        fprintf(stderr, "recv failed");
    }
    debug("reply received\n");
    // Add a NULL terminating character to make it a proper string before printing
    server_reply[recv_size] = '\0';
    fprintf(stdout, "Server message : %s", server_reply);
    return 0;
}