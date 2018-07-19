/*
 Copyright (c) 2016 Academia Sinica, Institute of Information Science

 License:

      GPL 3.0 : The content of this file is subject to the terms and
      cnditions defined in file 'COPYING.txt', which is part of this source
      code package.

 Project Name:

      BeDIPS

 File Description:

      This file contains the program to allow transmission between 
       LBeacon and Gateway. 

 File Name:

      Communication.c

 Version:
 
       1.2

 Abstract:

      BeDIPS uses LBeacons to deliver 3D coordinates and textual
      descriptions of their locations to users' devices. Basically, a
      LBeacon is an inexpensive, Bluetooth Smart Ready device. The 3D
      coordinates and location description of every LBeacon are retrieved
      from BeDIS (Building/environment Data and Information System) and
      stored locally during deployment and maintenance times. Once
      initialized, each LBeacon broadcasts its coordinates and location
      description to Bluetooth enabled user devices within its coverage
      area.

 Authors:

      Han Wang, hollywang@iis.sinica.edu.tw
      Gary Xiao, garyh0205@hotmail.com      
      
*/


#include "Communication.h"



int zigbee_init(Zigbee zigbee){

     /* Parameters for Zigbee initialization */
    char* xbee_mode  = "xbeeZB";
    char* xbee_device = "/dev/ttyAMA0"; 
    int xbee_baudrate = 9600;
    int LogLevel = 100;

    
    zigbee.pkt_Queue = malloc(sizeof(spkt_ptr));

    xbee_initial(xbee_mode, xbee_device, xbee_baudrate
                            , LogLevel, &(zigbee.xbee), zigbee.pkt_Queue);
    
    printf("Start establishing Connection to xbee\n");


    /*--------------Configuration for connection in Data mode--------------*/
    /* In this mode we aim to get Data.                                    */
    /*---------------------------------------------------------------------*/

    printf("Establishing Connection...\n");

    xbee_connector(&(zigbee.xbee), &(zigbee.con), zigbee.pkt_Queue);

    printf("Connection Successfully Established\n");

    /* Start the chain reaction!                                           */

    if((ret = xbee_conValidate(zigbee.con)) != XBEE_ENONE){
        xbee_log(zigbee.xbee, 1, "con unvalidate ret : %d", ret);
        return;
    }

    return XBEE_SUCCESSFULLY;
}




void *zigbee_send_file(Zigbee zigbee){
    

        
    /* Pointer point_to_CallBack will store the callback function.       */
    /* If pointer point_to_CallBack is NULL, break the Loop              */
        
    void *point_to_CallBack;

    if ((ret = xbee_conCallbackGet(zigbee.con, (xbee_t_conCallback*)            
        &point_to_CallBack))!= XBEE_ENONE) {

        xbee_log(zigbee.xbee, -1, "xbee_conCallbackGet() returned: %d", ret);
        return 0;
        
    }

    if (point_to_CallBack == NULL){
            
        printf("Stop Xbee...\n");
        return XBEE_ERROR;
    
    }


    addpkt(zigbee.pkt_Queue, Data, Gateway, zigbee.zig_message);

    /* If there are remain some packet need to send in the Queue,            */
    /* send the packet                                                   */
    if(zigbee.pkt_Queue->front->next != NULL){

        xbee_conTx(zigbee.con, NULL, zigbee.pkt_Queue->front->next->content);

        delpkt(zigbee.pkt_Queue);
        
    }
    else{
        
        xbee_log(zigbee.xbee, -1, "xbee packet Queue is NULL.");
        
    }
        
    /* A short time interval between transmission */
    usleep(XBEE_TIMEOUT);   
 

   return XBEE_SUCCESSFULLY;
}

void zigbee_free(Zigbee zigbee){

    /* Free Packet Queue for zigbee connection */
    Free_Packet_Queue(zigbee.pkt_Queue);

    /* Close connection  */
    if ((ret = xbee_conEnd(zigbee.con)) != XBEE_ENONE) {
        xbee_log(zigbee.xbee, 10, "xbee_conEnd() returned: %d", ret);
        return;
    }
    printf("Stop connection Succeeded\n");

    /* Close xbee                                                            */
    xbee_shutdown(zigbee.xbee);
    printf("Shutdown Xbee Succeeded\n");

}