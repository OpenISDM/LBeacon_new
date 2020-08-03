/*
  Copyright (c) BiDaE Technology Inc. All rights reserved.

  License:

    BiDaE SHAREWARE LICENSE
    Version 1.0, 31 July 2020

    Copyright (c) BiDaE Technology Inc. All rights reserved.
    The SOFTWARE implemented in the product is copyrighted and protected by 
    all applicable intellectual property laws worldwide. Any duplication of 
    the SOFTWARE other than for archiving or resetting purposes on the same 
    product without the written agreement from BiDaE Technology could be a 
    violation of law. For the avoidance of doubt, redistribution of this 
    SOFTWARE in source or binary form is prohibited. You agree to prevent 
    any unauthorized copying and redistribution of the SOFTWARE. 

    BiDaE Technology Inc. is the license steward. No one other than the 
    license steward has the right to modify or publish new versions of this 
    License. However, You may distribute the SOFTWARE under the terms of the 
    version of the License under which You originally received the Covered 
    Software, or under the terms of any subsequent version published by the 
    license steward.

    LIMITED WARRANTY:

    BiDaE Technology Inc. or its distributors, depending on which party sold 
    the SOFTWARE, warrants that the media on which the SOFTWARE is installed 
    will be free from defects in materials under normal and purposed use.

    BiDaE Technology Inc. or its distributor warrants, for your benefit alone, 
    that during the Warranty Period the SOFTWARE, shall operate substantially 
    in accordance with the functional specifications in the User's Manual. If, 
    during the Warranty Period, a defect in the SOFTWARE appears, You may 
    obtain a replacement of the SOFTWARE. Any replacement SOFTWARE will be 
    warranted for the remainder of the Warranty Period attached to the product.

    WHEN THE WARRANTY PERIOD HAS BEEN EXPIRED, THIS SOFTWARE IS PROVIDED 
    ''AS IS,'' AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
    PARTICULAR PURPOSE ARE DISCLAIMED. HENCEFORTH, IN NO EVENT SHALL BiDaE 
    TECHNOLOGY INC. OR ITS COLLABORATOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  Project Name:

     BeDIS

  File Name:

     UDP_API.c

  File Description:

     This file contains the program to transmission data using UDP protcol. The 
     device communicates by this UDP API should be in the same network.

  Version:
    
     2.0, 20190608

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
     Gary Xiao      , garyh0205@hotmail.com
 */
#include "UDP_API.h"
#include "libEncrypt.h"


int udp_initial(pudp_config udp_config, int recv_port)
{

    int return_value;

    int timeout;

#ifdef _WIN32
     udp_config -> sockVersion = MAKEWORD(2,2);

     if(WSAStartup(udp_config -> sockVersion, &udp_config -> wsaData) != 0)
         return 0;
#endif
    /* zero out the structure */
    memset((char *) &udp_config -> si_server, 0,
           sizeof(udp_config -> si_server));

    if (return_value = init_Packet_Queue( &udp_config -> pkt_Queue) != 
        pkt_Queue_SUCCESS)
        return return_value;

    if (return_value = init_Packet_Queue( &udp_config -> Received_Queue) != 
        pkt_Queue_SUCCESS)
        return return_value;

    /* create a send UDP socket */
    if ((udp_config -> send_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
        == -1)
        return send_socket_error;

    /* create a recv UDP socket */
    if ((udp_config -> recv_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
        == -1)
        return recv_socket_error;


    timeout = UDP_SELECT_TIMEOUT; // sec

    setsockopt(udp_config -> recv_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout,
               sizeof(timeout));

    udp_config -> si_server.sin_family = AF_INET;
    udp_config -> si_server.sin_port = htons(recv_port);
    udp_config -> si_server.sin_addr.s_addr = htonl(INADDR_ANY);

    udp_config -> shutdown = false;

    udp_config -> recv_port = recv_port;

    /* bind recv socket to the port */
    if( bind(udp_config -> recv_socket, (struct sockaddr *)&udp_config ->
             si_server, sizeof(udp_config -> si_server) ) == -1)
        return recv_socket_bind_error;

    /* The thread is used for receiving data */
    pthread_create(&udp_config -> udp_receive_thread, NULL,    
                   udp_recv_pkt_routine, (void*) udp_config);
    pthread_detach(udp_config -> udp_receive_thread);

    /* The thread is used for sending data */
    pthread_create(&udp_config -> udp_send_thread, NULL, udp_send_pkt_routine, 
                   (void*) udp_config);
    pthread_detach(udp_config -> udp_send_thread);

    return 0;
}


int udp_addpkt(pudp_config udp_config, char *address, unsigned int port, 
               char *content, int size)
{
    int ret = 0;
    char content_sha256[LENGTH_OF_SHA256];
    char ciphertext[LENGTH_OF_ENCODED_WIFI_MESSAGE];

    memset(content_sha256, 0, sizeof(content_sha256));
    ret = SHA_256_Hash(content, content_sha256, sizeof(content_sha256));
    
    memset(ciphertext, 0, sizeof(ciphertext));
    ret = AES_ECB_Encoder_With_Token_Prefix(content_sha256, ciphertext, sizeof(ciphertext));

    strcat(ciphertext, DELIMITER_SEMICOLON);
    strcat(ciphertext, content);
    size = strlen(ciphertext);
    
    if(size > MESSAGE_LENGTH)
        return addpkt_msg_oversize;

    addpkt(&udp_config -> pkt_Queue, address, port, ciphertext, size);

    return 0;
}


sPkt udp_getrecv(pudp_config udp_config)
{
    int ret = 0;
    char content_sha256[LENGTH_OF_SHA256];
    char decodedtext[LENGTH_OF_ENCODED_WIFI_MESSAGE];
    char *ciphertext = NULL;
    char *save_ptr = NULL;
    sPkt empty_pkt;
 

    sPkt tmp = get_pkt(&udp_config -> Received_Queue);

    if(tmp.is_null == true)
        return tmp;

    ciphertext = strtok_save(tmp.content, DELIMITER_SEMICOLON, &save_ptr);
    memset(decodedtext, 0, sizeof(decodedtext));
    if(1 != AES_ECB_Decoder_With_Token_Prefix(ciphertext, decodedtext, sizeof(decodedtext)))
        return empty_pkt;

    memset(content_sha256, 0, sizeof(content_sha256));
    ret = SHA_256_Hash(save_ptr, content_sha256, sizeof(content_sha256));
        
    if(0 != strncmp(decodedtext, content_sha256, strlen(content_sha256)))
        return empty_pkt;

    strcpy(tmp.content, save_ptr);
    tmp.content_size = strlen(tmp.content);

    return tmp;
}


void *udp_send_pkt_routine(void *udpconfig)
{

    pudp_config udp_config = (pudp_config) udpconfig;

    sPkt current_send_pkt;

    struct sockaddr_in si_send;

    while((udp_config -> shutdown) == false)
    {

        if(!(is_null( &udp_config -> pkt_Queue)))
        {

            current_send_pkt = get_pkt(&udp_config -> pkt_Queue);

            if(current_send_pkt.is_null == false)
            {
                memset(&si_send, 0, sizeof(si_send));
                si_send.sin_family = AF_INET;
                si_send.sin_port   = htons(current_send_pkt.port);
                si_send.sin_addr.s_addr   = inet_addr(current_send_pkt.address);

#ifdef debugging
                zlog_info(category_debug, "Start Send pkts\n(sendto [%s] msg [", 
                                                      current_send_pkt.address);
                print_content(current_send_pkt.content,   
                                                 current_send_pkt.content_size);
                zlog_info(category_debug, "])\n");
#endif

                if (sendto(udp_config -> send_socket, current_send_pkt.content, 
                    current_send_pkt.content_size, 0,
                    (struct sockaddr *)&si_send, sizeof(struct sockaddr)) == -1)
                {
#ifdef debugging
                    zlog_info(category_debug, "sendto error.[%s]\n", strerror(errno));
#endif
                }
                else
                {
#ifdef debugging
                    zlog_info(category_debug, "Send pkt success\n");
#endif
                }
            }
            else
            {
                sleep_t(SEND_THREAD_IDLE_SLEEP_TIME);
            }
        }
        else
        {
            sleep_t(SEND_THREAD_IDLE_SLEEP_TIME);
        }

    }

    return (void *)NULL;
}


void *udp_recv_pkt_routine(void *udpconfig)
{

    pudp_config udp_config = (pudp_config) udpconfig;

    int recv_len;

    char recv_buf[MESSAGE_LENGTH];

    char *address_ntoa_ptr;

    char address_ntoa[NETWORK_ADDR_LENGTH];

    int port;

    struct sockaddr_in si_recv;

    int socketaddr_len = sizeof(si_recv);


    /* keep listening for data */
    while((udp_config -> shutdown) == false)
    {

        memset(&si_recv, 0, sizeof(si_recv));

        memset(&recv_buf, 0, sizeof(char) * MESSAGE_LENGTH);

        recv_len = 0;
#ifdef debugging
        zlog_info(category_debug, "recv pkt.");
#endif
        /* try to receive some data, this is a non-blocking call */
        if ((recv_len = recvfrom(udp_config -> recv_socket, recv_buf,
             MESSAGE_LENGTH, 0, (struct sockaddr *) &si_recv, 
             (socklen_t *)&socketaddr_len)) == -1)
        {
#ifdef debugging
            zlog_info(category_debug, "No data received.");
#endif
            sleep_t(RECEIVE_THREAD_IDLE_SLEEP_TIME);
        }
        else if(recv_len > 0)
        {

            memset(address_ntoa, 0, sizeof(address_ntoa));
        
            address_ntoa_ptr = inet_ntoa(si_recv.sin_addr);

            memcpy(address_ntoa, address_ntoa_ptr, strlen(address_ntoa_ptr) * 
                   sizeof(char));

            port = ntohs(si_recv.sin_port);

#ifdef debugging
            /* print details of the client/peer and the data received */
            printf("Received packet from %s:%d\n", address_ntoa, port);
            printf("Data: [");
            print_content(recv_buf, recv_len);
            printf("]\n");
            printf("Data Length %d\n", recv_len);
#endif
            addpkt(&udp_config -> Received_Queue, address_ntoa, port,
                   recv_buf, recv_len);
        }
#ifdef debugging
        else
            zlog_info(category_debug, "else recvfrom error.");
#endif
    }
#ifdef debugging
    zlog_info(category_debug, "Exit Receive.");
#endif
    
    return (void *)NULL;
}


int udp_release(pudp_config udp_config)
{

    udp_config -> shutdown = true;

#ifdef _WIN32
    closesocket(udp_config -> send_socket);

    closesocket(udp_config -> recv_socket);
    
    WSACleanup();
#else
    close(udp_config -> send_socket);

    close(udp_config -> recv_socket);
#endif

    Free_Packet_Queue( &udp_config -> pkt_Queue);

    Free_Packet_Queue( &udp_config -> Received_Queue);

    return 0;
}
