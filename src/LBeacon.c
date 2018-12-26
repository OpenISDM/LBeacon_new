/*
 Copyright (c) 2016 Academia Sinica, Institute of Information Science

 License:

      GPL 3.0 : The content of this file is subject to the terms and
      conditions defined in file 'COPYING.txt', which is part of this source
      code package.

 Project Name:

      BeDIS

 File Description:

      This file contains the programs executed by location beacons to
      support indoor poositioning and object tracking functions.

 File Name:

      LBeacon.c

 Version:

       1.2,  20181114

 Abstract:

      BeDIS uses LBeacons to deliver 3D coordinates and textual
      descriptions of their locations to users' devices. Basically, a
      LBeacon is an inexpensive, Bluetooth Smart Ready device. The 3D
      coordinates and location description of every LBeacon are retrieved
      from BeDIS (Building/environment Data and Information System) server
      and stored locally during deployment and maintenance times. Once
      initialized, each LBeacon broadcasts its coordinates and location
      description to Bluetooth enabled user devices within its coverage
      area.

 Authors:

      Holly Wang, hollywang@iis.sinica.edu.tw
      Jake Lee, jakelee@iis.sinica.edu.tw
      Joey Zhou, joeyzhou@iis.sinica.edu.tw
      Kenneth Tang, kennethtang@iis.sinica.edu.tw
      James Huamg, jameshuang@iis.sinica.edu.tw
      Shirley Huang, shirley.huang.93@gmail.com



*/

#include "LBeacon.h"
#include "zlog.h"
#include "Communication.h"
#include "thpool.h"

#define Debugging


ErrorCode get_config(Config *config, char *file_name) {

    /* Return value is a struct containing all config information */
    int retry_time = 0;
    FILE *file = NULL;

    /* Create spaces for storing the string of the current line being read */
    char config_setting[CONFIG_BUFFER_SIZE];
    char *config_message = NULL;


    retry_time = FILE_OPEN_RETRY;
    while(retry_time--){
        file = fopen(file_name, "r");

        if(NULL != file){
            break;
	}
    }
    if (NULL == file) {
        zlog_info(category_health_report,
                    "Error openning file");
#ifdef Debugging
        zlog_debug(category_debug,
                    "Error openning file");
#endif
        return E_OPEN_FILE;
    }

     /* Keep reading each line and store into the config struct */

    /* item 1 */
    fgets(config_setting, sizeof(config_setting), file);
    config_message = strstr((char *)config_setting, DELIMITER);
    config_message = config_message + strlen(DELIMITER);
    memcpy(config->coordinate_X, config_message,
           strlen(config_message));

    /* item 2 */
    fgets(config_setting, sizeof(config_setting), file);
    config_message = strstr((char *)config_setting, DELIMITER);
    config_message = config_message + strlen(DELIMITER);
    memcpy(config->coordinate_Y, config_message,
           strlen(config_message));

    /* item 3 */
    fgets(config_setting, sizeof(config_setting), file);
    config_message = strstr((char *)config_setting, DELIMITER);
    config_message = config_message + strlen(DELIMITER);
    memcpy(config->coordinate_Z, config_message,
           strlen(config_message));

    /* item 4 */
    fgets(config_setting, sizeof(config_setting), file);
    config_message = strstr((char *)config_setting, DELIMITER);
    config_message = config_message + strlen(DELIMITER);
    memcpy(config->file_name, config_message, strlen(config_message));

    /* item 5 */
    fgets(config_setting, sizeof(config_setting), file);
    config_message = strstr((char *)config_setting, DELIMITER);
    config_message = config_message + strlen(DELIMITER);
    memcpy(config->file_path, config_message, strlen(config_message));

    /* item 6 */
    fgets(config_setting, sizeof(config_setting), file);
    config_message = strstr((char *)config_setting, DELIMITER);
    config_message = config_message + strlen(DELIMITER);
    memcpy(config->maximum_number_of_devices, config_message,
           strlen(config_message));

    /* item 7 */
    fgets(config_setting, sizeof(config_setting), file);
    config_message = strstr((char *)config_setting, DELIMITER);
    config_message = config_message + strlen(DELIMITER);
    memcpy(config->number_of_groups, config_message,
           strlen(config_message));

    /* item 8 */
    fgets(config_setting, sizeof(config_setting), file);
    config_message = strstr((char *)config_setting, DELIMITER);
    config_message = config_message + strlen(DELIMITER);
    memcpy(config->number_of_messages, config_message,
           strlen(config_message));

    /* item 9 */
    fgets(config_setting, sizeof(config_setting), file);
    config_message = strstr((char *)config_setting, DELIMITER);
    config_message = config_message + strlen(DELIMITER);
    memcpy(config->number_of_push_dongles, config_message,
           strlen(config_message));

    /* item 10 */
    fgets(config_setting, sizeof(config_setting), file);
    config_message = strstr((char *)config_setting, DELIMITER);
    config_message = config_message + strlen(DELIMITER);
    memcpy(config->rssi_coverage, config_message,
           strlen(config_message));

    coordinate_X.f = (float)atof(config->coordinate_X);
    coordinate_Y.f = (float)atof(config->coordinate_Y);
    coordinate_Z.f = (float)atof(config->coordinate_Z);

    /* Store coordinates of the beacon */
    sprintf(config->uuid,
            "%02x%02x%02x%02x0000%02x%02x%02x%02x0000%02x%02x%02x%02x",
            coordinate_Z.b[0], coordinate_Z.b[1], coordinate_Z.b[2],
            coordinate_Z.b[3], coordinate_X.b[0], coordinate_X.b[1],
            coordinate_X.b[2], coordinate_X.b[3], coordinate_Y.b[0],
            coordinate_Y.b[1], coordinate_Y.b[2], coordinate_Y.b[3]);

#ifdef Debugging
        zlog_debug(category_debug,
                    "Generated UUID: [%s]", config->uuid);
#endif

    /* item 11 */
    fgets(config_setting, sizeof(config_setting), file);
    config_message = strstr((char *)config_setting, DELIMITER);
    config_message = config_message + strlen(DELIMITER);
    // discard the newline character at the end
    if(strlen(config_message) >= 1 && 
	config_message[strlen(config_message)-1] == '\n'){

        memcpy(config->gateway_addr, config_message, 
		strlen(config_message) - 1);
    }else{   
        memcpy(config->gateway_addr, config_message, 
		strlen(config_message));
    }

    /* item 12 */
    fgets(config_setting, sizeof(config_setting), file);
    config_message = strstr((char *)config_setting, DELIMITER);
    config_message = config_message + strlen(DELIMITER);
    config->gateway_port = atoi(config_message);

    /* item 13 */
    fgets(config_setting, sizeof(config_setting), file);
    config_message = strstr((char *)config_setting, DELIMITER);
    config_message = config_message + strlen(DELIMITER);
    config->local_client_port = atoi(config_message);

#ifdef Debugging
        zlog_debug(category_debug,
                    "Gateway conn: addr=[%s], port=[%d], client_port=[%d]", 
			config->gateway_addr, config->gateway_port, 
			config->local_client_port);
#endif

    fclose(file);

    return WORK_SUCCESSFULLY;
}

void send_to_push_dongle(bdaddr_t *bluetooth_device_address, 
			DeviceType device_type, char *name, int rssi) {

    /* Stores the MAC address as a string */
    char address[LENGTH_OF_MAC_ADDRESS];
    struct ScannedDevice *temp_node;

    /* Converts the bluetooth device address to a string */
    ba2str(bluetooth_device_address, address);
    strcat(address, "\0");

    /* Check whether the MAC address has been seen recently by the LBeacon.
       If the address is already in the list, we just update its final 
       scanned time inside the function of check_is_in_list().
    */

    if(BLE == device_type){

        temp_node = check_is_in_list(address, &BLE_object_list_head);

    }else if(BR_EDR == device_type){

        temp_node = check_is_in_list(address, &scanned_list_head);

    }else{
#ifdef Debugging
        zlog_debug(category_debug,
                    "Unknown device_type=[%d]", device_type);
#endif
        return;
    }

    if (NULL == temp_node) {

    /* The address is new. */

        /* Allocate memory from memory pool for a new node, initialize the
        node, and insert the new node to the scanned_list_head and
        BR_object_list_head if the address is that of a BR/EDR device,
        else if it is a BLE device, insert the new node into the
        BLE_object_list_head. */

#ifdef Debugging
        zlog_debug(category_debug,
            "******Get the memory from the pool. ****** ");
        zlog_debug(category_debug,
            "device_type[%d]: %17s - %20s - RSSI %4d", 
		device_type, address, name, rssi);
#endif

        temp_node = (struct ScannedDevice*) mp_alloc(&mempool);


        /* Initialize the list entries */
        init_entry(&temp_node->sc_list_entry);
        init_entry(&temp_node->tr_list_entry);

        /* Get the initial scan time for the new node. */
        temp_node->initial_scanned_time = get_system_time();
        temp_node->final_scanned_time = temp_node->initial_scanned_time;

        /* Copy the MAC address to the node */
        strncpy(temp_node->scanned_mac_address,
                address,
                LENGTH_OF_MAC_ADDRESS);

        /* Insert the new node into the right lists. */
        pthread_mutex_lock(&list_lock);

        if(BLE == device_type){

            /* Insert the new node to the BLE_object_list_head */
            insert_list_first(&temp_node->tr_list_entry,
                            &BLE_object_list_head.list_entry);

        }else if(BR_EDR == device_type){

            /* Insert the new node to the scanned list */
            insert_list_first(&temp_node->sc_list_entry,
                            &scanned_list_head.list_entry);

            /* Insert the new node to the BR_object_list_head  */
            insert_list_first(&temp_node->tr_list_entry,
                            &BR_object_list_head.list_entry);

        }

        pthread_mutex_unlock(&list_lock);

    }
    return;
}



struct ScannedDevice *check_is_in_list(char address[],
                                       ObjectListHead *list) {

    /* Create a temporary list pointer and set as the head */
    struct List_Entry *list_pointers;
    ScannedDevice *temp = NULL;
    int number_in_list = get_list_length(&list->list_entry);
    int len = 0;
    char *addr_last_digits = NULL;
    char *temp_last_digits = NULL;
    bool temp_is_null = true;
    bool is_empty = false;

    /* If there is no node in the list, reutrn NULL directly. */
    pthread_mutex_lock(&list_lock);

    is_empty = is_entry_list_empty(&list->list_entry);

    pthread_mutex_unlock(&list_lock);

    if(is_empty){
        return NULL;
    }

    
    /* Go through the list to check whether the input address is in the list.
    */

    pthread_mutex_lock(&list_lock);

    list_for_each(list_pointers, &list->list_entry) {
        /* According to the device type stored in the list, get the 
	specific data */
        switch(list->device_type){

            case BR_EDR:

                temp = ListEntry(list_pointers, ScannedDevice,
                                 sc_list_entry);
                break;

            case BLE:

                temp = ListEntry(list_pointers, ScannedDevice,
                                 tr_list_entry);
                break;

            default:
		break;
        }

        len = strlen(address);

        addr_last_digits = &address[len - NUM_DIGITS_TO_COMPARE];
        temp_last_digits =
                     &temp->scanned_mac_address[len - NUM_DIGITS_TO_COMPARE];

        /* Compare the first and the last digits of the MAC address */
        if ((false == strncmp(address, temp->scanned_mac_address,
                      NUM_DIGITS_TO_COMPARE))&&
            (false == strncmp(addr_last_digits, temp_last_digits,
                      NUM_DIGITS_TO_COMPARE))) {

            /* Update the final scan time */
            temp->final_scanned_time = get_system_time();
            temp_is_null = false;
            break;
        }
    }

    pthread_mutex_unlock(&list_lock);

    if(temp_is_null == true){
        return NULL;
    }

    return temp;

}



ErrorCode enable_advertising(int advertising_interval,
                             char *advertising_uuid,
                 int major_number,
                 int minor_number,
                             int rssi_value) {

    int dongle_device_id = 0;
    int device_handle = 0;
    int retry_time = 0;
    uint8_t status;
    struct hci_request request;
    int return_value = 0;
    uint8_t segment_length = 1;
    unsigned int *uuid = NULL;
    int uuid_iterator;

    /* Open Bluetooth device */
    retry_time = DONGLE_GET_RETRY;
    while(retry_time--){
        dongle_device_id = hci_get_route(NULL);

        if(dongle_device_id >= 0){
            break;
	}
    }
    if (dongle_device_id < 0){
        zlog_info(category_health_report,
		"Error openning the device");
#ifdef Debugging
        zlog_debug(category_debug,
		"Error openning the device");
#endif
        return E_OPEN_DEVICE;
    }


    retry_time = SOCKET_OPEN_RETRY;
    while(retry_time--){
        device_handle = hci_open_dev(dongle_device_id);

        if(device_handle >= 0){
            break;
	}
    }
    if (device_handle < 0) {
        zlog_info(category_health_report,
		"Error openning socket");
#ifdef Debugging
        zlog_debug(category_debug,
		"Error openning socket");
#endif
        return E_OPEN_DEVICE;
    }

    le_set_advertising_parameters_cp advertising_parameters_copy;
    memset(&advertising_parameters_copy, 0,
        sizeof(advertising_parameters_copy));
    advertising_parameters_copy.min_interval = htobs(advertising_interval);
    advertising_parameters_copy.max_interval = htobs(advertising_interval);
    advertising_parameters_copy.chan_map = 7;

    memset(&request, 0, sizeof(request));
    request.ogf = OGF_LE_CTL;
    request.ocf = OCF_LE_SET_ADVERTISING_PARAMETERS;
    request.cparam = &advertising_parameters_copy;
    request.clen = LE_SET_ADVERTISING_PARAMETERS_CP_SIZE;
    request.rparam = &status;
    request.rlen = 1;

    return_value = hci_send_req(device_handle, &request,
                                    HCI_SEND_REQUEST_TIMEOUT_IN_MS);

    if (return_value < 0) {

        /* Error handling */
        hci_close_dev(device_handle);

#ifdef Debugging

        zlog_debug(category_debug,
                   "Can't send request %s (%d)", strerror(errno),
                   errno);

#endif
        zlog_info(category_health_report,
                  "Can't send request %s (%d)", strerror(errno),
                  errno);

        return E_SEND_REQUEST_TIMEOUT;

    }

    le_set_advertise_enable_cp advertisement_copy;
    memset(&advertisement_copy, 0, sizeof(advertisement_copy));
    advertisement_copy.enable = 0x01;

    memset(&request, 0, sizeof(request));
    request.ogf = OGF_LE_CTL;
    request.ocf = OCF_LE_SET_ADVERTISE_ENABLE;
    request.cparam = &advertisement_copy;
    request.clen = LE_SET_ADVERTISE_ENABLE_CP_SIZE;
    request.rparam = &status;
    request.rlen = 1;

     return_value = hci_send_req(device_handle, &request,
                                HCI_SEND_REQUEST_TIMEOUT_IN_MS);

    if (return_value < 0) {

        /* Error handling */
        hci_close_dev(device_handle);

#ifdef Debugging
        zlog_debug(category_debug,
                   "Can't send request %s (%d)", strerror(errno),
                   errno);
#endif
        zlog_info(category_health_report,
                  "Can't send request %s (%d)", strerror(errno),
                  errno);
        return E_SEND_REQUEST_TIMEOUT;
    }

    le_set_advertising_data_cp advertisement_data_copy;
    memset(&advertisement_data_copy, 0, sizeof(advertisement_data_copy));

    segment_length = 1;
    advertisement_data_copy
        .data[advertisement_data_copy.length + segment_length] =
        htobs(EIR_FLAGS);
    segment_length++;
    advertisement_data_copy
        .data[advertisement_data_copy.length + segment_length] = htobs(0x1A);
    segment_length++;
    advertisement_data_copy.data[advertisement_data_copy.length] =
        htobs(segment_length - 1);

    advertisement_data_copy.length += segment_length;

    segment_length = 1;
    advertisement_data_copy
        .data[advertisement_data_copy.length + segment_length] =
        htobs(EIR_MANUFACTURE_SPECIFIC_DATA);
    segment_length++;
    advertisement_data_copy
        .data[advertisement_data_copy.length + segment_length] = htobs(0x4C);
    segment_length++;
    advertisement_data_copy
        .data[advertisement_data_copy.length + segment_length] = htobs(0x00);
    segment_length++;
    advertisement_data_copy
        .data[advertisement_data_copy.length + segment_length] = htobs(0x02);
    segment_length++;
    advertisement_data_copy
        .data[advertisement_data_copy.length + segment_length] = htobs(0x15);
    segment_length++;

    uuid = uuid_str_to_data(advertising_uuid);

    for (uuid_iterator = 0; uuid_iterator < strlen(advertising_uuid) / 2;
        uuid_iterator++) {
        advertisement_data_copy
            .data[advertisement_data_copy.length + segment_length] =
            htobs(uuid[uuid_iterator]);
        segment_length++;

    }

    /* Major number */
    advertisement_data_copy
        .data[advertisement_data_copy.length + segment_length] =
        htobs(major_number >> 8 & 0x00FF);
    segment_length++;
    advertisement_data_copy
        .data[advertisement_data_copy.length + segment_length] =
        htobs(major_number & 0x00FF);
    segment_length++;

    /* Minor number */
    advertisement_data_copy
        .data[advertisement_data_copy.length + segment_length] =
        htobs(minor_number >> 8 & 0x00FF);
    segment_length++;
    advertisement_data_copy
        .data[advertisement_data_copy.length + segment_length] =
        htobs(minor_number & 0x00FF);
    segment_length++;


    /* RSSI calibration */
    advertisement_data_copy
        .data[advertisement_data_copy.length + segment_length] =
        htobs(twoc(rssi_value, 8));
    segment_length++;

    advertisement_data_copy.data[advertisement_data_copy.length] =
        htobs(segment_length - 1);

    advertisement_data_copy.length += segment_length;

    memset(&request, 0, sizeof(request));
    request.ogf = OGF_LE_CTL;
    request.ocf = OCF_LE_SET_ADVERTISING_DATA;
    request.cparam = &advertisement_data_copy;
    request.clen = LE_SET_ADVERTISING_DATA_CP_SIZE;
    request.rparam = &status;
    request.rlen = 1;

    return_value = hci_send_req(device_handle, &request,
                                HCI_SEND_REQUEST_TIMEOUT_IN_MS);

    hci_close_dev(device_handle);

    if (return_value < 0) {
        /* Error handling */

#ifdef Debugging
        zlog_debug(category_debug,
                   "Can't send request %s (%d)", strerror(errno),
                   errno);
#endif
        zlog_info(category_health_report,
                  "Can't send request %s (%d)", strerror(errno),
                  errno);

        return E_SEND_REQUEST_TIMEOUT;
    }

    if (status) {
        /* Error handling */

#ifdef Debugging
        zlog_debug(category_debug,
                   "LE set advertise returned status %d", status);
#endif
        zlog_info(category_health_report,
                  "LE set advertise returned status %d", status);

        return E_ADVERTISE_STATUS;
    }

    return WORK_SUCCESSFULLY;
}



ErrorCode disable_advertising() {

    int dongle_device_id = 0;
    int device_handle = 0;
    int retry_time = 0;
    uint8_t status;
    struct hci_request request;
    int return_value = 0;

    /* Open Bluetooth device */
    retry_time = DONGLE_GET_RETRY;
    while(retry_time--){
        dongle_device_id = hci_get_route(NULL);

        if(dongle_device_id >= 0){
            break;
	}
    }

    if (dongle_device_id < 0) {
        zlog_info(category_health_report,
		"Error openning the device");
#ifdef Debugging
        zlog_debug(category_debug,
		"Error openning the device");
#endif
        return E_OPEN_DEVICE;
    }

    retry_time = SOCKET_OPEN_RETRY;
    while(retry_time--){
        device_handle = hci_open_dev(dongle_device_id);

        if(device_handle >= 0){
            break;
	}
    }

    if (device_handle < 0) {
        zlog_info(category_health_report,
		"Error openning socket");
#ifdef Debugging
        zlog_debug(category_debug,
		"Error openning socket");
#endif
        return E_OPEN_DEVICE;
    }

    le_set_advertise_enable_cp advertisement_copy;

    memset(&advertisement_copy, 0, sizeof(advertisement_copy));

    memset(&request, 0, sizeof(request));
    request.ogf = OGF_LE_CTL;
    request.ocf = OCF_LE_SET_ADVERTISE_ENABLE;
    request.cparam = &advertisement_copy;
    request.clen = LE_SET_ADVERTISE_ENABLE_CP_SIZE;
    request.rparam = &status;
    request.rlen = 1;

    return_value = hci_send_req(device_handle, &request,
                                    HCI_SEND_REQUEST_TIMEOUT_IN_MS);

    hci_close_dev(device_handle);

    if (return_value < 0) {

        /* Error handling */
#ifdef Debugging
        zlog_debug(category_debug,
                   "Can't set advertise mode: %s (%d)",
                   strerror(errno), errno);
#endif
        zlog_info(category_health_report,
                  "Can't set advertise mode: %s (%d)",
                  strerror(errno), errno);

        return E_ADVERTISE_MODE;
    }

    if (status) {

        /* Error handling */
#ifdef Debugging
        zlog_debug(category_debug,
                   "LE set advertise enable on returned status %d",
                   status);
#endif
        zlog_info(category_health_report,
                  "LE set advertise enable on returned status %d",
                  status);

        return E_ADVERTISE_STATUS;
    }

    return WORK_SUCCESSFULLY;

}

void *cleanup_scanned_list(void* param) {
    struct List_Entry *list_pointers, *save_list_pointers;
    ScannedDevice *temp;
    bool is_empty = false;

#ifdef Debugging
    zlog_debug(category_debug,
        ">> cleanup_scanned_list ");
#endif

    while (true == ready_to_work) {
	/* Use pthread mutex, cond and signal to control the flow, 
	instead of using busy while loop and sleep mechanism.
	*/
	pthread_mutex_lock(&exec_lock);

	while(true != reach_cln_scanned_list)
	{
	    pthread_cond_wait(&cond_cln_scanned_list, &exec_lock);
	}
	reach_cln_scanned_list = false;

	pthread_mutex_unlock(&exec_lock);


#ifdef Debugging
        zlog_debug(category_debug,
            "cleanup scanned list in cleanup_scanned_list function");
#endif	


	pthread_mutex_lock(&list_lock);
        
        if(false == is_entry_list_empty(&scanned_list_head.list_entry)){
   
            list_for_each_safe(list_pointers,
                           save_list_pointers,
                           &scanned_list_head.list_entry){

                temp = ListEntry(list_pointers, ScannedDevice, sc_list_entry);

                /* If the device has been in the scanned list for at least
		INTERVAL_FOR_CLEANUP_SCANNED_LIST_IN_SEC
                seconds, remove its struct node from the scanned list 
                */
                if (get_system_time() - temp->initial_scanned_time >
                    INTERVAL_FOR_CLEANUP_SCANNED_LIST_IN_SEC){

                    remove_list_node(&temp->sc_list_entry);

                    /* If the node no longer is in the BR_object_list_head,
                    free the space back to the memory pool. 
                    */
                    if(is_isolated_node(&temp->tr_list_entry)){
            	        mp_free(&mempool, temp);
                    }
                }
	    }
        }

        pthread_mutex_unlock(&list_lock);
        
        cln_scanned_list_last_time = get_system_time();

    }//#end while

#ifdef Debugging
    zlog_debug(category_debug,
        "<< cleanup_scanned_list ");
#endif

}// cleanup_scanned_list


int beacon_basic_info(char *message, size_t message_size, int polled_type){

    char basic_info[MAX_LENGTH_RESP_BASIC_INFO]; 
    
    // 1. packet type 
    message[0] = 0x0F & polled_type;
    message[1] = '\0';

    // 2. LBeacon part
    strcat(message, BEACON_BASIC_PREFIX);
    
    // LBeacon UUID
    strcat(message, g_config.uuid);
    strcat(message, ";");

    // LBeacon major and minor version
    memset(basic_info, 0, sizeof(basic_info));
    sprintf(basic_info, "%d.%d;", LBEACON_MAJOR_VER, LBEACON_MINOR_VER);
    strcat(message, basic_info);
    
    // 3. Gateway part
    strcat(message, GATEWAY_BASIC_PREFIX);
    // Gateway IP address
    strcat(message, g_config.gateway_addr);
    strcat(message, ";");
   
    /* 4. Make sure the resulted message (basic information) does not 
	exceed our expected length
    */
    if(strlen(message) > MAX_LENGTH_RESP_BASIC_INFO){
#ifdef Debugging
        zlog_debug(category_debug,
            "Error in beacon_basic_info(), the length of basic information "
            "is [%d], and limitation is [%d].",
	    strlen(message), MAX_LENGTH_RESP_BASIC_INFO);
#endif
	return 1;
    }

    return 0;   
}


void *manage_communication(void* param){
    Threadpool thpool;
    int id = 0;

    long long gateway_latest_time = 0;
    int polled_type;
    char message[MESSAGE_LENGTH];
    
    FILE *br_object_file = NULL;
    FILE *ble_object_file = NULL;
    bool is_br_empty = false;
    bool is_ble_empty = false;
    char msg_temp_one[MESSAGE_LENGTH];
    char msg_temp_two[MESSAGE_LENGTH];
    int max_objects = 0;
    int used_objects = 0;

    FILE *health_file = NULL;

    int retry_time = 0;
    int ret_val = 0;

#ifdef Debugging
    zlog_debug(category_debug,
        ">> manage_communication ");
#endif

    /* Initialize the thread pool and worker threads 
    */
    thpool = thpool_init(NUM_WORK_THREADS);
    if(NULL != thpool){
	/*Evenly assign the worker threads to own the jobs of 
	send/recieve data to/from gateway.
        */		
        for(id = 0; id< NUM_WORK_THREADS; id++){
	    if(id % 2 == 0){
	    	thpool_add_work(thpool,(void*)send_data, 
			(sudp_config_beacon *) &udp_config, 0);
	    }else{
	    	thpool_add_work(thpool,(void*)receive_data, 
			(sudp_config_beacon *) &udp_config, 0);
	    }
        }
    }else{
        zlog_info(category_health_report,
	    "Unable to initialize thread pool in manage_communication");	
#ifdef Debugging
        zlog_debug(category_debug,
	    "Unable to initialize thread pool in manage_communication");	
#endif
	return;
    } // if-else
              
 
    while(true == ready_to_work){
	/* sleep a short time to prevent occupying CPU in this busy 
	while loop.
	*/	
	sleep(INTERVAL_FOR_BUSY_WAITING_CHECK_IN_SEC);


	/* If LBeacon has not got pakcet from gateway for 5 minutes, LBeacon 
	   sends request_to_join to gateway again. The purpose is to handle 
	   the gateway upgrade cases in which gateway might not keep the 
	   registered LBeacon ID map.
	*/
	if(get_system_time() - gateway_latest_time > 
		INTERVAL_RECEIVE_MESSAGE_FROM_GATEWAY_IN_SEC){
#ifdef Debugging
            zlog_debug(category_debug,
	        "Send requets_to_join to gateway again");	
#endif

            memset(message, 0, sizeof(message)); 
	    if(0 != beacon_basic_info(message, sizeof(message), 
			request_to_join)){
#ifdef Debugging
            zlog_debug(category_debug,
	        "Unable to prepare basic information for response. "
		"Abort sending this response to gateway.");	
#endif	
		continue;	
	    }

	    ret_val = addpkt(&udp_config.send_pkt_queue, 
			UDP, udp_config.send_ipv4_addr, 
			message, sizeof(message));

	    if(pkt_Queue_SUCCESS != ret_val)
	    {
        	zlog_info(category_health_report,
	    	    "Unable to add packet to queue, error=[%d]", 
		    ret_val);
#ifdef Debugging
        	zlog_debug(category_debug,
	    	    "Unable to add packet to queue, error=[%d]", 
		    ret_val);
#endif
	    }

	    /* Because network connection has failed for long time, we 
		should notify timeout_cleanup thread to remove nodes from 
		all lists.
	    */
	    pthread_mutex_lock(&exec_lock);

	    reach_cln_all_lists = true;
	    pthread_cond_signal(&cond_cln_all_lists);

	    pthread_mutex_unlock(&exec_lock);
	}


        /* Check call back from the gateway. If not polled by gateway. 
        */
        polled_type = undefined;
        if(true == is_null(&udp_config.recv_pkt_queue)){
	    /* continue to next iteration if receive packet queue is empty.
	    */
	    continue;
	}else{
	    /* Update gateway_latest_time to make LBeacon aware that its 
	       connection to gateway is still okay.
            */
	    gateway_latest_time = get_system_time();
	
            /* 
	       Get one packet from receive packet queue
	    */
            sPkt tmp_pkt = get_pkt(&udp_config.recv_pkt_queue);
            polled_type = 0x0F & tmp_pkt.content[0];    
        }

	/* According to the polled data type, prepare a work item */
        switch(polled_type){
	    case join_request_ack:
#ifdef Debugging
        	zlog_debug(category_debug,
	    	    "Receive join_request_ack from gateway");
#endif
		break;

            case tracked_object_data:
#ifdef Debugging
        	zlog_debug(category_debug,
	    	    "Receive tracked_object_data from gateway");
#endif
  		/* return directly, if both BR and BLE tracked list is 
		emtpy 
		*/
                pthread_mutex_lock(&list_lock);

		is_br_empty = 
		    is_entry_list_empty(&BR_object_list_head.list_entry);
                is_ble_empty = 
 		    is_entry_list_empty(&BLE_object_list_head.list_entry);

	        pthread_mutex_unlock(&list_lock);

	        if(is_br_empty && is_ble_empty){
#ifdef Debugging
        	zlog_debug(category_debug,
	    	    "Both BR and BLE lists are empty.");
#endif
		    continue;
		}


                /* Copy track_object data to a file to be transmited */
		memset(message, 0, sizeof(message));
		memset(msg_temp_one, 0, sizeof(msg_temp_one));
		memset(msg_temp_two, 0, sizeof(msg_temp_two));

		used_objects = 0;
		max_objects = (sizeof(message) - 
				MAX_LENGTH_RESP_BASIC_INFO - 
				used_objects * MAX_LENGTH_RESP_DEVICE_INFO)/
				MAX_LENGTH_RESP_DEVICE_INFO;

     		if(WORK_SUCCESSFULLY !=
			consolidate_tracked_data(&BR_object_list_head,
						msg_temp_one,
						sizeof(msg_temp_one),
						max_objects,
						&used_objects)){

#ifdef Debugging
        	zlog_debug(category_debug,
	    	    "Unable to consolidate BR_EDR devices, abort BR_EDR "
		    "devices this time.");
#endif	
		    /*clean up message buffer again to have clear space.*/
		    memset(msg_temp_one, 0, sizeof(msg_temp_one));
		     	
		}

		max_objects = (sizeof(message) - 
				MAX_LENGTH_RESP_BASIC_INFO - 
				used_objects * MAX_LENGTH_RESP_DEVICE_INFO)/
				MAX_LENGTH_RESP_DEVICE_INFO;

		if(WORK_SUCCESSFULLY !=
		        consolidate_tracked_data(&BLE_object_list_head,
						msg_temp_two,
						sizeof(msg_temp_two),
						max_objects,
						&used_objects)){
#ifdef Debugging
        	zlog_debug(category_debug,
	    	    "Unable to consolidate BLE devices, abort BR_EDR "
		    "devices this time.");
#endif	
		    /*clean up message buffer again to have clear space.*/
		    memset(msg_temp_two, 0, sizeof(msg_temp_two));	
		}

	        if(0 != beacon_basic_info(message, sizeof(message), 
				tracked_object_data)){
#ifdef Debugging
                    zlog_debug(category_debug,
	        	"Unable to prepare basic information for response. "
			"Abort sending this response to gateway.");	
#endif	
		    continue;	
		}

                if(sizeof(message) > strlen(message) + 
					strlen(msg_temp_one) + 
					strlen(msg_temp_two)){

               	    strcat(message, msg_temp_one);
		    strcat(message, msg_temp_two);
		    ret_val = addpkt(&udp_config.send_pkt_queue, UDP, 
 				    udp_config.send_ipv4_addr, 
					message, sizeof(message)); 

	    	    if(pkt_Queue_SUCCESS != ret_val)
	    	    {
        		zlog_info(category_health_report,
	    	    	    "Unable to add packet to queue, error=[%d]", 
			    ret_val);
#ifdef Debugging
        		zlog_debug(category_debug,
	    	    	    "Unable to add packet to queue, error=[%d]", 
			    ret_val);
#endif
	    	    }

		}else{
        		zlog_info(category_health_report,
	    	    	    "Abort BR/BLE tracked data, because there is "
			    "potential buffer overflow. strlen(message)=%d, "
			    "strlen(msg_temp_one)=%d,strlen(msg_temp_two)=%d",
			     strlen(message), strlen(msg_temp_one), 
			     strlen(msg_temp_two));
#ifdef Debugging
        		zlog_debug(category_debug,
	    	    	    "Abort BR/BLE tracked data, because there is "
			    "potential buffer overflow. strlen(message)=%d, "
			    "strlen(msg_temp_one)=%d,strlen(msg_temp_two)=%d",
			     strlen(message), strlen(msg_temp_one), 
			     strlen(msg_temp_two));
#endif
		} // if-else
		
		break;

            case health_report:
#ifdef Debugging
        	zlog_debug(category_debug,
	    	    "Receive health_report from gateway");
#endif

    		retry_time = FILE_OPEN_RETRY;
    		while(retry_time--){
        	    health_file = 
			fopen(HEALTH_REPORT_LOG_FILE_NAME, "r");

		    if(NULL != health_file){
            		break;
		    }
    		}

		if(NULL == health_file){
       		    zlog_info(category_health_report,
	    	    	"Error openning file");
#ifdef Debugging
        	    zlog_debug(category_debug,
	    	    	"Error openning file");
#endif
			
		}else{
                    /* contruct the content for UDP packet*/
                    memset(message, 0, sizeof(message));
	            
		    if(0!=beacon_basic_info(message, sizeof(message), 
				health_report)){		
#ifdef Debugging
	                zlog_debug(category_debug,
	        	    "Unable to prepare basic information for "
			    "response. Abort sending this response to "
			    "gateway.");	
#endif	
    			continue;	
		    }

  		    /* read health report data to temp buffer*/
		    memset(msg_temp_one, 0, sizeof(msg_temp_one));

	            fread(msg_temp_one, 
			sizeof(msg_temp_one) - strlen(message) - 1, 
			sizeof(char), health_file); 

		    fclose(health_file);
		    
                    if(sizeof(message) > strlen(message) + 
					strlen(msg_temp_one)){

               	        strcat(message, msg_temp_one);

		        ret_val = addpkt(&udp_config.send_pkt_queue, UDP, 
			    		udp_config.send_ipv4_addr, 
					message, sizeof(message)); 

	    	        if(pkt_Queue_SUCCESS != ret_val)
	    	        {
        		    zlog_info(category_health_report,
	    	    	        "Unable to add packet to queue, error=[%d]", 
				ret_val);
#ifdef Debugging
        		    zlog_debug(category_debug,
	    	    	        "Unable to add packet to queue, error=[%d]", 
				ret_val);
#endif
		        }

		    }else{

        		zlog_info(category_health_report,
	    	    	    "Abort health report data, because there is "
			    "potential buffer overflow. strlen(message)=%d, "
			    "strlen(msg_temp_one)=%d", 
			    strlen(message), strlen(msg_temp_one));

#ifdef Debugging
        		zlog_debug(category_debug,
	    	    	    "Abort health report data, because there is "
			    "potential buffer overflow. strlen(message)=%d, "
			    "strlen(msg_temp_one)=%d",
			    strlen(message), strlen(msg_temp_one));
#endif
  		    } // if-else
		}

                break;

            default:
#ifdef Debugging
        	zlog_debug(category_debug,
	    	    "Receive unknown packet type=[%d] from gateway", 
			polled_type);
#endif
                break;

        }



    } // end of the while


    /* Free the thread pool */
    thpool_destroy(thpool);

#ifdef Debugging
    zlog_debug(category_debug,
    "<< manage_communication ");
#endif

}

ErrorCode copy_object_data_to_file(char *file_name, 
				ObjectListHead *list, 
				const int max_num_objects, 
				int *used_objects ) {

    FILE *track_file = NULL;;
    struct List_Entry *list_pointers, *head_pointers, *tail_pointers;
    ScannedDevice *temp;
    int number_in_list;
    int number_to_send;
    char timestamp_initial_str[LENGTH_OF_EPOCH_TIME];
    char timestamp_final_str[LENGTH_OF_EPOCH_TIME];
    char basic_info[MAX_LENGTH_RESP_BASIC_INFO];
    int retry_time = 0;
    int node_count;
    unsigned timestamp_init;
    unsigned timestamp_end;
    /* Head of a local list for tracked object */
    List_Entry local_list_entry;


    /* Check the input parameter if is valid */
    if(list != &BR_object_list_head &&
       list != &BLE_object_list_head){

        return E_INPUT_PARAMETER;
    }


    DeviceType device_type = list->device_type;

    retry_time = FILE_OPEN_RETRY;
    while(retry_time--){
        track_file = fopen(file_name, "w");

        if(NULL != track_file){
            break;
	}
    }
    if(NULL == track_file){

        retry_time = FILE_OPEN_RETRY;
        while(retry_time--){
            track_file = fopen(file_name, "wt");

            if(NULL != track_file){
                break;
	    }
        }
    }
    if(NULL == track_file){
        zlog_info(category_health_report,
            "Error openning file");
#ifdef Debugging
        zlog_debug(category_debug,
            "Error openning file");
#endif
        return E_OPEN_FILE;

    }

    /* Get the number of objects with data to be transmitted */
    number_in_list = get_list_length(&list->list_entry);
    number_to_send = min(max_num_objects, number_in_list);
    *used_objects = number_to_send;

    /*Check if number_to_send is zero. If yes, no need to do more; close
    file and return */
    if(0 == number_to_send){

       fclose(track_file);
       return WORK_SUCCESSFULLY;

    }

    /* Insert device_type and number_to_send at the struct of the track
    file */
    sprintf(basic_info, "%d;%d;", device_type, number_to_send);
    fputs(basic_info, track_file);

#ifdef Debugging
    zlog_debug(category_debug, 
	"Device type: %d; Number to send: %d",
	device_type, number_to_send);
#endif


    pthread_mutex_lock(&list_lock);

/* This code block is for debugging the linked list operations. In release 
version, we should not waste resource in iterating the linked list only 
ensure the correctnedd.

#ifdef Debugging

    list_for_each(list_pointers, &list->list_entry){
    	zlog_debug(category_debug,
	"Input list: list->list_entry %d list_pointers %d prev %d next %d",
		&list->list_entry,
		list_pointers,
		list_pointers->prev,
		list_pointers->next);
    }

#endif
*/

    /* Set temporary pointer to point to the head of the input list */
    head_pointers = list->list_entry.next;
    list_pointers = list->list_entry.next;

    /* Go through the input tracked_object list to move 
    number_to_send nodes in the list to local list */
    for (node_count = 1; node_count <= number_to_send;
		list_pointers = list_pointers->next, node_count++){

        /* If the node is the last in the list */
        if(node_count == number_to_send){

            /* Set a marker for the last pointer of the last node */
            tail_pointers = list_pointers;

        }
    }


    /* Set the head of the input list to point to the last node */
    list->list_entry.next = tail_pointers->next;
    tail_pointers->next->prev = &list->list_entry;


    /* Initilize the local list */
    init_entry(&local_list_entry);
    local_list_entry.next = head_pointers;
    head_pointers->prev = &local_list_entry;
    local_list_entry.prev = tail_pointers;
    tail_pointers->next = &local_list_entry;

    pthread_mutex_unlock(&list_lock);



/* This code block is for debugging the linked list operations. In release 
version, we should not waste resource in iterating the linked list only 
ensure the correctnedd.

#ifdef Debugging

      list_for_each(list_pointers, &local_list_entry){
    	zlog_debug(category_debug,
		"local list:  list_pointers %d prev %d next %d",
			list_pointers,
			list_pointers->prev,
			list_pointers->next);
      }

#endif
*/

    /* Go throngh the local object list to get the content and write the
       content to file */
     list_for_each(list_pointers, &local_list_entry){

        temp = ListEntry(list_pointers, ScannedDevice, tr_list_entry);

        /* sprintf() is the function to set a format and convert the
           datatype to char */
        sprintf(timestamp_initial_str, "%llu", temp->initial_scanned_time);
        sprintf(timestamp_final_str, "%llu", temp->final_scanned_time);

        /* Write the content to the file */
        fputs(temp->scanned_mac_address, track_file);
        fputs(";", track_file);
        fputs(timestamp_initial_str, track_file);
        fputs(";", track_file);
        fputs(timestamp_final_str, track_file);
        fputs(";", track_file);

    }

    /* Remove nodes from the local list and release memory allocated to
       nodes that are also not in scanned_device_list */
    free_tracked_list(&local_list_entry, device_type);

    /* Close the file for storing data in the input list */
    fclose(track_file);

    return WORK_SUCCESSFULLY;

}


ErrorCode consolidate_tracked_data(ObjectListHead *list, 
					char *msg_buf, 
					size_t msg_size,
					const int max_num_objects,
					int *used_objects){

    ErrorCode ret_val;
    FILE *file_fd = NULL;
    char *file_name = NULL;
    int retry_time = 0;

    /* Check the input parameter if is valid */
    if(list != &BR_object_list_head &&
        list != &BLE_object_list_head){
#ifdef Debugging
        zlog_debug(category_debug, 
	    "Error of invalid input parameter, list is neither BR "
	    "nor BLE list");
#endif

        return E_INPUT_PARAMETER;
    }

    if(BR_EDR == list->device_type)
	file_name = TRACKED_BR_TXT_FILE_NAME;
    else if(BLE == list->device_type)
	file_name = TRACKED_BLE_TXT_FILE_NAME;
    else{
#ifdef Debugging
        zlog_debug(category_debug, 
	    "Error of invalid input parameter, list device_type=[%d]",
	    list->device_type);
#endif
	return E_INPUT_PARAMETER;
    }


    ret_val =
	copy_object_data_to_file(file_name, list, 
		max_num_objects, used_objects);

    if(WORK_SUCCESSFULLY == ret_val){

        /* Open the file that is going to be sent to the gateway */
	retry_time = FILE_OPEN_RETRY;
        while(retry_time--){
            file_fd = fopen(file_name, "r");

            if(NULL != file_fd){
                break;
	    }
        }
        if (NULL == file_fd){
            zlog_info(category_health_report, 
	    "Error openning file");
#ifdef Debugging
            zlog_debug(category_debug, 
	    "Error openning file");
#endif
	    return E_OPEN_FILE;
	}

        fgets(msg_buf, msg_size, file_fd);
    	fclose(file_fd);
    }

    return ret_val;

}

void free_tracked_list(List_Entry *list_entry, DeviceType device_type){

    struct List_Entry *list_pointers, *save_list_pointers;
    ScannedDevice *temp;


    pthread_mutex_lock(&list_lock);

    if(false == is_entry_list_empty(list_entry)){
        list_for_each_safe(list_pointers,
                           save_list_pointers,
                           list_entry){

    	    temp = ListEntry(list_pointers, ScannedDevice, tr_list_entry);

            remove_list_node(&temp->tr_list_entry);

            if(BLE == device_type){

                mp_free(&mempool, temp);

            }else{

                /* If the node is no longer in scanned list, return 
		the space back to the memory pool. */
	        if(is_isolated_node(&temp->sc_list_entry)){
                    mp_free(&mempool, temp);
                }

            }
        }
    }

    pthread_mutex_unlock(&list_lock);

    return;
}

void cleanup_list(ObjectListHead *list, bool is_scanned_list_head){

    struct List_Entry *list_pointers, *save_list_pointers;
    ScannedDevice *temp;

    pthread_mutex_lock(&list_lock);

    /*Check whether the list is empty */
    if(false == is_entry_list_empty(&list->list_entry)){
        /* Go throgth lists to release all memory allocated to the
        nodes */

        list_for_each_safe(list_pointers,
                           save_list_pointers,
                           &list->list_entry){

        /* If the input list_entry is used for scanned list, we should 
           remove the node from sc_list_entry first. Otherwise, we remove 
           the node from tr_list_entry.
        */
        if(is_scanned_list_head){
            temp = ListEntry(list_pointers, ScannedDevice, sc_list_entry);
            remove_list_node(&temp->sc_list_entry);
        }
            temp = ListEntry(list_pointers, ScannedDevice, tr_list_entry);
            remove_list_node(&temp->tr_list_entry);
        }
   
        /* Because both scanned_list_head and BR_object_list_head use the 
	   same node for two list_entry (sc_list_entry and tr_list_entry), 
           if the device_type is BR_EDR, we should make sure the node is 
           removed from the other list as well. 
        */
        if(BR_EDR == list->device_type){
	    if(is_scanned_list_head){
		if(false == is_isolated_node(&temp->tr_list_entry)){
		    remove_list_node(&temp->tr_list_entry);
		}
            }else{
		if(false == is_isolated_node(&temp->sc_list_entry)){
		    remove_list_node(&temp->sc_list_entry);
		}
            }
	}	

	mp_free(&mempool, temp);

    }
    pthread_mutex_unlock(&list_lock);


    return;
}


/* A static struct function that returns specific bluetooth BLE request. */
const struct hci_request ble_hci_request(uint16_t ocf, 
					int clen,
					void * status, 
					void * cparam)
{
    struct hci_request rq;
    memset(&rq, 0, sizeof(rq));
    rq.ogf = OGF_LE_CTL;
    rq.ocf = ocf;
    rq.cparam = cparam;
    rq.clen = clen;
    rq.rparam = status;
    rq.rlen = 1;

    return rq;
}

/* A static function for prase the name from the BLE device. */
static void eir_parse_name(uint8_t *eir, size_t eir_len,
                        char *buf, size_t buf_len)
{
    size_t offset;

    offset = 0;
    while (offset < eir_len) {
        uint8_t field_len = eir[0];
        size_t name_len;

        /* Check for the end of EIR */
        if (field_len == 0)
            break;

        if (offset + field_len > eir_len)
            goto failed;

        switch (eir[1]) {

            case EIR_NAME_SHORT:
            case EIR_NAME_COMPLETE:

                name_len = field_len - 1;

                if (name_len > buf_len)
                    goto failed;

                memcpy(buf, &eir[2], name_len);
                return;
            }

        offset += field_len + 1;
        eir += field_len + 1;
    }

failed:
    snprintf(buf, buf_len, NULL);
}


void *start_ble_scanning(void *param){
    uint8_t ble_buffer[HCI_MAX_EVENT_SIZE]; /*A buffer for the
                                            callback event */
    int socket = 0; /*Number of the socket */
    int dongle_device_id = 0; /*dongle id */
    int ret, opt, status, len;
    struct hci_filter new_filter; /*Filter for controling the events*/
    evt_le_meta_event *meta;
    le_advertising_info *info;
    char addr[LENGTH_OF_MAC_ADDRESS];
    int retry_time = 0;
    struct hci_request scan_params_rq;
    struct hci_request set_mask_rq;
    struct hci_request enable_adv_rq;
    struct hci_request disable_adv_rq;
    int i=0;
    uint8_t reports_count;
    void * offset = NULL;
    char name[LENGTH_OF_DEVICE_NAME];
    int rssi;
    bool keep_scanning;

#ifdef Debugging
    zlog_debug(category_debug,
        ">> start_ble_scanning... ");
#endif

    while(true == ready_to_work){

        /* Get the dongle id */
        retry_time = DONGLE_GET_RETRY;
        while(retry_time--){
            dongle_device_id = hci_get_route(NULL);

            if(dongle_device_id >= 0){
                break;
	    }
        }
        if (dongle_device_id < 0) {
            zlog_info(category_health_report,
                "Error openning the device");
#ifdef Debugging
            zlog_debug(category_debug,
                "Error openning the device");
#endif
            return;
        }

        /* Open Bluetooth device */
        retry_time = SOCKET_OPEN_RETRY;
        while(retry_time--){
            socket = hci_open_dev(dongle_device_id);

            if(socket >= 0){
                break;
	    }
        }
        if (socket < 0) {
            zlog_info(category_health_report,
                "Error openning socket");
#ifdef Debugging
            zlog_debug(category_debug,
                "Error openning socket");
#endif
             return;
        }

        /* Set BLE scan para,eters */
        if( 0> hci_le_set_scan_parameters(socket, 0x01, htobs(0x0010),
                                      htobs(0x0010), 0x00, 0x00, 
					HCI_SEND_REQUEST_TIMEOUT_IN_MS)){
/*
            zlog_info(category_health_report,
                "Error setting parameters of BLE scanning");
#ifdef Debugging
            zlog_debug(category_debug,
                "Error setting parameters of BLE scanning");
#endif
*/
        }

        if( 0> hci_le_set_scan_enable(socket, 0x01, 1, 
					HCI_SEND_REQUEST_TIMEOUT_IN_MS)){
/*
            zlog_info(category_health_report,
                "Error enabling BLE scanning");
#ifdef Debugging
            zlog_debug(category_debug,
                "Error enabling BLE scanning");
#endif
*/
      }

        le_set_event_mask_cp event_mask_cp;
        memset(&event_mask_cp, 0, sizeof(le_set_event_mask_cp));

        for (i = 0 ; i < 8 ; i++ ) event_mask_cp.mask[i] = 0xFF;

        set_mask_rq =
            ble_hci_request(OCF_LE_SET_EVENT_MASK, 
			LE_SET_EVENT_MASK_CP_SIZE,
                        &status, &event_mask_cp);

        ret = hci_send_req(socket, &set_mask_rq, 
			HCI_SEND_REQUEST_TIMEOUT_IN_MS);

        if ( ret < 0 ) {

            hci_close_dev(socket);
            return;
        }

        le_set_scan_enable_cp scan_cp;
        memset(&scan_cp, 0, sizeof(scan_cp));
        scan_cp.enable      = 0x01; // Enable flag.
        scan_cp.filter_dup  = 0x00; // Filtering disabled.

        enable_adv_rq =
            ble_hci_request(OCF_LE_SET_SCAN_ENABLE, 
			LE_SET_SCAN_ENABLE_CP_SIZE,
                        &status, &scan_cp);

        ret = hci_send_req(socket, &enable_adv_rq, 
			HCI_SEND_REQUEST_TIMEOUT_IN_MS);
        if ( ret < 0 ) {

            hci_close_dev(socket);

            return;
        }

        hci_filter_clear(&new_filter);
        hci_filter_set_ptype(HCI_EVENT_PKT, &new_filter);
        hci_filter_set_event(EVT_LE_META_EVENT, &new_filter);


        if (0 > setsockopt(socket, SOL_HCI, HCI_FILTER, &new_filter,
            sizeof(new_filter)) ) {

          /* Error handling */
            hci_close_dev(socket);
            
	    zlog_info(category_health_report,
                "Error setting HCI filter");
#ifdef Debugging
            zlog_debug(category_debug,
                "Error setting HCI filter");
#endif
        }

        keep_scanning = true;

        while(true == keep_scanning){

            if(read(socket, ble_buffer, sizeof(ble_buffer))
                                                >= HCI_EVENT_HDR_SIZE) {

                meta = (evt_le_meta_event*)
                                      (ble_buffer + HCI_EVENT_HDR_SIZE + 1);

                offset = meta->data + 1;
                info = (le_advertising_info *)offset;

                rssi = (signed char)info->data[info->length];

                /* If the rssi vaule is within the threshold */
                if(rssi > RSSI_RANGE){

		    memset(name, 0, sizeof(name));
                    eir_parse_name(info->data, info->length, name,
                                   sizeof(name) - 1);
                    /* If the name of the BLE device is not unknown */
                    if(strcmp(name, "")!= 0){

			ba2str(&(info->bdaddr), addr);
                        send_to_push_dongle(&info->bdaddr, BLE, name, rssi);

                    }
                }

            }else{
                keep_scanning = false;

            }

        }

        /* Close the process of scanning BLE device, and close the socket. */
        memset(&scan_cp, 0, sizeof(scan_cp));
        scan_cp.enable = 0x00;  // Disable flag.

        disable_adv_rq =
            ble_hci_request(OCF_LE_SET_SCAN_ENABLE, 
			LE_SET_SCAN_ENABLE_CP_SIZE,
            		&status, &scan_cp);

        ret = hci_send_req(socket, &disable_adv_rq,
              HCI_SEND_REQUEST_TIMEOUT_IN_MS);

        if ( ret < 0 ) {
            hci_close_dev(socket);

	    zlog_info(category_health_report,
            	"Error sending HCI request");
#ifdef Debugging
	    zlog_debug(category_debug,
            	"Error sending HCI request");
#endif
            return 0;
        }

        hci_close_dev(socket);

#ifdef Debugging
        zlog_debug(category_debug, 
		"Scanning done of BLE devices");
#endif

    } // end while (ready_to_work)

#ifdef Debugging
    zlog_debug(category_debug,
        "<< start_ble_scanning... ");
#endif

}

void *start_br_scanning(void* param) {

    struct hci_filter filter; /*Filter for controling the events*/
    struct pollfd output; /*A callback event from the socket */
    unsigned char event_buffer[HCI_MAX_EVENT_SIZE]; /*A buffer for the
                                                      callback event */
    unsigned char *event_buffer_pointer; /*A pointer for the event buffer */
    hci_event_hdr *event_handler; /*Record the event type */
    inquiry_cp inquiry_copy; /*Storing the message from the socket */
    inquiry_info_with_rssi *info_rssi; /* Record an
                                          EVT_INQUIRY_RESULT_WITH_RSSI
                                          message */
    inquiry_info *info; /*Record an EVT_INQUIRY_RESULT message */
    int event_buffer_length; /*Length of the event buffer */
    int dongle_device_id = 1; /*dongle id */
    int socket = 0; /*Number of the socket */
    int results; /*Return the result form the socket */
    int results_id; /*ID of the result */
    int retry_time = 0;
    bool keep_scanning;
    char name[LENGTH_OF_DEVICE_NAME];
    int rssi;

#ifdef Debugging
    zlog_debug(category_debug,
        ">> start_br_scanning... ");
#endif

    while(true == ready_to_work){
        /* Open Bluetooth device */
        retry_time = DONGLE_GET_RETRY;
        while(retry_time--){
            dongle_device_id = hci_get_route(NULL);

            if(dongle_device_id >= 0){
                break;
	    }
        }
        if(dongle_device_id < 0){

            zlog_info(category_health_report,
                "Error openning the device");
#ifdef Debugging
            zlog_debug(category_debug,
                "Error openning the device");
#endif
            return;
        }

        retry_time = SOCKET_OPEN_RETRY;
        while(retry_time--){
            socket = hci_open_dev(dongle_device_id);

            if(socket >= 0){
                break;
	    }
        }

        if (socket < 0 ){
            zlog_info(category_health_report,
                "Error openning socket");
#ifdef Debugging
            zlog_debug(category_debug,
                "Error openning socket");
#endif
            return;
        }

        /* Setup filter */
        hci_filter_clear(&filter);
        hci_filter_set_ptype(HCI_EVENT_PKT, &filter);
        hci_filter_set_event(EVT_INQUIRY_RESULT, &filter);
        hci_filter_set_event(EVT_INQUIRY_RESULT_WITH_RSSI, &filter);
        hci_filter_set_event(EVT_INQUIRY_COMPLETE, &filter);


        if (0 > setsockopt(socket, SOL_HCI, HCI_FILTER, &filter,
                        sizeof(filter))) {
            zlog_info(category_health_report,
                "Error setting HCI filter");
#ifdef Debugging
            zlog_debug(category_debug,
                "Error setting HCI filter");
#endif

            hci_close_dev(socket);
            return;

        }

        hci_write_inquiry_mode(socket, 0x01, 10);

        if (0 > hci_send_cmd(socket, OGF_HOST_CTL, OCF_WRITE_INQUIRY_MODE,
            		WRITE_INQUIRY_MODE_RP_SIZE, &inquiry_copy)) {

            /* Error handling */
            zlog_info(category_health_report,
                "Error setting inquiry mode");
#ifdef Debugging
            zlog_debug(category_debug,
                "Error setting inquiry mode");
#endif
            hci_close_dev(socket);
            return;

        }

        memset(&inquiry_copy, 0, sizeof(inquiry_copy));

        /* Use the global inquiry access code (GIAC), which has 
	0x338b9e as its lower address part (LAP) */
        inquiry_copy.lap[2] = 0x9e;
        inquiry_copy.lap[1] = 0x8b;
        inquiry_copy.lap[0] = 0x33;

        /* No limit on number of responses per scan */
        inquiry_copy.num_rsp = 0;
        inquiry_copy.length = 0x30;

#ifdef Debugging
        zlog_debug(category_debug, "Starting inquiry with RSSI...");
#endif


        if (0 > hci_send_cmd(socket, OGF_LINK_CTL, OCF_INQUIRY, 
			INQUIRY_CP_SIZE, &inquiry_copy)) {

            /* Error handling */
            zlog_info(category_health_report,
                "Error starting inquiry");
#ifdef Debugging
            zlog_debug(category_debug,
                "Error starting inquiry");
#endif

            hci_close_dev(socket);
            return;

        }

        output.fd = socket;
        output.events = POLLIN | POLLERR | POLLHUP;


        /* An indicator for continuing to scan the devices. */
        /* After the inquiring events completing, it should jump 
	out of the while loop for getting a new socket */

        keep_scanning = true;

        while (true == keep_scanning) {
            output.revents = 0;
            /* Poll the bluetooth device for an event */
            if (0 < poll(&output, 1, -1)) {

                event_buffer_length =
                    read(socket, event_buffer, sizeof(event_buffer));

                if (0 > event_buffer_length) {
                    continue;
                }else if (0 == event_buffer_length) {

                    break;

                }

                event_handler = (void *)(event_buffer + 1);
                event_buffer_pointer = event_buffer + 
					(1 + HCI_EVENT_HDR_SIZE);
                results = event_buffer_pointer[0];

                switch (event_handler->evt) {

                /* Scanned device with no RSSI value */
                case EVT_INQUIRY_RESULT: {
		    
                    for (results_id = 0; 
			results_id < results; results_id++) {

                        info = (void *)event_buffer_pointer +
                               (sizeof(*info) * results_id) + 1;

                    }
                } break;

                /* Scanned device with RSSI value; when within rangle, 
		send message to bluetooth device. */
                case EVT_INQUIRY_RESULT_WITH_RSSI: {

                    for (results_id = 0; results_id < results; results_id++){

                        info_rssi = (void *)event_buffer_pointer +
                            (sizeof(*info_rssi) * results_id) + 1;

                        if (info_rssi->rssi > RSSI_RANGE) {

			    memset(name, 0, sizeof(name));
                            send_to_push_dongle(&info_rssi->bdaddr, 
						BR_EDR, name, 
						info_rssi->rssi);
                        }

                    }

                } break;

                /* Stop the scanning process */
                case EVT_INQUIRY_COMPLETE: {

                    /* In order to jump out of the while loop. Set 
		    keep_scanning flag to false, new socket will not 
		    been received. 
		    */
                    keep_scanning = false;

                } break;


                default:

                break;

                }

            }

        } //end while

        close(socket);

#ifdef Debugging
        zlog_debug(category_debug, 
		"Scanning done of BR devices");
#endif

    }//end while

#ifdef Debugging
    zlog_debug(category_debug,
    "<< start_br_scanning... ");
#endif

}

void *timeout_cleanup(void* param){

#ifdef Debugging
    zlog_debug(category_debug,
        ">> timeout_cleanup... ");
#endif
    
    while(true == ready_to_work){
	/* Use pthread mutex, cond and signal to control the flow, 
	instead of using busy while loop and sleep mechanism.
	*/
	pthread_mutex_lock(&exec_lock);

	while(true != reach_cln_all_lists)
	{
	    pthread_cond_wait(&cond_cln_all_lists, &exec_lock);
	}
	reach_cln_all_lists = false;

	pthread_mutex_unlock(&exec_lock);

#ifdef Debugging
        zlog_debug(category_debug,
            "cleanup all lists in timeout_cleanup function");
#endif	
	cleanup_list(&scanned_list_head, true);
	cleanup_list(&BR_object_list_head, false);
	cleanup_list(&BLE_object_list_head, false);
    }

#ifdef Debugging
    zlog_debug(category_debug,
        "<< timeout_cleanup... ");
#endif
}


void cleanup_exit(ErrorCode err_code){

    struct List_Entry *list_pointers, *save_list_pointers;
    ScannedDevice *temp;

#ifdef Debugging
    zlog_debug(category_debug,
        ">> cleanup_exit... ");
#endif

    /* Set flag to false */
    ready_to_work = false;

    if(&mempool != NULL){

        /* Go throgth all three lists to release all memory allocated
           to the nodes */

	cleanup_list(&scanned_list_head, true);
	cleanup_list(&BR_object_list_head, false);
	cleanup_list(&BLE_object_list_head, false);
        
        mp_destroy(&mempool);
    }

    pthread_mutex_destroy(&list_lock);
    pthread_mutex_destroy(&exec_lock);
    pthread_cond_destroy(&cond_cln_scanned_list);
    pthread_cond_destroy(&cond_cln_all_lists);

    Wifi_free(&udp_config);

#ifdef Bluetooth_classic
    /* Release the handler for Bluetooth */
    free(g_push_file_path);
#endif

#ifdef Debugging
    zlog_debug(category_debug,
        "<< cleanup_exit... ");
#endif

    exit(err_code);
}


int main(int argc, char **argv) {

    ErrorCode return_value = WORK_SUCCESSFULLY;
    struct sigaction sigint_handler;

    /* Initialize the application log */
    if (zlog_init("../config/zlog.conf") == 0) {

        category_health_report = 
		zlog_get_category(LOG_CATEGORY_HEALTH_REPORT);

        if (!category_health_report) {

            zlog_fini();
        }

#ifdef Debugging
    	category_debug = 
		zlog_get_category(LOG_CATEGORY_DEBUG);

        if (!category_debug) {

            zlog_fini();
        }
#endif
    }

    /* Load config struct */
    return_value = get_config(&g_config, CONFIG_FILE_NAME);
    if(WORK_SUCCESSFULLY != return_value){
        zlog_info(category_health_report,
            "Error openning file");
#ifdef Debugging
        zlog_debug(category_debug,
            "Error openning file");
#endif
        return E_OPEN_FILE;
    }


    /*Initialize the global flag */
    ready_to_work = true;

    /* Initialize the lock for accessing the lists */
    pthread_mutex_init(&list_lock,NULL);

    /* Initialize the lock for execution flows between threads */
    reach_cln_scanned_list = false;
    reach_cln_all_lists = false;
    pthread_mutex_init(&exec_lock, NULL);
    pthread_cond_init(&cond_cln_scanned_list, NULL);
    pthread_cond_init(&cond_cln_all_lists, NULL);


    /* Initialize the memory pool */
    if(mp_init(&mempool, sizeof(struct ScannedDevice), SLOTS_IN_MEM_POOL)
            != MEMORY_POOL_SUCCESS){
        
        zlog_info(category_health_report,
            "Error allocating memory");
#ifdef Debugging
        zlog_debug(category_debug,
            "Error allocating memory");
#endif
    }

    /* Initialize the wifi connection to gateway */
    strcpy(udp_config.send_ipv4_addr, g_config.gateway_addr);
    udp_config.send_portno = g_config.gateway_port;
    udp_config.recv_portno = g_config.local_client_port;

    return_value = Wifi_init(&udp_config);
    if(WORK_SUCCESSFULLY != return_value){

        zlog_info(category_health_report,
            "Error initializing network connection to gateway");
#ifdef Debugging
        zlog_debug(category_debug,
            "Error initializing network connection to gateway");
#endif
    }

    /*Initialize the global lists */
    init_entry(&scanned_list_head.list_entry);
    scanned_list_head.device_type = BR_EDR;
    init_entry(&BR_object_list_head.list_entry);
    BR_object_list_head.device_type = BR_EDR;
    init_entry(&BLE_object_list_head.list_entry);
    BLE_object_list_head.device_type = BLE;


    /* Register handler function for SIGINT signal */
    sigint_handler.sa_handler = ctrlc_handler;
    sigemptyset(&sigint_handler.sa_mask);
    sigint_handler.sa_flags = 0;

    if (-1 == sigaction(SIGINT, &sigint_handler, NULL)) {
        zlog_info(category_health_report,
            "Error registering signal handler for SIGINT");
#ifdef Debugging
        zlog_debug(category_debug,
            "Error registering signal handler for SIGINT");
#endif
    }


    /* Create the thread for track BR_EDR device */
    pthread_t br_scanning_thread;

    return_value = startThread(&br_scanning_thread,
                               start_br_scanning, NULL);

    if(return_value != WORK_SUCCESSFULLY){
        zlog_info(category_health_report,
            "Error creating thread");
#ifdef Debugging
        zlog_debug(category_debug,
            "Error creating thread");
#endif
    }


    /* Create the thread for track BLE device */
    pthread_t ble_scanning_thread;

    return_value = startThread(&ble_scanning_thread,
                               start_ble_scanning, NULL);

    if(return_value != WORK_SUCCESSFULLY){
        zlog_info(category_health_report,
            "Error creating thread");
#ifdef Debugging
        zlog_debug(category_debug,
            "Error creating thread");
#endif
    }


    /* Create the the cleanup_scanned_list thread */
    pthread_t cleanup_scanned_list_thread;

    return_value = startThread(&cleanup_scanned_list_thread,
                               cleanup_scanned_list, &scanned_list_head);

    if(return_value != WORK_SUCCESSFULLY){
        zlog_info(category_health_report,
            "Error creating thread");
#ifdef Debugging
        zlog_debug(category_debug,
            "Error creating thread");
#endif
    }


    /* Create the thread for track device */
    pthread_t manage_communication_thread;

    return_value = startThread(&manage_communication_thread,
                               manage_communication, NULL);

    if(return_value != WORK_SUCCESSFULLY){
        zlog_info(category_health_report,
            "Error creating thread");
#ifdef Debugging
        zlog_debug(category_debug,
            "Error creating thread");
#endif
    }


    /* Create the thread for track device */
    pthread_t timer_thread;

    return_value = startThread(&timer_thread,
                               timeout_cleanup, NULL);

    if(return_value != WORK_SUCCESSFULLY){
        zlog_info(category_health_report,
            "Error creating thread");
#ifdef Debugging
        zlog_debug(category_debug,
            "Error creating thread");
#endif
    }


    zlog_info(category_health_report,
                  "All the threads are created.");
#ifdef Debugging
    zlog_debug(category_debug, "All the threads are created.");
#endif


    /* Start bluetooth advertising and wait while all threads are
       executing
    */
     return_value =
        enable_advertising(INTERVAL_ADVERTISING_IN_MS,
               g_config.uuid,
           LBEACON_MAJOR_VER,
               LBEACON_MINOR_VER,
               RSSI_VALUE);

    if (WORK_SUCCESSFULLY != return_value){

        zlog_info(category_health_report,
            "Unable to enable advertising. Please make sure "
            "all the hardware devices are ready and try again.");

#ifdef Debugging
        zlog_debug(category_debug,
            "Unable to enable advertising. Please make sure "
            "all the hardware devices are ready and try again.");
#endif

        cleanup_exit(return_value);
    }

    cln_scanned_list_last_time = get_system_time();
    while (true == ready_to_work) {
        sleep(INTERVAL_FOR_BUSY_WAITING_CHECK_IN_SEC);
	
	/* If it reaches the time interval of cleaning up scanned list, 
           we should notify cleanup_scanned_list thread to remove old 
           nodes from scanned_list_head.

           In this way, the appearance of a single node (used by both
	   scanned device and BR_EDR device at the same time) in 
	   scanned_list_head list will not be longer than the time
           (2 * INTERVAL_FOR_CLEANUP_SCANNED_LIST_IN_SEC) and this is 
	   also the worse case in which the existing node in 
	   scanned_list_head blocks the BR_EDR devices from being 
	   inserted into BR_object_list_head.
	*/
	if(get_system_time() - cln_scanned_list_last_time > 
		INTERVAL_FOR_CLEANUP_SCANNED_LIST_IN_SEC){
		
	    pthread_mutex_lock(&exec_lock);

	    reach_cln_scanned_list = true;
	    pthread_cond_signal(&cond_cln_scanned_list);

	    pthread_mutex_unlock(&exec_lock);
	}
    }

    /* When signal is received, disable message advertising */
    disable_advertising();

    cleanup_exit(WORK_SUCCESSFULLY);

    return WORK_SUCCESSFULLY;

}


/* Follow are functions for communication via BR/EDR path to Bluetooth
   classic devices */
#ifdef Bluetooth_classic


void start_classic_pushing(void){

    /* An iterator through the list of ScannedDevice structs */
    int device_id;

    int number_of_push_dongles = atoi(g_config.number_of_push_dongles);
    int maximum_number_of_devices_per_dongle =
        maximum_number_of_devices / number_of_push_dongles;

    /* An iterator through each push dongle */
    int push_dongle_id;

    /* An iterator through a block of devices per dongle */
    int block_id;

    int dongle_device_id = 0; /*Device ID of dongle */
    int maximum_number_of_devices;;



    g_push_file_path =
        malloc(g_config.file_path_length + g_config.file_name_length);


    if (g_push_file_path == NULL) {

         /* Error handling */
    //    zlog_info(category_health_report, errordesc[E_MALLOC].message);
        cleanup_exit();
        return E_MALLOC;

    }

    memcpy(g_push_file_path, g_config.file_path,
           g_config.file_path_length - 1);
    memcpy(g_push_file_path + g_config.file_path_length - 1,
           g_config.file_name, g_config.file_name_length - 1);

    /* the  maximum number of devices of an array */
    maximum_number_of_devices = atoi(g_config.maximum_number_of_devices);


    /* Initialize each ThreadStatus struct in the g_idle_handler array */
    for (device_id = 0; device_id < maximum_number_of_devices; device_id++) {

         strncpy(g_idle_handler[device_id].scanned_mac_address, "0",
         LENGTH_OF_MAC_ADDRESS);
        g_idle_handler[device_id].idle = true;
        g_idle_handler[device_id].is_waiting_to_send = false;

    }



    /* Create an arrayof threads for sending message to the scanned MAC
     * address */
    pthread_t send_file_thread[maximum_number_of_devices];

    /* After all the other threads are ready, set this flag to false. */
    send_message_cancelled = false;


   for (device_id = 0; device_id < maximum_number_of_devices; device_id++) {

         if (g_idle_handler[device_id].is_waiting_to_send == true) {

            /* Depending on the number of push dongles, split the threads
             * evenly and assign each thread to a push dongle device ID */
            for (push_dongle_id = 0;
                push_dongle_id < number_of_push_dongles;
                push_dongle_id++) {

                for (block_id = 0;
                     block_id < maximum_number_of_devices_per_dongle;
                     block_id++) {

                    if (device_id ==
                        push_dongle_id *
                        maximum_number_of_devices_per_dongle +
                        block_id) {

                            dongle_device_id = push_dongle_id + 1;

                        }

                    }

                }

            }

        return_value =  startThread(&send_file_thread[device_id], send_file,
                    (void *)dongle_device_id);

        if(return_value != WORK_SUCCESSFULLY){

         //   zlog_info(category_health_report,
         //             errordesc[E_START_THREAD].message);
            cleanup_exit();
            return 1;
        }


    }

    /*Set send_message_cancelled flag to false now. All the thread are
      ready.*/
    send_message_cancelled = false;


     /* ready_to_work = false , shut down.
        wait for send_file_thread to exit. */

    for (device_id = 0; device_id < maximum_number_of_devices; device_id++) {

        return_value = pthread_join(send_file_thread[device_id], NULL);

        if (return_value != 0) {

            zlog_info(category_health_report, strerror(errno));
            cleanup_exit();
            return;

        }
    }


}


char *choose_file(char *message_to_send) {

    DIR *groupdir;           /* A dirent that stores list of directories */
    struct dirent *groupent; /* A dirent struct that stores directory info */
    int message_id = 0;      /* A iterator for number of messages and
                                groups */
    int group_id = 0;        /* A iterator for number of groups */
    char *return_value;      /* Return value which turns file path to a
                                string */

    /* Convert number of groups and messages from a string to an integer */
    int number_of_groups = atoi(g_config.number_of_groups);
    int number_of_messages = atoi(g_config.number_of_messages);

    /* An array of buffer for group file names */
    char groups[number_of_groups][FILE_NAME_BUFFER];

    /* An array of buffer for message file names */
    char messages[number_of_messages][FILE_NAME_BUFFER];

    char file_path[FILE_NAME_BUFFER];

    DIR *messagedir = NULL;
    struct dirent *messageent = NULL;

    /* Stores all the name of files and directories in groups */
    groupdir = opendir("/home/pi/LBeacon/messages/");
    if (groupdir) {
        while ((groupent = readdir(groupdir)) != NULL) {
            if (strcmp(groupent->d_name, ".") != 0 &&
                strcmp(groupent->d_name, "..") != 0) {
                strcpy(groups[message_id], groupent->d_name);
                message_id++;
            }
        }
        closedir(groupdir);
    }
    else {
        /* Error handling */
        return NULL;
    }

    /* Stores file path of message_to_send */
    memset(file_path, 0, FILE_NAME_BUFFER);
    message_id = 0;

    /* Go through each message in directory and store each file name */
    for (group_id = 0; group_id < number_of_groups; group_id++) {
        /* Concatenate strings to make file path */
        sprintf(file_path, "/home/pi/LBeacon/messages/");
        strcat(file_path, groups[group_id]);

        messagedir = opendir(file_path);
        if (messagedir) {
            while ((messageent = readdir(messagedir)) != NULL) {
                if (strcmp(messageent->d_name, ".") != 0 &&
                    strcmp(messageent->d_name, "..") != 0) {
                    strcpy(messages[message_id], messageent->d_name);
                    /* If message name found, return file path */
                    if (0 == strcmp(messages[message_id], message_to_send)) {
                        strcat(file_path, "/");
                        strcat(file_path, messages[message_id]);
                        return_value = &file_path[0];
                        return return_value;
                    }
                    message_id++;
                }
            }
            closedir(messagedir);
        }
        else {
            /* Error handling */
            return NULL;
        }

        return;

    }

    /* Error handling */
    return NULL;
}



void *send_file(void *id) {

    obexftp_client_t *client = NULL; /* ObexFTP client */
    int dongle_device_id = 0;        /* Device ID of each dongle */
    int socket;                      /* ObexFTP client's socket */
    int channel = -1;                /* ObexFTP channel */
    int thread_id = (int)id;         /* Thread ID */
    char *address = NULL;            /* Scanned MAC address */
    char *file_name;                  /* File name of message to be sent */
    int return_value;                /* Return value for error handling */

    /* Get the maximum number of devices from config file. */
    int maximum_number_of_devices = atoi(g_config.maximum_number_of_devices);

    /* An iterator through the array of ScannedDevice struct */
    int device_id;

    long long start;
    long long end;


    while (true == ready_to_work &&  false == send_message_cancelled) {

        for (device_id = 0; device_id < maximum_number_of_devices;
            device_id++) {

            if (device_id == thread_id &&
                g_idle_handler[device_id].is_waiting_to_send == true) {


                /* Open socket and use current time as start time to keep
                 * of how long has taken to send the message to the device */
                socket = hci_open_dev(dongle_device_id);


                if (dongle_device_id < 0 || socket < 0) {

                    /* Error handling */
                    strncpy(
                            g_idle_handler[device_id].scanned_mac_address,
                            "0",
                            LENGTH_OF_MAC_ADDRESS);

                    g_idle_handler[device_id].idle = true;
                    g_idle_handler[device_id].is_waiting_to_send = false;
                    break;

                }

                start = get_system_time();
                address =
                    (char *)g_idle_handler[device_id].scanned_mac_address;
                channel = obexftp_browse_bt_push(address);

                /* Extract basename from file path */
                file_name = strrchr(g_push_file_path, '/');
                file_name[g_config.file_name_length] = '\0';

                if (!file_name) {

                    file_name = g_push_file_path;

                }
                else {

                    file_name++;

                }
                printf("Sending file %s to %s\n", file_name, address);

                /* Open connection */
                client = obexftp_open(OBEX_TRANS_BLUETOOTH, NULL, NULL,
                                      NULL);
                end = get_system_time();
                printf("Time to open connection: %lld ms\n", end - start);

                if (client == NULL) {

                    /* Error handling */
                    strncpy(
                            g_idle_handler[device_id].scanned_mac_address,
                            "0",
                            LENGTH_OF_MAC_ADDRESS);

                    g_idle_handler[device_id].idle = true;
                    g_idle_handler[device_id].is_waiting_to_send = false;
                    close(socket);
                    break;

                }

                /* Connect to the scanned device */
                return_value = obexftp_connect_push(client, address,
                                                    channel);

                /* If obexftp_connect_push returns a negative integer, then
                 * it goes into error handling */
                if (0 > return_value) {

                    /* Error handling */
                    obexftp_close(client);
                    client = NULL;
                    strncpy(
                            g_idle_handler[device_id].scanned_mac_address,
                            "0",
                            LENGTH_OF_MAC_ADDRESS);

                    g_idle_handler[device_id].idle = true;
                    g_idle_handler[device_id].is_waiting_to_send = false;
                    close(socket);
                    break;

                }

                /* Push file to the scanned device */
                return_value = obexftp_put_file(client, g_push_file_path,
                                                file_name);
                if (0 > return_value) {

                    /* TODO: Error handling */
                }

                /* Disconnect connection */
                return_value = obexftp_disconnect(client);
                if (0 > return_value) {

                    /* TODO: Error handling  */
                    pthread_exit(NULL);
                    return;

                }

               /* Leave the socket open */
                obexftp_close(client);
                client = NULL;
                strncpy(g_idle_handler[device_id].scanned_mac_address,
                        "0",
                        LENGTH_OF_MAC_ADDRESS);

                g_idle_handler[device_id].idle = true;
                g_idle_handler[device_id].is_waiting_to_send = false;
                close(socket);

            }//end if

        }//end for loop

    } //end while loop

    /* Exit forcibly by main thread */
    if(ready_to_work == false){
        return;
    }

}

#endif //Bluetooth_classic
