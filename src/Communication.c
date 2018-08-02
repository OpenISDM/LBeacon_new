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



ErrorCode_Xbee zigbee_init(Zigbee zigbee){

     /* Parameters for Zigbee initialization */
    char* xbee_mode = "xbeeZB";

    char* xbee_device = "/dev/ttyAMA0";

    int xbee_baudrate = 9600;

    int LogLevel = 100;

    xbee_initial(xbee_mode, xbee_device, xbee_baudrate, 
                        LogLevel, &(zigbee.xbee), &zigbee.pkt_Queue);
    printf("Start establishing Connection to xbee\n");


    printf("Establishing Connection...\n");  

    xbee_connector(&(zigbee.xbee), &(zigbee.con), &zigbee.pkt_Queue);
    
    printf("Connection Successfully Established\n");

    /* Start the chain reaction                                             */
    if((ret = xbee_conValidate(zigbee.con)) != XBEE_ENONE){
        
        xbee_log(zigbee.xbee, 1, "con unvalidate ret : %d", ret);
        
        perror(error_xbee[E_XBEE_VALIDATE].message);
        
        return E_XBEE_VALIDATE;
    }

    return XBEE_SUCCESSFULLY;
}


int receive_call_back(Zigbee zigbee){
  
    /* Check the connection of call back is enable */ 
    if(xbee_check_CallBack(zigbee.con, &zigbee.pkt_Queue, false)){

      perror(error_xbee[E_CALL_BACK].message);
        
      return NULL;
    
    };
    
    /* Get the polled type form the gateway */
    int call_back_type = CallBack(zigbee.xbee, zigbee.con, &zigbee.pkt_Queue, NULL);

    switch(call_back_type){

        case TRACK_OBJECT_DATA:
          
          return TRACK_OBJECT_DATA;
          break;

        case HEALTH_REPORT:

          return HEALTH_REPORT;
          break;

        default:
          break;
    
    }

    return NOT_YET_POLLED;

}

void zigbee_send_file(Zigbee zigbee){
    
    /* Add the content that to be sent to the gateway to the packet queue */
    addpkt(&zigbee.pkt_Queue, Data, Gateway, zigbee.zig_message);

    /* If there are remain some packet need to send in the Queue,send the 
    packet */                                      
    xbee_send_pkt(zigbee.con, &zigbee.pkt_Queue);

    usleep(XBEE_TIMEOUT);
        
    xbee_connector(&zigbee.xbee, &zigbee.con, &zigbee.pkt_Queue);  
 

   return;
}


ErrorCode_Xbee zigbee_free(Zigbee zigbee){

    Free_Packet_Queue(&zigbee.pkt_Queue);

    /* Close connection                                                      */
    if ((ret = xbee_conEnd(zigbee.con)) != XBEE_ENONE) {
        
        xbee_log(zigbee.xbee, 10, "xbee_conEnd() returned: %d", ret);
        perror(error_xbee[E_CONNECT].message);

        return;
    }

    Free_Packet_Queue(&zigbee.pkt_Queue);
    
    printf("Stop connection Succeeded\n");

    /* Close xbee                                                            */
    xbee_shutdown(zigbee.xbee);
    printf("Shutdown Xbee Succeeded\n");

}