# LBeacon
#---------------------------------------------------------------------------
CC = gcc -std=gnu99
OBJS = LinkedList.o Mempool.o thpool.o Communication.o UDP_API.o pkt_Queue.o BeDIS.o LBeacon.o
LIB = -L /usr/local/lib

#---------------------------------------------------------------------------
all: LBeacon
LBeacon: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o LBeacon $(LIB) -lrt -lpthread -lbfb -lbluetooth -lwiringPi -lzlog 
	@mv LBeacon ../bin/
	chown pi:pi ../bin/LBeacon

LBeacon.o: LBeacon.c LBeacon.h
	$(CC) LBeacon.c LBeacon.h $(LIB) -c
BeDIS.o: BeDIS.c BeDIS.h
	$(CC) $(CFLAGS) BeDIS.c -c
LinkedList.o: LinkedList.c LinkedList.h
	$(CC) LinkedList.c LinkedList.h $(LIB) -c
pkt_Queue.o: pkt_Queue.c pkt_Queue.h
	$(CC) pkt_Queue.c pkt_Queue.h -c
UDP_API.o: UDP_API.c UDP_API.h pkt_Queue.o
	$(CC) $(CFLAGS) UDP_API.c -c
Mempool.o: Mempool.c Mempool.h
	$(CC) Mempool.c Mempool.h $(LIB) -c
Thpool.o: thpool.c thpool.h
	$(CC) thpool.c thpool.h $(LIB) -c
Communication.o: Communication.c Communication.h
	$(CC) Communication.c Communication.h -c

clean:
	find . -type f | xargs touch
	@rm -rf *.o *.h.gch *.log *.log.0 *.txt LBeacon
