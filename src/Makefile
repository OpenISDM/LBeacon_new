# LBeacon
#---------------------------------------------------------------------------
CC = gcc -std=gnu99 
OBJS = Utilities.o LinkedList.o Mempool.o thpool.o Communication.o xbee_Serial.o xbee_API.o pkt_Queue.o LBeacon.o
LIB = -L /usr/local/lib

#---------------------------------------------------------------------------
all: clean LBeacon
LBeacon: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o LBeacon $(LIB) -lrt -lpthread -lmulticobex -lbfb -lbluetooth -lobexftp -lopenobex -lxbee -lwiringPi -lzlog
LBeacon.o: LBeacon.c LBeacon.h
	$(CC) LBeacon.c LBeacon.h $(LIB) -c
Utilities.o: Utilities.c Utilities.h
	$(CC) Utilities.c Utilities.h  $(LIB) -c
LinkedList.o: LinkedList.c LinkedList.h
	$(CC) LinkedList.c LinkedList.h $(LIB) -c
pkt_Queue.o: pkt_Queue.c pkt_Queue.h
	$(CC) pkt_Queue.c pkt_Queue.h -c
xbee_Serial.o: xbee_Serial.c xbee_Serial.h
	$(CC) xbee_Serial.c xbee_Serial.h -c
xbee_API.o: xbee_API.c xbee_API.h
	$(CC) xbee_API.c xbee_API.h -c
Mempool.o: Mempool.c Mempool.h
	$(CC) Mempool.c Mempool.h $(LIB) -c
Thpool.o: thpool.c thpool.h
	$(CC) thpool.c thpool.h $(LIB) -c
Communication.o: Communication.c Communication.h
	$(CC) Communication.c Communication.h -c

clean:
	find . -type f | xargs touch
	@rm -rf *.o *.h.gch *.log *.log.0 LBeacon
