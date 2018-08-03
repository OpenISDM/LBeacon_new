/*
 * Copyright (c) 2016 Academia Sinica, Institute of Information Science
 *
 * License:
 *
 *      GPL 3.0 : The content of this file is subject to the terms and
 *      cnditions defined in file 'COPYING.txt', which is part of this
 *      source code package.
 *
 * Project Name:
 *
 *      BeDIPS
 *
 * File Description:
 *
 *      This file contains the program to connect to xbee by API mode and in
 *      the project, we use it for data transmission most.
 *
 * File Name:
 *
 *      xbee_API.c
 *
 * Abstract:
 *
 *      BeDIPS uses LBeacons to deliver 3D coordinates and textual
 *      descriptions of their locations to users' devices. Basically, a
 *      LBeacon is an inexpensive, Bluetooth Smart Ready device. The 3D
 *      coordinates and location description of every LBeacon are retrieved
 *      from BeDIS (Building/environment Data and Information System) and
 *      stored locally during deployment and maintenance times. Once
 *      initialized, each LBeacon broadcasts its coordinates and location
 *      description to Bluetooth enabled user devices within its coverage
 *      area.
 *
 * Authors:
 *      Gary Xiao       , garyh0205@hotmail.com
 */

#include "xbee_API.h"


/*
 * xbee_initial
 *     For initialize zigbee, include loading config.
 * Parameter:
 *     xbee_mode: we use xbeeZB as our device, this parameter is for setting
 *                libxbee3 work mode.
 *     xbee_device: This parameter is to define where is our zigbee device path.
 *     xbee_baudrate: This parameter is to define what our zigbee working
 *                    baudrate.
 *     LogLevel: To decide libxbee3 whether need to export log or not.
 *     xbee: A pointer to catch zigbee pointer.
 *     pkt_Queue: A pointer of the packet queue we use.
 * Return Value:
 *     xbee_err: If return 0, everything work successfully.
 *               If not 0, somthing wrong.
 */
xbee_err xbee_initial(char* xbee_mode, char* xbee_device, int xbee_baudrate
                        , int LogLevel, struct xbee** xbee, pkt_ptr pkt_Queue){
    printf("Start Connecting to xbee\n");
    printf("xbee Setup\n");
    printf("xbee Mode : %s\n",xbee_mode);
    printf("xbee_device : %s\n", xbee_device);
    printf("xbee_baudrate : %d\n", xbee_baudrate);

    if ((ret = xbee_setup(xbee, xbee_mode, xbee_device, xbee_baudrate))
                                                        != XBEE_ENONE) {
        printf("Connection Failed\nret: %d (%s)\n", ret, xbee_errorToStr(ret));
        return ret;
    }
    printf("xbee Connected\n");

    if((ret = xbee_validate(*xbee)) != XBEE_ENONE){
        printf("Connection unvalidate\nret: %d (%s)\n",
        ret, xbee_errorToStr(ret));
        return ret;
    }

    printf("Start Setting up Log Level\n");

    /* Setup Log Level 0:disable Log, 100:enable Log                         */
    if ((ret = xbee_logLevelSet(*xbee, LogLevel)) != XBEE_ENONE) {
        printf("Setting Failed\nret: %d (%s)\n", ret, xbee_errorToStr(ret));
        return ret;
    }
    printf("Setting Log Level Success\nLog Level : %d\n",LogLevel);

    init_Packet_Queue(pkt_Queue);

    return ret;
}

/*
 * xbee_connector
 *     For connect to zigbee and assign it's destnation address.
 * Parameter:
 *     xbee: A pointer to catch zigbee pointer.
 *     con: A pointer of the connector of zigbee.
 *     pkt_Queue: A pointer of the packet queue we use.
 * Return Value:
 *     xbee_err: If return 0, everything work successfully.
 *               If not 0, somthing wrong.
 */
xbee_err xbee_connector(struct xbee** xbee, struct xbee_con** con
                                                , pkt_ptr pkt_Queue){
    

    bool Require_CallBack = true;

    if((ret = xbee_conValidate(*con)) == XBEE_ENONE){
        if(is_null(pkt_Queue))
            return XBEE_ENONE;
        else if(address_compare(pkt_Queue->front.next->address, pkt_Queue->address)){
            printf("Same Address\n");
            return XBEE_ENONE;
        }
        else{
            Require_CallBack = !(xbee_check_CallBack(*con, pkt_Queue, true));
            
            /* Close connection                                                      */
            if ((ret = xbee_conEnd(*con)) != XBEE_ENONE) {
                xbee_log(*xbee, 10, "xbee_conEnd() returned: %d", ret);
            }
        }
    }

    int Mode;

    struct xbee_conAddress address;
    struct xbee_conSettings settings;

    /* ---------------------- Setup dest address. -------------------------- */
    /* If the packet Queue still remain packets, continue to fill address    */

    memset(&address, 0, sizeof(address));
    memset(pkt_Queue->address, 0, sizeof(unsigned char) * 8);
    
    address.addr64_enabled = 1;

    printf("Fill Address to the Connector\n");
    if(!is_null(pkt_Queue)){
        for(int i=0 ; i < 8 ; i++){
            address.addr64[i] = pkt_Queue->front.next->address[i];
            pkt_Queue->address[i] = pkt_Queue->front.next->address[i];
        }
        Mode = pkt_Queue->front.next->type;
    }
    else{
        Mode = Data;
    }

    printf("Fill Address Success\n");
    
    char* strMode = type_to_str(Mode);
    printf("Mode : %s\n", strMode);
    if(Mode == Local_AT){
        if((ret = xbee_conNew(*xbee, con, strMode, NULL)) != XBEE_ENONE) {
            xbee_log(*xbee, 1, "xbee_conNew() returned: %d (%s)", ret
                                                , xbee_errorToStr(ret));
            return ret;
        }
        printf("Enter Local_AT Mode\n");
    }
    else if(Mode == Data){
        if((ret = xbee_conNew(*xbee, con, strMode, &address)) != XBEE_ENONE) {
            xbee_log(*xbee, 1, "xbee_conNew() returned: %d (%s)", ret
                                                , xbee_errorToStr(ret));
            return ret;
        }
        printf("Enter Data Mode\n");
    }
    else{
        printf("<<Error>> conMode Error\n");
        return XBEE_EFAILED;
    }

    if(Require_CallBack){
        /* Set CallBack Function to call CallBack if packet received              */
        if((ret = xbee_conCallbackSet(*con, (xbee_t_conCallback) CallBack, NULL)) != XBEE_ENONE) {
            xbee_log(*xbee, 1, "xbee_conCallbackSet() returned: %d", ret);

            printf("Error!!! initialize  \n");
            return ret;
        }
    }

    if((ret = xbee_conValidate(*con)) != XBEE_ENONE){
        xbee_log(*xbee, 1, "con unvalidate ret : %d", ret);
        return ret;
    }

    /* If settings.catchAll = 1, then all packets will receive                */
    if ((ret = xbee_conSettings(*con, NULL, &settings)) != XBEE_ENONE)
                                                            return ret;

    settings.catchAll = 1;

    if ((ret = xbee_conSettings(*con, &settings, NULL)) != XBEE_ENONE)
                                                            return ret;

    /*
    if ((ret = xbee_conDataSet(*con, *xbee, NULL)) != XBEE_ENONE) {
        xbee_log(*xbee, -1, "xbee_conDataSet() returned: %d", ret);
        return ret;
    }
    */

    printf("Connector Established\n");

    return XBEE_ENONE;
}

/*
 * xbee_send_pkt
 *      For sending pkt to dest address.
 * Parameter:
 *      con : a pointer for xbee connector.
 *      pkt_Queue : A pointer point to the packet queue we use.
 * Return Value:
 *      xbee error code
 *      if 0, work successfully.
 */
xbee_err xbee_send_pkt(struct xbee_con* con, pkt_ptr pkt_Queue){ 
    if(!(is_null(pkt_Queue))){
        if(!(address_compare(pkt_Queue->front.next->address, pkt_Queue->address))){
            printf("Not the same, Error\n");
            return XBEE_ENONE;        
        }
        xbee_conTx(con, NULL, pkt_Queue->front.next->content);
        delpkt(pkt_Queue);
    }else{
        printf("pkt_queue is NULL");
    }

    return XBEE_ENONE;
}

/*
 * xbee_check_CallBack
 *      Check if CallBack is disabled and pkt_Queue is NULL.
 * Parameter:
 *      con : a pointer for xbee connector.
 *      pkt_Queue : A pointer point to the packet queue we use.
 * Return Value:
 *      True if CallBack is disabled and pkt_Queue is NULL, else false.
 *
 */
bool xbee_check_CallBack(struct xbee_con* con, pkt_ptr pkt_Queue, bool exclude_pkt_Queue){
    /* Pointer point_to_CallBack will store the callback function.       */
    /* If pointer point_to_CallBack is NULL, break the Loop              */
    void *point_to_CallBack;

    if ((ret = xbee_conCallbackGet(con, (xbee_t_conCallback*)
        &point_to_CallBack))!= XBEE_ENONE) {
    return true;
    }

    if (point_to_CallBack == NULL && (exclude_pkt_Queue || is_null(pkt_Queue))){
        printf("Stop Xbee...\n");
        return true;
    }
    return false;
}


/* ---------------------------callback Section------------------------------ */
/* It will be executed once for each packet that is received on              */
/* an associated connection                                                  */
/* ------------------------------------------------------------------------- */

/*  Data Transmission                                                        */
int CallBack(struct xbee *xbee, struct xbee_con *con, struct xbee_pkt **pkt
                                                            , void **data) {
    printf("Enter CallBack Data\n");
    if ((*pkt)->dataLen > 0) {

        printf("Received Data: %s\n",((*pkt)->data));

        /* If data[0] == '@', callback will be end.                          */
        if ((*pkt)->data[0] == '@') {
            /* Disable the call back function */
            xbee_conCallbackSet(con, NULL, NULL);
        
        }else if((*pkt)->data[0] == 'T'){

             return 1;
        
        }else if((*pkt)->data[0] == 'H'){

            return 2;
        } 
        xbee_log(xbee, -1, "rx: [%s]\n", (*pkt)->data);

       
    }
}
