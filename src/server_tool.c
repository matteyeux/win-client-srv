#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <include/server_tool.h>
#include <include/common.h>

#pragma comment(lib, "ws2_32.lib") // Winsock Library
 
int tcp_server(int port){
    WSADATA wsa;
    SOCKET master , new_socket, client_socket[30], s;
    struct sockaddr_in server, address;
    int max_clients = 30, activity, addrlen, i, valread;
    char *message = "Server message\r\n";
     
    // receive buffer size
    int MAXRECV = 1024;
    // set of socket descriptors
    fd_set readfds;
    // 1 extra for null character, string termination
    char *buffer;
    buffer =  (char*) malloc((MAXRECV + 1) * sizeof(char));
 
    for(i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }
 
    fprintf(stdout, "\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        fprintf(stderr, "Failed. Error Code : %d\n" ,WSAGetLastError());
        exit(EXIT_FAILURE);
    }
     
    fprintf(stdout, "Initialised\n");
     
    // Create socket
    if((master = socket(AF_INET, SOCK_STREAM, 0 )) == INVALID_SOCKET)
    {
        fprintf(stderr, "Could not create socket : %d\n" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
 
    fprintf(stdout, "Socket created\n");
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
     
    //Bind
    if( bind(master ,(struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        fprintf(stderr, "Bind failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
     
    debug("Bind done\n");
 
    //Listen to incoming connections
    listen(master, 3);
     
    //Accept and incoming connection
    debug("Waiting for incoming connections...");
     
    addrlen = sizeof(struct sockaddr_in);
     
    while(TRUE)
    {
        //clear the socket fd set
        FD_ZERO(&readfds);
  
        //add master socket to fd set
        FD_SET(master, &readfds);
         
        //add child sockets to fd set
        for ( i = 0; i < max_clients; i++) 
        {
            s = client_socket[i];
            if(s > 0)
            {
                FD_SET( s, &readfds);
            }
        }
         
        //wait for an activity on any of the sockets, timeout is NULL , so wait indefinitely
        activity = select( 0, &readfds, NULL, NULL, NULL);
    
        if ( activity == SOCKET_ERROR ) 
        {
            fprintf(stderr, "select call failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
          
        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(master, &readfds)) 
        {
            if ((new_socket = accept(master, (struct sockaddr *)&address, (int *)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
          
            //inform user of socket number - used in send and receive commands
            fprintf(stdout, "New connection, socket fd is %lld, ip is : %s, port : %d \n", (__int64)new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        
            //send new connection greeting message
            if( send(new_socket, message, strlen(message), 0) != strlen(message)) 
            {
                perror("send failed");
            }              
            debug("Welcome message sent successfully");
            fprintf(stdout, "%s\n",message);
            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++) 
            {
                if (client_socket[i] == 0)
                {
                    client_socket[i] = new_socket;
                    fprintf(stdout, "Adding to list of sockets at index %d \n", i);
                    break;
                }
            }
        }

        // else its some IO operation on some other socket :)
        for (i = 0; i < max_clients; i++) 
        {
            s = client_socket[i];
            //if client presend in read sockets             
            if (FD_ISSET( s, &readfds)) 
            {
                //get details of the client
                getpeername(s, (struct sockaddr*)&address, (int*)&addrlen);
                //Check if it was for closing , and also read the incoming message
                //recv does not place a null terminator at the end of the string (whilst printf %s assumes there is one).
                valread = recv( s, buffer, MAXRECV, 0);
                if( valread == SOCKET_ERROR)
                {
                    int error_code = WSAGetLastError();
                    if(error_code == WSAECONNRESET)
                    {
                        //Somebody disconnected , get his details and print
                        fprintf(stderr, "Host disconnected unexpectedly, ip %s, port %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                      
                        //Close the socket and mark as 0 in list for reuse
                        closesocket(s);
                        client_socket[i] = 0;
                    }
                    else
                    {
                        fprintf(stderr, "recv failed with error code : %d", error_code);
                    }
                }
                if ( valread == 0)
                {
                    //Somebody disconnected , get his details and print
                    fprintf(stderr, "Host disconnected, ip %s, port %d\n" ,inet_ntoa(address.sin_addr) ,ntohs(address.sin_port));
                      
                    //Close the socket and mark as 0 in list for reuse
                    closesocket(s);
                    client_socket[i] = 0;
                }
                  
                //Echo back the message that came in
                else
                {
                    //add null character, if you want to use with printf/debug or other string handling functions
                    buffer[valread] = '\0';
                    fprintf(stdout, "%s:%d - %s\n" , inet_ntoa(address.sin_addr), ntohs(address.sin_port), buffer);
                    send(s, buffer, valread, 0);
                }
            }
        }
    }
     
    closesocket(s);
    WSACleanup();
}
