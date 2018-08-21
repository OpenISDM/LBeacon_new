# LBeacon
#---------------------------------------------------------------------------
CC = gcc -std=gnu99
OBJS = Utilities.o LinkedList.o pkt_Queue.o xbee_Serial.o xbee_API.o Mempool.o LBeacon.o thpool.o Communication.o 
LIB = -L/usr/local/lib

#---------------------------------------------------------------------------
all: LBeacon
LBeacon: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o LBeacon $(LIB) -lrt -lpthread -lmulticobex -lbfb -lbluetooth -lobexftp -lopenobex -lxbee -lwiringPi -lzlog
LBeacon.o: LBeacon.c LBeacon.h
	$(CC) LBeacon.c $(CFLAGS) $(LIB) -c
Utilities.o: Utilities.c Utilities.h
	$(CC) Utilities.c $(CFLAGS) $(LIB) -c
LinkedList.o: LinkedList.c LinkedList.h
	$(CC) LinkedList.c $(CFLAGS) $(LIB) -c
pkt_Queue.o: pkt_Queue.c pkt_Queue.h
	$(CC) pkt_Queue.c pkt_Queue.h -lxbee -c
xbee_Serial.o: xbee_Serial.c xbee_Serial.h
	$(CC) xbee_Serial.c xbee_Serial.h -c
xbee_API.o: xbee_API.c xbee_API.h
	$(CC) xbee_API.c xbee_API.h  -lxbee -c
Mempool.o: Mempool.c Mempool.h
	$(CC) Mempool.c $(CFLAGS) $(LIB) -c
Thpool.o: thpool.c thpool.h
	$(CC) thpool.c $(CFLAGS) $(LIB) -c
Communication.o: Communication.c Communication.h
	$(CC) Communication.c  -lxbee -c
clean:
	find . -type f | xargs touch 
	@rm -rf *.o
