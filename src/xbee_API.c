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

xbee_err xbee_initial(char* xbee_mode, char* xbee_device, int xbee_baudrate
                    , struct xbee** xbee, pkt_ptr pkt_Queue
                    , pkt_ptr Received_Queue){

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

        printf("Connection unvalidate\nret: %d (%s)\n", ret
              , xbee_errorToStr(ret));

        return ret;

    }

    init_Packet_Queue(pkt_Queue);

    init_Packet_Queue(Received_Queue);

    return ret;
}

xbee_err xbee_connector(struct xbee** xbee, struct xbee_con** con
                      , pkt_ptr pkt_Queue, pkt_ptr Received_Queue){

    bool Require_CallBack = true;

    if((ret = xbee_conValidate(*con)) == XBEE_ENONE){

        if(is_null(pkt_Queue))

            return XBEE_ENONE;

        else if(address_compare(pkt_Queue -> Queue[pkt_Queue -> front].address
                              , pkt_Queue -> address)){

            printf("Same Address\n");

            return XBEE_ENONE;

        }

        else{

            Require_CallBack = !(xbee_check_CallBack(*con, pkt_Queue, true));

            /* Close connection                                               */
            if ((ret = xbee_conEnd(*con)) != XBEE_ENONE) {

                xbee_log(*xbee, 10, "xbee_conEnd() returned: %d", ret);

            }

        }

    }

    int Mode;

    struct xbee_conAddress address;

    struct xbee_conSettings settings;

    memset(&address, 0, sizeof(address));

    memset(pkt_Queue -> address, 0, sizeof(unsigned char) * 8);

    address.addr64_enabled = 1;

    printf("Fill Address to the Connector\n");

    if(!is_null(pkt_Queue)){

        address_copy(pkt_Queue -> Queue[pkt_Queue -> front].address
                   , address.addr64);

        address_copy(pkt_Queue -> Queue[pkt_Queue -> front].address
                   , pkt_Queue -> address);

        Mode = pkt_Queue -> Queue[pkt_Queue -> front].type;

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

        /* Set CallBack Function to call CallBack if packet received          */
        if((ret = xbee_conCallbackSet(*con, CallBack, NULL)) != XBEE_ENONE) {

            xbee_log(*xbee, 1, "xbee_conCallbackSet() returned: %d", ret);

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

    if ((ret = xbee_conDataSet(*con, Received_Queue, NULL)) != XBEE_ENONE) {

        xbee_log(*xbee, -1, "xbee_conDataSet() returned: %d", ret);

        return ret;

    }

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

        if(!(address_compare(pkt_Queue -> Queue[pkt_Queue -> front].address
                           , pkt_Queue -> address))){

            printf("Not the same, Error\n");

            return XBEE_ENONE;

        }

        xbee_conTx(con, NULL, pkt_Queue -> Queue[pkt_Queue -> front].content);

        delpkt(pkt_Queue);

    }

    else{

        printf("pkt_queue is NULL");

    }

    return XBEE_ENONE;

}

bool xbee_check_CallBack(struct xbee_con* con, pkt_ptr pkt_Queue
                       , bool exclude_pkt_Queue){

    /* Pointer point_to_CallBack will store the callback function.       */
    /* If pointer point_to_CallBack is NULL, break the Loop              */

    void *point_to_CallBack;

    if ((ret = xbee_conCallbackGet(con
     , (xbee_t_conCallback*)&point_to_CallBack))!= XBEE_ENONE) {

         return true;
    }

    if (point_to_CallBack == NULL && (exclude_pkt_Queue || is_null(pkt_Queue))){

        return true;

    }

    return false;

}

xbee_err xbee_release(struct xbee* xbee, struct xbee_con* con
                      , pkt_ptr pkt_Queue, pkt_ptr Received_Queue){

    add_log(&pkt_Queue -> xbee_log, collect_info, "Stop xbee ...", false);

    /* Close connection                                                      */
    if(xbee_conValidate(con) != XBEE_ENONE){

        if ((ret = xbee_conEnd(con)) != XBEE_ENONE) {

            char ret_value[100];

            memset(ret_value, 0, 100);

            sprintf(ret_value, "xbee_conEnd() returned: %d.", ret);

            add_log(&pkt_Queue -> xbee_log, collect_info, ret_value, false);

        }
    }

    Free_Packet_Queue(pkt_Queue);

    Free_Packet_Queue(Received_Queue);

    add_log(&pkt_Queue -> xbee_log, collect_info, "Stop connection Succeeded.", false);

    /* Close xbee                                                            */
    xbee_shutdown(xbee);

    add_log(&pkt_Queue -> xbee_log, collect_info, "Shutdown Xbee Succeeded.", false);

    release_log_struct(&pkt_Queue -> xbee_log);

    }

/*  Data Transmission                                                        */
void CallBack(struct xbee *xbee, struct xbee_con *con, struct xbee_pkt **pkt
                                                            , void **data) {

    pkt_ptr Received_Queue = (pkt_ptr)*data;

    printf("Enter CallBack Data\n");

    if (((*pkt) -> dataLen > 0 ) && (str_to_type((*pkt) -> conType) == Data)) {

        addpkt(Received_Queue, str_to_type((*pkt) -> conType)
             , print_address((*pkt) -> address.addr64), (*pkt)->data);

        display_pkt("Receied Data", Received_Queue, Received_Queue->front);

        xbee_log(xbee, -1, "rx: [%s]\n", (*pkt)->data);

    }

}
