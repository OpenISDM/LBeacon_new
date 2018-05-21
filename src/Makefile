# LBeacon
#---------------------------------------------------------------------------
CC = gcc
OBJS = LBeacon.o Utilities.o LinkedList.o Queue.o
CFLAGS = -g
LIB = -L/usr/local/lib

#---------------------------------------------------------------------------
all: LBeacon
LBeacon: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o LBeacon $(LIB) -lrt -lpthread -lmulticobex -lbfb -lbluetooth -lobexftp -lopenobex
LBeacon.o: LBeacon.c LBeacon.h
	$(CC) LBeacon.c $(CFLAGS) $(LIB) -c
Utilities.o: Utilities.c Utilities.h
	$(CC) Utilities.c $(CFLAGS) $(LIB) -c
LinkedList.o: LinkedList.c LinkedList.h
	$(CC) LinkedList.c $(CFLAGS) $(LIB) -c
Queue.o: Queue.c Queue.h
	$(CC) Queue.c $(CFLAGS) $(LIB) -c
clean:
	@rm -rf *.o
