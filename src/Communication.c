/*
 Copyright (c) 2016 Academia Sinica, Institute of Information Science

 License:

      GPL 3.0 : The content of this file is subject to the terms and
      cnditions defined in file 'COPYING.txt', which is part of this source
      code package.

 Project Name:

      BeDIPS

 File Description:

      This file contains the programs to allow data transmission between 
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
#define Debugging

ErrorCode_Xbee zigbee_init(){

    /* The error indicator returns from the libxbee library */
    int error_indicator;

    xbee_config.xbee_mode = XBEE_MODE;
    xbee_config.xbee_device = XBEE_DEVICE;
    xbee_config.xbee_datastream = XBEE_DATASTREAM;
    xbee_config.config_location = XBEE_CONFIG_PATH;

    
    xbee_Serial_Power_Reset(xbee_Serial_Power_Pin);

    usleep(XBEE_TIMEOUT);

    xbee_Serial_init(&xbee_config.xbee_datastream, 
                     xbee_config.xbee_device);

    xbee_LoadConfig(&xbee_config);

    close(xbee_config.xbee_datastream);

    xbee_initial(&xbee_config);
   
#ifdef Debugging 

    zlog_debug(category_debug, "Establishing Connection...");
      
#endif

    xbee_connector(&xbee_config);

    
#ifdef Debugging 
   
    zlog_debug(category_debug, 
               "Zigbee Connection Successfully Established");
      
#endif

    zlog_info(category_health_report, 
              "Zigbee Connection Successfully Established");

    /* Start the chain reaction                                             */
    if((error_indicator = xbee_conValidate(xbee_config.con)) != XBEE_ENONE){

#ifdef Debugging 

        zlog_debug(category_debug, "con unvalidate ret : %d", 
                   error_indicator);
      
#endif      
 /*       
        perror(error_xbee[E_XBEE_VALIDATE].message);
        zlog_info(category_health_report, 
                  error_xbee[E_XBEE_VALIDATE].message);
 */       
        return E_XBEE_VALIDATE;
    }

    return XBEE_SUCCESSFULLY;
}


int receive_call_back(){
    
 
    /* Check the connection of call back is enable */ 
    if(xbee_check_CallBack(&xbee_config, false)){
 /*     
      perror(error_xbee[E_CALL_BACK].message);
      zlog_info(category_health_report, 
                error_xbee[E_CALL_BACK].message);
 */     
      return NOT_YET_POLLED;
    
    };

    /* Get the polled type from the packet received from the gateway */
    pPkt temppkt = get_pkt(&xbee_config.Received_Queue);
        
    if(temppkt != NULL){


        if(temppkt -> content[0] == 'T'){

          /* Delete the packet and return the indicator. */
          delpkt(&xbee_config.Received_Queue);
          return TRACK_OBJECT_DATA;

        }else if(temppkt -> content[0] == 'H'){

          /* Delete the packet and return the indicator back. */
          delpkt(&xbee_config.Received_Queue);
          return HEALTH_REPORT; 

        /* If data[0] == '@', callback will be end. */
        }else if(temppkt -> content[0] == '@'){

            xbee_conCallbackSet(xbee_config.con, NULL, NULL);

        }

         delpkt(&xbee_config.Received_Queue);   

    }

    return NOT_YET_POLLED;

}

void *zigbee_send_file(char *zig_message){


    /* Add the content that to be sent to the gateway to the packet queue */
    addpkt(&xbee_config.pkt_Queue, Data, Gateway, zig_message);

    /* If there are remain some packet need to send in the Queue,send the 
    packet */                                      
    xbee_send_pkt(&xbee_config);


    xbee_connector(&xbee_config);

    usleep(XBEE_TIMEOUT);
        

   return;
}


void zigbee_free(){


    /* Release the xbee elements and close the connection. */
    xbee_release(&xbee_config);
   
    zlog_info(category_health_report, 
              "Stop Xbee connection Succeeded\n");

    return;

}
