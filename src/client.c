/*
	This a TCP client
*/ 
#include <stdio.h>
#include <winsock2.h>
#include <string.h>

#include <include/client_tool.h>
#include <include/common.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

void usage(int argc , char *argv[]){
    fprintf(stdout, "usage : %s <ip> [port] [message]\n", argv[0]);
    fprintf(stdout, "-i, --ip <ip>\t\t set ip address of the server, you can also set a hostname\n");
    fprintf(stdout, "-p, --port [port]\t set socket port\n");
    fprintf(stdout, "-m --message [message]\t set message to send\n");
}

int main(int argc , char *argv[])
{      
    char *ip = "127.0.0.1";
    char *message_to_send = NULL;
    int port = 8888;
    int i = 0;
    if (argc != 1 && argc > 7){
        usage(argc, argv);
        return 0;
    }

    for(i; i < argc; i++) {
        if(!strcmp(argv[i], "-i") || !strcmp(argv[i], "--ip")) {
            ip = argv[i + 1];
        } else if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")) {
            port = atoi(argv[i + 1]);
        } else if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--message"))
            message_to_send = argv[i+1];
        else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
            usage(argc, argv);
            return 0;
        }
    }
    tcp_client(ip, 8888, message_to_send);
    return 0;
}