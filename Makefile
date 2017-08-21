CC = cl
DEBUG ?=
DBG = -DDEBUG
# ifeq ($(DEBUG), 1)
# 	DBG = -DDEBUG
# endif

all : default

default : server.exe client.exe

client.exe: src/client.c src/client_tool.c src/common.c
	$(CC) src/client.c src/client_tool.c src/common.c /I. $(DBG)

server.exe: src/server.c src/server_tool.c src/common.c
	$(CC) src/server.c src/server_tool.c src/common.c /I. $(DBG)

clean : 
	del server.exe client.exe server.obj server_tool.obj common.obj