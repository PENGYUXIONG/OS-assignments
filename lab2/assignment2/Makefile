
CFLAGS = -Wall -O2

prodcon: main.o queue.o thread.o tands.o
	gcc -o prodcon main.o queue.o thread.o tands.o -lpthread

main: main.c header.h
	gcc $(CFLAGS) -c main.c -o main -lpthread 

queue: queue.c header.h
	gcc $(CFLAGS) -c queue.c -o queue -lpthread 

thread: thread.c header.h
	gcc $(CFLAGS) -c thread.c -o thread -lpthread

tands: tands.c
	gcc $(CFLAGS) -c tands.c -o tands

clean:
	rm prodcon main.o queue.o thread.o tands.o

debug: main.c queue.c thread.c tands.c header.h
	gcc -Wall -c -g main.c
	gcc -Wall -c -g queue.c
	gcc -Wall -c -g thread.c
	gcc -Wall -c -g tands.c
	gcc -o prodcon -g main.o queue.o thread.o tands.o -lpthread