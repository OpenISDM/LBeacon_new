


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

    return 1;
}


/*
*  zigbee_connection:
*
*  When called, this function sends a containing the specified message packet 
*  to the gateway via xbee module and and receives command or data from the 
*  gateway. 
*
*  Parameters:
*
*  zigbee - the struct of necessary parameter and data
*  message - the message be sent to the gateway 
*
*  Return value:
*
*  ErrorCode: The error code for the corresponding error
*
*/

int zigbee_connection(Zigbee zigbee, char *message){
    

        
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
        return 0;
    
    }


    addpkt(zigbee.pkt_Queue, Data, Gateway, message);

    /* If there are remain some packet need to send in the Queue,            */
    /* send the packet                                                   */
    if(zigbee.pkt_Queue->front->next != NULL){

        xbee_conTx(zigbee.con, NULL, zigbee.pkt_Queue->front->next->content);

        delpkt(zigbee.pkt_Queue);
        
    }
    else{
        
        xbee_log(zigbee.xbee, -1, "xbee packet Queue is NULL.");
        
    }
        
    usleep(2000000);   
 

   return 1;
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