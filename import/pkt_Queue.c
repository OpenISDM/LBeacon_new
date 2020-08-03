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

     pkt_Queue.c

  File Description:

     This file contains functions for the waiting queue.

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

#include "pkt_Queue.h"


/* Initialize and free Queue */


int init_Packet_Queue(pkt_ptr pkt_queue)
{
    /* The variable for initializing the pkt queue  */
    int num;

    pthread_mutex_init( &pkt_queue -> mutex, 0);

    pthread_mutex_lock( &pkt_queue -> mutex);

    pkt_queue -> is_free = false;

    pkt_queue -> front = -1;

    pkt_queue -> rear  = -1;

    /* Initialize all flags in the pkt queue  */
    for(num = 0;num < MAX_QUEUE_LENGTH; num ++)
        pkt_queue -> Queue[num].is_null = true;

    pthread_mutex_unlock( &pkt_queue -> mutex);

    return pkt_Queue_SUCCESS;

}


int Free_Packet_Queue(pkt_ptr pkt_queue)
{
    /* The variable for initializing the pkt queue  */
    int num;

    pthread_mutex_lock( &pkt_queue -> mutex);

    pkt_queue -> is_free = true;

    /* Delete all pkts in the pkt queue */
    while (is_null(pkt_queue) == false)
        delpkt(pkt_queue);

    /* Reset all flags in the pkt queue */
    for(num = 0;num < MAX_QUEUE_LENGTH; num ++)
        pkt_queue -> Queue[num].is_null = true;

    pthread_mutex_unlock( &pkt_queue -> mutex);

    pthread_mutex_destroy( &pkt_queue -> mutex);

    return pkt_Queue_SUCCESS;

}


/* New : add pkts */


int addpkt(pkt_ptr pkt_queue, char *address, unsigned int port, 
           char *content, int content_size)
{

    int current_idx;

    if(content_size > MESSAGE_LENGTH)
        return MESSAGE_OVERSIZE;

    pthread_mutex_lock( &pkt_queue -> mutex);

    if(pkt_queue -> is_free == true)
    {
        pthread_mutex_unlock( &pkt_queue -> mutex);
        return pkt_Queue_is_free;
    }

#ifdef debugging
    printf("--------- Content ---------\n");

    printf("address            : %s\n", address);
    printf("port               : %d\n", port);

    printf("\n");
    printf("--------- content ---------\n");

    print_content(content, content_size);

    printf("\n");
    printf("---------------------------\n");
#endif

    if(is_full(pkt_queue) == true)
    {
        /* If the pkt queue is full */
        pthread_mutex_unlock( &pkt_queue -> mutex);
        return pkt_Queue_FULL;
    }
    else if(is_null(pkt_queue) == true)
    {
        /* If there is no pkt in the pkt queue */
        pkt_queue -> front = 0;
        pkt_queue -> rear  = 0;
    }
    else if( pkt_queue -> rear == MAX_QUEUE_LENGTH - 1)
    {
        /* If the rear points to the end of the queue reset the location to the 
           first location of the pkt queue 
         */
        pkt_queue -> rear = 0;
    }
    else
    {
        /* If the rear not points to the end of the pkt queue move to the next 
           location of the pkt queue 
         */
        pkt_queue -> rear ++;
    }

    current_idx = pkt_queue -> rear;

    pkt_queue -> Queue[current_idx].is_null = false;

    memset(pkt_queue -> Queue[current_idx].address, 0, 
           NETWORK_ADDR_LENGTH * sizeof(char));

    strncpy(pkt_queue -> Queue[current_idx].address, address, 
            NETWORK_ADDR_LENGTH);

    pkt_queue -> Queue[current_idx].port = port;

    memset(pkt_queue -> Queue[current_idx].content, 0, 
           MESSAGE_LENGTH * sizeof(char));

    strncpy(pkt_queue -> Queue[current_idx].content, content, 
            content_size);

    pkt_queue -> Queue[current_idx].content_size = content_size;

#ifdef debugging
    display_pkt("addedpkt", pkt_queue, current_idx);

    printf("= pkt_queue len  =\n");

    printf("%d\n", queue_len(pkt_queue));

    printf("==================\n");
#endif

    pthread_mutex_unlock( &pkt_queue -> mutex);

    return pkt_Queue_SUCCESS;

}


sPkt get_pkt(pkt_ptr pkt_queue)
{

    sPkt tmp;

    pthread_mutex_lock( &pkt_queue -> mutex);

    memset(&tmp, 0, sizeof(tmp));

    if(is_null(pkt_queue) == true)
    {
        /* If the pkt queue is null, return a blank pkt */
        tmp.is_null = true;

        pthread_mutex_unlock( &pkt_queue -> mutex);
        return tmp;
    }

#ifdef debugging
    display_pkt("Get_pkt", pkt_queue, pkt_queue -> front);
#endif

    tmp = pkt_queue -> Queue[pkt_queue -> front];

    delpkt(pkt_queue);

    pthread_mutex_unlock( &pkt_queue -> mutex);

    return tmp;
}


/* Delete : delete pkts */


int delpkt(pkt_ptr pkt_queue) 
{

    int current_idx;

    if(is_null(pkt_queue) == true) 
    {
        return pkt_Queue_SUCCESS;
    }

    current_idx = pkt_queue -> front;

#ifdef debugging
    display_pkt("deledpkt", pkt_queue, current_idx);
#endif

    pkt_queue -> Queue[current_idx].is_null = true;

    memset(pkt_queue -> Queue[current_idx].content, 0, 
           MESSAGE_LENGTH * sizeof(char));

    memset(pkt_queue -> Queue[current_idx].address, 0, 
           NETWORK_ADDR_LENGTH * sizeof(char));

    pkt_queue -> Queue[current_idx].port = 0;

    if(current_idx == pkt_queue -> rear)
    {
        pkt_queue -> front = -1;

        pkt_queue -> rear  = -1;
    }
    else if(current_idx == MAX_QUEUE_LENGTH - 1)
        pkt_queue -> front = 0;
    else
        pkt_queue -> front += 1;

#ifdef debugging

    printf("= pkt_queue len  =\n");

    printf("%d\n", queue_len(pkt_queue));

    printf("==================\n");

#endif

    return pkt_Queue_SUCCESS;

}


int display_pkt(char *display_title, pkt_ptr pkt_queue, int pkt_num)
{

    pPkt current_pkt;

    if(pkt_num < 0 || pkt_num >= MAX_QUEUE_LENGTH)
    {
        return pkt_Queue_display_over_range;
    }

    current_pkt = &pkt_queue -> Queue[pkt_num];

    printf("==================\n");

    printf("%s\n", display_title);

    printf("==================\n");

    printf("===== address ====\n");

    printf("%s\n", current_pkt -> address);

    printf("====== port ======\n");

    printf("%d\n", current_pkt -> port);

    printf("==== content =====\n");

    print_content(current_pkt -> content, current_pkt -> content_size);

    printf("\n");
    printf("==================\n");

    return pkt_Queue_SUCCESS;
}


/* Tools */


bool is_null(pkt_ptr pkt_queue)
{

    if (pkt_queue->front == -1 && pkt_queue->rear == -1)
        return true;

    return false;
}


bool is_full(pkt_ptr pkt_queue)
{

    if(pkt_queue -> front == pkt_queue -> rear + 1)
        
        return true;

    else if(pkt_queue -> front == 0 && 
            pkt_queue -> rear == MAX_QUEUE_LENGTH - 1)
        
        return true;

    else
        
        return false;
}


int queue_len(pkt_ptr pkt_queue)
{

    if (pkt_queue -> front == 0 && pkt_queue -> rear == 0)
        return 1;

    else if(pkt_queue -> front == -1 && pkt_queue -> rear == -1)
        return 0;

    else if (pkt_queue -> front == pkt_queue -> rear)
        return 1;

    else if (pkt_queue -> rear > pkt_queue -> front){

        int len = (pkt_queue -> rear - pkt_queue -> front + 1);
        return len;
    }

    else if (pkt_queue -> front > pkt_queue -> rear){

        int len = ((MAX_QUEUE_LENGTH - pkt_queue -> front) + 
                   pkt_queue -> rear + 1);
        return len;
    }

    else
        return queue_len_error;

    return queue_len_error;
}


void print_content(char *content, int size)
{

    int loc;

    for(loc = 0; loc < size; loc ++)
        printf("%c", content[loc]);

}
