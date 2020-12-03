CFLAGS = -Wall -O2

build: server client
	
server: server.o tands.o getTime.o
	gcc -o server server.o tands.o getTime.o

client: client.o tands.o getTime.o
	gcc -o client client.o tands.o getTime.o

server.o: server.c header.h
	gcc $(CFLAGS) -c server.c -o server.o

client.o: client.c header.h
	gcc $(CFLAGS) -c client.c -o client.o

tands.o: tands.c header.h
	gcc $(CFLAGS) -c tands.c -o tands.o

getTime.o: getTime.c
	gcc $(CFLAGS) -c getTime.c -o getTime.o

clientMan:
	groff -man -Tpdf < client.man > client.pdf

serverMan:
	groff -man -Tpdf < server.man > server.pdf

clean:
	rm server client *.o *.pdf