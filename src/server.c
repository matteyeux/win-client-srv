#include <stdio.h>
#include <winsock2.h>
#include <include/server_tool.h>

int main(int argc , char *argv[])
{   
    tcp_server(8888);
    return 0;
}