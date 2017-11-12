
all: client

client: client_mt.o clientMT.o
	gcc -o client client_mt.o clientMT.o

client_mt.o: client_mt.c
	gcc -o client_mt.o -c client_mt.c -W -Wall -ansi -pedantic -std=c99

clientMT.o: clientMT.c client.h
	gcc -o clientMT.o -c clientMT.c -W -Wall -ansi -pedantic -std=c99

clean:
	rm -rf *.o

mrproper: clean
	rm -rf client
			