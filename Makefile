CC=g++
INC= -I/opt/dis/include -I/opt/dis/include/dis
LIB= -L/opt/dis/lib64 -lsisci

all: client server

dolphin-server.o: dolphin-server.hpp dolphin-server.cpp
	g++ -g -Wall $(INC) dolphin-server.cpp -c -o dolphin-server.o

server-main.o: server_main.cpp
	g++ -g -Wall $(INC) server_main.cpp -c -o server-main.o

dolphin-client.o: dolphin-client.hpp dolphin-client.cpp
	g++ -g -Wall $(INC) dolphin-client.cpp -c -o dolphin-client.o

client-main.o: client_main.cpp
	g++ -g -Wall $(INC) client_main.cpp -c -o client-main.o

client: dolphin-client.o client-main.o
	g++ -o client -g dolphin-client.o client-main.o $(LIB)
	
server: dolphin-server.o server-main.o
	g++ -o server -g dolphin-server.o server-main.o $(LIB) 

clean:
	rm -f *.o client server
