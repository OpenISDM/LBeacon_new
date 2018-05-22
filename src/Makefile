# LBeacon
#---------------------------------------------------------------------------
CC = gcc -std=gnu99
OBJS = Utilities.o LinkedList.o pkt_Queue.o xbee_API.o LBeacon.o 
CFLAGS = -g
LIB = -L/usr/local/lib
LIBXBEE = ../libxbee3/lib/

#---------------------------------------------------------------------------
all: LBeacon
LBeacon: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o LBeacon $(LIB) -lrt -lpthread -lmulticobex -lbfb -lbluetooth -lobexftp -lopenobex -lxbee
LBeacon.o: LBeacon.c LBeacon.h
	$(CC) LBeacon.c $(CFLAGS) $(LIB) -c
Utilities.o: Utilities.c Utilities.h
	$(CC) Utilities.c $(CFLAGS) $(LIB) -c
LinkedList.o: LinkedList.c LinkedList.h
	$(CC) LinkedList.c $(CFLAGS) $(LIB) -c
pkt_Queue.o: pkt_Queue.c pkt_Queue.h
	$(CC) pkt_Queue.c pkt_Queue.h $(LIBXBEE) -lxbee -c
xbee_API.o: xbee_API.c xbee_API.h
	$(CC) xbee_API.c xbee_API.h  $(LIBXBEE) -lxbee -c
clean:
	@rm -rf *.o
