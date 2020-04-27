# LBeacon
#---------------------------------------------------------------------------
CC = gcc -std=gnu99 -O3
OBJS = LinkedList.o Mempool.o thpool.o UDP_API.o pkt_Queue.o BeDIS.o LBeacon.o
LIB = -L /usr/local/lib -L /home/bedis/bot-encrypt 
INC = -I ../import -I ../import/libEncrypt

#---------------------------------------------------------------------------
all: LBeacon
LBeacon: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o LBeacon $(INC) $(LIB) -lrt -lpthread -lbluetooth -lzlog -lEncrypt
	@mv LBeacon ../bin/
	chown bedis:bedis ../bin/LBeacon

LBeacon.o: 
	$(CC) LBeacon.c LBeacon.h $(INC) $(LIB) -c
BeDIS.o: 
	$(CC) $(CFLAGS) ../import/BeDIS.c  -c
LinkedList.o:  
	$(CC) ../import/LinkedList.c $(LIB) -c
pkt_Queue.o: 
	$(CC) ../import/pkt_Queue.c  -c
UDP_API.o: 
	$(CC) $(CFLAGS) ../import/UDP_API.c $(INC) -c 
Mempool.o: 
	$(CC) ../import/Mempool.c  $(LIB) -c
thpool.o: 
	$(CC) ../import/thpool.c  $(LIB) -c
clean:
	find . -type f | xargs touch
	@rm -rf *.o *.h.gch *.log *.log.0 *.txt LBeacon
