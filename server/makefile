
all: server

server: server_mt.o server_multithread.o
	gcc -o server server_mt.o server_multithread.o -lpthread

server_mt.o: server_mt.c
	gcc -o server_mt.o -c server_mt.c -W -Wall -ansi -pedantic -std=c99 -lpthread

server_multithread.o: server_multithread.c server.h
	gcc -o server_multithread.o -c server_multithread.c -W -Wall -ansi -pedantic -std=c99

clean:
	rm -rf *.o

mrproper: clean
	rm -rf server
			
