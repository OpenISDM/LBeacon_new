/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and conditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIPS

  File Description:

     This file contains the program to connect to Wi-Fi and in the project, we
     use it for data transmission most.

  File Name:

     UDP_API.c

  Abstract:

     BeDIS uses LBeacons to deliver 3D coordinates and textual descriptions of
     their locations to users' devices. Basically, a LBeacon is an inexpensive,
     Bluetooth Smart Ready device. The 3D coordinates and location description
     of every LBeacon are retrieved from BeDIS (Building/environment Data and
     Information System) and stored locally during deployment and maintenance
     times. Once initialized, each LBeacon broadcasts its coordinates and
     location description to Bluetooth enabled user devices within its coverage
     area.

  Authors:
     Gary Xiao		, garyh0205@hotmail.com
 */
#include "UDP_API.h"


int udp_initial(pudp_config udp_config){

    int ret;

    // zero out the structure
    memset((char *) &udp_config -> si_server, 0, sizeof(udp_config
            -> si_server));

    if(ret = init_Packet_Queue( &udp_config -> pkt_Queue) != pkt_Queue_SUCCESS)

        return ret;

    if(ret = init_Packet_Queue( &udp_config -> Received_Queue)
       != pkt_Queue_SUCCESS)

        return ret;

    //create a send UDP socket
    if ((udp_config -> send_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
         == -1)
        perror("socket errror.\n");

    //create a recv UDP socket
    if ((udp_config -> recv_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
         == -1)
        perror("socket errror.\n");

    struct timeval timeout;
    timeout.tv_sec = UDP_SELECT_TIMEOUT; //秒

    if (setsockopt(udp_config -> recv_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout
                 , sizeof(timeout)) == -1)
        perror("setsockopt failed:");

    udp_config -> si_server.sin_family = AF_INET;
    udp_config -> si_server.sin_port = htons(UDP_LISTEN_PORT);
    udp_config -> si_server.sin_addr.s_addr = htonl(INADDR_ANY);

    udp_config -> shutdown = false;

    //bind recv socket to port
    if( bind(udp_config -> recv_socket , (struct sockaddr *)&udp_config ->
             si_server, sizeof(udp_config -> si_server) ) == -1)
        perror("bind error.\n");

    pthread_create(&udp_config -> udp_receive, NULL, udp_recv_pkt, (void*)
                   udp_config);

    pthread_create(&udp_config -> udp_send, NULL, udp_send_pkt, (void*)
                   udp_config);

    return 0;

}

int udp_addpkt(pkt_ptr pkt_queue, char *raw_addr, char *content, int size){

    if(size > WIFI_MESSAGE_LENGTH)
        return E_ADDPKT_OVERSIZE;

    const int UDP = 3;

    char address[NETWORK_ADDR_LENGTH];

    memset(&address, 0, NETWORK_ADDR_LENGTH);

    //Record current filled Address Location.
    int address_loc = 0;

    // Four part in a address.(devided by '.')
    for(int n = 0; n < 4; n++){

        //in each part, at most 3 number.
        int count = 0;
        unsigned char tmp[3];

        memset(&tmp, 0, sizeof(char) * 3);

        while(count != 3){

            //When read '.' from address_loc means the end of this part.
            if (raw_addr[address_loc] == '.'){

                address_loc ++;
                break;

            }
            else{
                tmp[count] = raw_addr[address_loc];

                count ++;
                address_loc ++;

                if(address_loc >= strlen(raw_addr))
                    break;

            }

        }

        for(int lo = 0; lo < 3;lo ++)

            if ((3 - count) > lo )
                address[n * 3 + lo] = '0';
            else
                address[n * 3 + lo] = tmp[lo - (3 - count)];

        if (count == 3)
            address_loc ++;
    }

    addpkt(pkt_queue, UDP, address, content, size);

    return 0;
}

void *udp_send_pkt(void *udpconfig){

    pudp_config udp_config = (pudp_config) udpconfig;

    struct sockaddr_in si_send;

    const int socketaddr_len = sizeof(si_send);

    // Stored a recovered address.
    char dest_address[17];

    while(udp_config -> shutdown == false){

        if(is_null( &udp_config -> pkt_Queue) == false){

            pthread_mutex_lock( &udp_config -> pkt_Queue.mutex);

            memset(&dest_address, 0, sizeof(char) * 17);

            char *tmp_address = hex_to_char(udp_config -> pkt_Queue.Queue[
                                udp_config -> pkt_Queue.front].address, 12);

            int address_loc = 0;

            for(int n=0;n < 4;n ++){

                bool no_zero = false;

                for(int loc=0;loc < 3;loc ++){

                    if(tmp_address[n * 3 + loc]== '0' && no_zero == false &&
                       loc != 2)
                        continue;

                    no_zero = true;

                    dest_address[address_loc] = tmp_address[n * 3 + loc];

                    address_loc ++;

                }

                if(n < 3){

                    dest_address[address_loc] = '.';

                    address_loc ++;
                }

            }

            printf("Dest Address : %s\n", dest_address );

            memset(&si_send, 0, sizeof(si_send));
            si_send.sin_family = AF_INET;
            si_send.sin_port   = htons(UDP_LISTEN_PORT);

            if (inet_aton(dest_address, &si_send.sin_addr) == 0)

                perror("inet_aton error.\n");

            if (sendto(udp_config -> send_socket, udp_config -> pkt_Queue.Queue[
                       udp_config -> pkt_Queue.front].content, WIFI_MESSAGE_LENGTH,0
                       , (struct sockaddr *) &si_send, socketaddr_len) == -1)
                perror("recvfrom error.\n");

            pthread_mutex_unlock( &udp_config -> pkt_Queue.mutex);

            delpkt( &udp_config -> pkt_Queue);

        }

    }

    printf("Exit Send.\n");

}

void *udp_recv_pkt(void *udpconfig){

    pudp_config udp_config = (pudp_config) udpconfig;

    int ret;

    int recv_len;

    char recv_buf[WIFI_MESSAGE_LENGTH];

    struct sockaddr_in si_recv;

    int socketaddr_len = sizeof(si_recv);

    //keep listening for data
    while(udp_config -> shutdown == false){

        memset(&si_recv, 0, sizeof(si_recv));

        memset(&recv_buf, 0, sizeof(char) * WIFI_MESSAGE_LENGTH);

        recv_len = 0;

        printf("recv pkt.\n");

        //try to receive some data, this is a non-blocking call
        if ((recv_len = recvfrom(udp_config -> recv_socket, recv_buf,
             WIFI_MESSAGE_LENGTH, 0, (struct sockaddr *) &si_recv, &socketaddr_len))
                                                                         == -1){

            printf("error recv_len %d\n", recv_len);

            perror("recvfrom error.\n");

        }
        else if(recv_len > 0){

            //print details of the client/peer and the data received
            printf("Received packet from %s:%d\n", inet_ntoa(si_recv.sin_addr),
                                                   ntohs(si_recv.sin_port));
            printf("Data: %s\n" , recv_buf);

        }
        else
            perror("else recvfrom error.\n");

    }
    printf("Exit Receive.\n");
}

int udp_release(pudp_config udp_config){

    pthread_join(udp_config -> udp_send, NULL);

    pthread_join(udp_config -> udp_receive, NULL);

    close(udp_config -> send_socket);

    close(udp_config -> recv_socket);

    Free_Packet_Queue( &udp_config -> pkt_Queue);

    Free_Packet_Queue( &udp_config -> Received_Queue);

    return 0;
}

char *udp_hex_to_address(unsigned char *hex_addr){

    // Stored a recovered address.
    char *dest_address;
    dest_address = malloc(sizeof(char) * 17);
    memset(&dest_address, 0, sizeof(char) * 17);
    char *tmp_address = hex_to_char(hex_addr, 12);
    int address_loc = 0;
    for(int n=0;n < 4;n ++){

        bool no_zero = false;
        for(int loc=0;loc < 3;loc ++){

            if(tmp_address[n * 3 + loc]== '0' && no_zero == false &&
               loc != 2)
                continue;
            no_zero = true;
            dest_address[address_loc] = tmp_address[n * 3 + loc];
            address_loc ++;

        }

        if(n < 3){

            dest_address[address_loc] = '.';
            address_loc ++;

        }
    }
    return dest_address;
}
