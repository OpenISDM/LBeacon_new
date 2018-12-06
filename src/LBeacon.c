/*
 Copyright (c) 2016 Academia Sinica, Institute of Information Science

 License:

      GPL 3.0 : The content of this file is subject to the terms and
      cnditions defined in file 'COPYING.txt', which is part of this source
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

      Han Wang, hollywang@iis.sinica.edu.tw
      Jake Lee, jakelee@iis.sinica.edu.tw
      Joey Zhou, joeyzhou@iis.sinica.edu.tw
      Kenneth Tang, kennethtang@iis.sinica.edu.tw
      James Huamg, jameshuang@iis.sinica.edu.tw
      Shirley Huang, shirley.huang.93@gmail.com



*/

#include "LBeacon.h"


#define Debugging


Config get_config(char *file_name) {

    /* Return value is a struct containing all config information */
    Config config;

    FILE *file = fopen(file_name, "r");
    if (file == NULL) {

        /* Error handling */
        perror(errordesc[E_OPEN_FILE].message);
        zlog_info(category_health_report,
                  errordesc[E_OPEN_FILE].message);
        cleanup_exit();
        return;

    }
    else {
    /* Create spaces for storing the string of the current line being read */
    char config_setting[CONFIG_BUFFER_SIZE];
    char *config_message[CONFIG_FILE_LENGTH];

     /* Keep reading each line and store into the config struct */
    fgets(config_setting, sizeof(config_setting), file);
    config_message[0] = strstr((char *)config_setting, DELIMITER);
    config_message[0] = config_message[0] + strlen(DELIMITER);
    memcpy(config.coordinate_X, config_message[0],
           strlen(config_message[0]));
    config.coordinate_X_length = strlen(config_message[0]);

    fgets(config_setting, sizeof(config_setting), file);
    config_message[1] = strstr((char *)config_setting, DELIMITER);
    config_message[1] = config_message[1] + strlen(DELIMITER);
    memcpy(config.coordinate_Y, config_message[1],
           strlen(config_message[1]));
    config.coordinate_Y_length = strlen(config_message[1]);

    fgets(config_setting, sizeof(config_setting), file);
    config_message[2] = strstr((char *)config_setting, DELIMITER);
    config_message[2] = config_message[2] + strlen(DELIMITER);
    memcpy(config.coordinate_Z, config_message[2],
           strlen(config_message[2]));
    config.coordinate_Z_length = strlen(config_message[2]);

    fgets(config_setting, sizeof(config_setting), file);
    config_message[3] = strstr((char *)config_setting, DELIMITER);
    config_message[3] = config_message[3] + strlen(DELIMITER);
    memcpy(config.file_name, config_message[3], strlen(config_message[3]));
    config.file_name_length = strlen(config_message[3]);

    fgets(config_setting, sizeof(config_setting), file);
    config_message[4] = strstr((char *)config_setting, DELIMITER);
    config_message[4] = config_message[4] + strlen(DELIMITER);
    memcpy(config.file_path, config_message[4], strlen(config_message[4]));
    config.file_path_length = strlen(config_message[4]);

    fgets(config_setting, sizeof(config_setting), file);
    config_message[5] = strstr((char *)config_setting, DELIMITER);
    config_message[5] = config_message[5] + strlen(DELIMITER);
    memcpy(config.maximum_number_of_devices, config_message[5],
           strlen(config_message[5]));
    config.maximum_number_of_devices_length = strlen(config_message[5]);

    fgets(config_setting, sizeof(config_setting), file);
    config_message[6] = strstr((char *)config_setting, DELIMITER);
    config_message[6] = config_message[6] + strlen(DELIMITER);
    memcpy(config.number_of_groups, config_message[6],
           strlen(config_message[6]));
    config.number_of_groups_length = strlen(config_message[6]);

    fgets(config_setting, sizeof(config_setting), file);
    config_message[7] = strstr((char *)config_setting, DELIMITER);
    config_message[7] = config_message[7] + strlen(DELIMITER);
    memcpy(config.number_of_messages, config_message[7],
           strlen(config_message[7]));
    config.number_of_messages_length = strlen(config_message[7]);

    fgets(config_setting, sizeof(config_setting), file);
    config_message[8] = strstr((char *)config_setting, DELIMITER);
    config_message[8] = config_message[8] + strlen(DELIMITER);
    memcpy(config.number_of_push_dongles, config_message[8],
           strlen(config_message[8]));
    config.number_of_push_dongles_length = strlen(config_message[8]);

    fgets(config_setting, sizeof(config_setting), file);
    config_message[9] = strstr((char *)config_setting, DELIMITER);
    config_message[9] = config_message[9] + strlen(DELIMITER);
    memcpy(config.rssi_coverage, config_message[9],
           strlen(config_message[9]));
    config.rssi_coverage_length = strlen(config_message[9]);

    fgets(config_setting, sizeof(config_setting), file);
    config_message[10] = strstr((char *)config_setting, DELIMITER);
    config_message[10] = config_message[10] + strlen(DELIMITER);
    memcpy(config.uuid, config_message[10], strlen(config_message[10]));
    config.uuid_length = strlen(config_message[10]);


    fclose(file);
    }

    return config;
}




long long get_system_time() {
    /* A struct that stores the time */
    struct timeb t;

    /* Return value as a long long type */
    long long system_time;

    /* Convert time from Epoch to time in milliseconds of a long long type */
    ftime(&t);
    system_time = 1000 * t.time + t.millitm;

    return system_time;
}




void print_RSSI_value(bdaddr_t *bluetooth_device_address, bool has_rssi,
    int rssi) {

    /* Scanned MAC address */
    char address[LENGTH_OF_MAC_ADDRESS];

    /* Converts the bluetooth device address to string */
    ba2str(bluetooth_device_address, address);
    strcat(address, "\0");

    /* Print bluetooth device's RSSI value */
    printf("%17s", address);
    if (has_rssi) {
        printf(" RSSI:%d", rssi);
    }
    else {
        printf(" RSSI:n/a");
    }
    printf("\n");
    fflush(NULL);

    return;

}





void send_to_push_dongle(bdaddr_t *bluetooth_device_address, bool is_ble) {

    /* Stores the MAC address as a string */
    char address[LENGTH_OF_MAC_ADDRESS];

    /* Converts the bluetooth device address to a string */
    ba2str(bluetooth_device_address, address);
    strcat(address, "\0");

    struct ScannedDevice *new_node;

    /* Check whether the MAC address has been seen recently by the LBeacon.*/

    pthread_mutex_lock(&list_lock);
    if(is_ble == true){


        new_node = check_is_in_list(address,
                                    &BLE_object_list_head);


    }else{

        new_node = check_is_in_list(address,
                                    &scanned_list_head);

    }

    if (new_node != NULL) {

        /* Update the final scan time, release lock and return */
        new_node->final_scanned_time = get_system_time();

        pthread_mutex_unlock(&list_lock);

    }else{

        /* The address is new. */

        /* Allocate memory from memory pool for a new node, initialize the
        node, and insert the new node to the scanned list and
        tracked_BR_object_list if the address is that of a BR/EDR device,
        else if it is a BLE device, insert the new node into the
        tracked_BLE_object_list. */

        pthread_mutex_unlock(&list_lock);

#ifdef Debugging

        zlog_debug(category_debug,
                    "******Get the memory from the pool. ****** ");

#endif

        new_node = (struct ScannedDevice*) mp_alloc(&mempool);


        /* Initialize the list entries */
        init_entry(&new_node->sc_list_entry);
        init_entry(&new_node->tr_list_entry);

        /* Get the initial scan time for the new node. */
        new_node->initial_scanned_time = get_system_time();
        new_node->final_scanned_time = new_node->initial_scanned_time;

        /* Copy the MAC address to the node */
        strncpy(new_node->scanned_mac_address,
                address,
                LENGTH_OF_MAC_ADDRESS);

        /* Insert the new node into the right lists. */
        pthread_mutex_lock(&list_lock);

        if(is_ble == true){

            /* Insert the new node to the tracked_BLE_object_list */
            insert_list_first(&new_node->tr_list_entry,
                            &BLE_object_list_head.list_entry);


        }else{

            /* Insert the new node to the scanned list */
            insert_list_first(&new_node->sc_list_entry,
                            &scanned_list_head.list_entry);

            /* Insert the new node to the track_BR_object_list  */
            insert_list_first(&new_node->tr_list_entry,
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
    ScannedDevice *temp;


    /* If there is no node in the list, reutrn NULL directly. */
    if(list->list_entry.next == list->list_entry.prev){

        return NULL;

    }

    /* Go through list */
    list_for_each(list_pointers, &list->list_entry) {

        /* According to the device type stored in the list, get the specific
        data */
        switch(list->device_type){

            case BR_EDR:

                temp = ListEntry(list_pointers, ScannedDevice,
                                 sc_list_entry);
                break;

            case BLE:

                temp = ListEntry(list_pointers, ScannedDevice,
                                 tr_list_entry);
                break;

        }

        int len = strlen(address);

        char *addr_last_digits = &address[len - NUM_DIGITS_TO_COMPARE];
        char *temp_last_digits =
                     &temp->scanned_mac_address[len - NUM_DIGITS_TO_COMPARE];

        /* Compare the first and the last digits of the MAC address */
        if ((strncmp(address, temp->scanned_mac_address,
                      NUM_DIGITS_TO_COMPARE)== false)&&
            (strncmp(addr_last_digits, temp_last_digits,
                      NUM_DIGITS_TO_COMPARE)== false)) {

            return temp;

        }

    }

    /* Input MAC address is new */
    return NULL;
}



ErrorCode enable_advertising(int advertising_interval,
                             char *advertising_uuid,
                             int rssi_value) {

    int dongle_device_id = hci_get_route(NULL);
    int device_handle = 0;

    if ((device_handle = hci_open_dev(dongle_device_id)) < 0) {

        /* Error handling */
        perror(errordesc[E_OPEN_DEVICE].message);
        zlog_info(category_health_report,
                  errordesc[E_OPEN_DEVICE].message);

        return E_OPEN_DEVICE;
    }

    le_set_advertising_parameters_cp advertising_parameters_copy;
    memset(&advertising_parameters_copy, 0,
        sizeof(advertising_parameters_copy));
    advertising_parameters_copy.min_interval = htobs(advertising_interval);
    advertising_parameters_copy.max_interval = htobs(advertising_interval);
    advertising_parameters_copy.chan_map = 7;

    uint8_t status;
    struct hci_request request;
    memset(&request, 0, sizeof(request));
    request.ogf = OGF_LE_CTL;
    request.ocf = OCF_LE_SET_ADVERTISING_PARAMETERS;
    request.cparam = &advertising_parameters_copy;
    request.clen = LE_SET_ADVERTISING_PARAMETERS_CP_SIZE;
    request.rparam = &status;
    request.rlen = 1;

     int return_value = hci_send_req(device_handle, &request,
                                    HCI_SEND_REQUEST_TIMEOUT);

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
                                HCI_SEND_REQUEST_TIMEOUT);

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

    uint8_t segment_length = 1;
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

    unsigned int *uuid = uuid_str_to_data(advertising_uuid);
    int uuid_iterator;

    for (uuid_iterator = 0; uuid_iterator < strlen(advertising_uuid) / 2;
        uuid_iterator++) {
        advertisement_data_copy
            .data[advertisement_data_copy.length + segment_length] =
            htobs(uuid[uuid_iterator]);
        segment_length++;

    }

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
                                HCI_SEND_REQUEST_TIMEOUT);

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

    int dongle_device_id = hci_get_route(NULL);
    int device_handle = 0;
    if ((device_handle = hci_open_dev(dongle_device_id)) < 0) {
        /* Error handling */
        perror(errordesc[E_OPEN_FILE].message);
        zlog_info(category_health_report,
                  errordesc[E_OPEN_FILE].message);

        return E_OPEN_FILE;
    }

    le_set_advertise_enable_cp advertisement_copy;
    uint8_t status;

    memset(&advertisement_copy, 0, sizeof(advertisement_copy));

    struct hci_request request;
    memset(&request, 0, sizeof(request));
    request.ogf = OGF_LE_CTL;
    request.ocf = OCF_LE_SET_ADVERTISE_ENABLE;
    request.cparam = &advertisement_copy;
    request.clen = LE_SET_ADVERTISE_ENABLE_CP_SIZE;
    request.rparam = &status;
    request.rlen = 1;

    int return_value = hci_send_req(device_handle, &request,
                                    HCI_SEND_REQUEST_TIMEOUT);

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




void *stop_broadcast(void *beacon_location) {
    #ifdef Debugging
        zlog_debug(category_debug,
                    ">> stop_broadcast ");
    #endif

    int enable_advertising_success =
        enable_advertising(ADVERTISING_INTERVAL, beacon_location,
                           RSSI_VALUE);

    if (enable_advertising_success == 0) {

        struct sigaction sigint_handler;
        sigint_handler.sa_handler = ctrlc_handler;
        sigemptyset(&sigint_handler.sa_mask);
        sigint_handler.sa_flags = 0;

        if (sigaction(SIGINT, &sigint_handler, NULL) == -1) {

            /* Error handling */
            perror("sigaction error");
            zlog_info(category_health_report, "sigaction error");
            return;
        }

        perror("Hit ctrl-c to stop advertising");

        while (false == g_done) {
            sleep(1);
        }

        /* When signal is received, disable message advertising */
        perror("Shutting down");
        disable_advertising();

    }

    if(ready_to_work == false){

        return;
    }
    #ifdef Debugging
        zlog_debug(category_debug,
                    "<< stop_broadcast ");
    #endif
}





void *cleanup_scanned_list(void) {
    #ifdef Debugging
        zlog_debug(category_debug,
                    ">> cleanup_scanned_list ");
    #endif

    struct List_Entry *list_pointers, *save_list_pointers;
    ScannedDevice *temp;

    while (false == g_done && true == ready_to_work) {


        /*Check whether the list is empty */
        while(false == g_done && 
		scanned_list_head.list_entry.next == 
			scanned_list_head.list_entry.prev){

            sleep(TIMEOUT_WAITING);
        }


        /* Go through list */
        list_for_each_safe(list_pointers,
                           save_list_pointers,
                           &scanned_list_head.list_entry){


            temp = ListEntry(list_pointers, ScannedDevice, sc_list_entry);

            /* If the device has been in the scanned list for at least 30
            seconds, remove its struct node from the scanned list */
            if (get_system_time() - temp->initial_scanned_time > TIMEOUT) {

       	        pthread_mutex_lock(&list_lock);
                
		remove_list_node(&temp->sc_list_entry);
        
		pthread_mutex_unlock(&list_lock);

                /* If the node no longer is in the tracked_BR_object_lists,
                free the space back to the memory pool. */
              
                if(temp->tr_list_entry.next
                                        == temp->tr_list_entry.prev){
                  mp_free(&mempool, temp);

                }

            }
            else {
                continue;
            }
        }
    }

    #ifdef Debugging
        zlog_debug(category_debug,
                    "<< cleanup_scanned_list ");
    #endif
    return;

}




void *manage_communication(void) {
    #ifdef Debugging
        zlog_debug(category_debug,
                    ">> manage_communication ");
    #endif

    Threadpool thpool;
    FILE *br_object_file, *ble_object_file;
    char *zig_message[ZIG_MESSAGE_LENGTH];
    int polled_type, copy_progress;

    /* Initialize the thread pool and work threads waiting for the
    new work/job to be assigned. */
    thpool = thpool_init(NUM_WORK_THREADS);
    if(thpool == NULL){

        /* Could not create thread pool, handle error */
        perror(errordesc[E_INIT_THREAD_POOL].message);
        zlog_info(category_health_report,
                  errordesc[E_INIT_THREAD_POOL].message);
        cleanup_exit();

        return;

    }

    while(false == g_done && true == ready_to_work){


        /* Check call back from the gateway. If not polled by gateway, sleep
        for a short time. If polled, take the action according to the
        poll type. */

        polled_type = receive_call_back();


        while(false == g_done && polled_type == NOT_YET_POLLED){

#ifdef Debugging

            zlog_debug(category_debug, "Not yet Polled, go to sleep");
#endif

            sleep(TIMEOUT_WAITING);

            polled_type = receive_call_back();

        }

        /* According to the polled data type, prepare a work item */
        switch(polled_type){

            case TRACK_OBJECT_DATA:

                /* Copy track_object data to a file to be transmited */
                copy_progress =
                (int)copy_object_data_to_file("tracked_br_txt.txt",
                                                BR_object_list_head);

                if(copy_progress == WORK_SUCCESSFULLY){

                    /* Open the file that is going to be sent to the gateway */
                    br_object_file = fopen("tracked_br_list.txt", "r");

                    if (br_object_file == NULL) {

                        /* Error handling */
                        perror(errordesc[E_OPEN_FILE].message);
                        zlog_info(category_health_report,
                                  errordesc[E_OPEN_FILE].message);
                        cleanup_exit();
                        return;
                        }

                    /* Read the file to get the content for the message to
                       send */
                    fgets(zig_message, sizeof(zig_message),
                          br_object_file);

                    fclose(br_object_file);

                }

                /* Copy BLE_tracked data to the a file to be transmited */
                copy_progress =
                (int)copy_object_data_to_file("tracked_ble_txt.txt",
                                                BLE_object_list_head);

                if(copy_progress == WORK_SUCCESSFULLY){

                    /* Open the file that is going to be sent to the gateway */
                    ble_object_file = fopen("tracked_ble_txt.txt", "r");

                    if (ble_object_file == NULL) {

                        /* Error handling */
                        perror(errordesc[E_OPEN_FILE].message);
                        zlog_info(category_health_report,
                                  errordesc[E_OPEN_FILE].message);
                        cleanup_exit();
                        return;
                    }

                    /* Read the file to get the content for the message to
                    send */
                    char temp_char[ZIG_MESSAGE_LENGTH];
                    fgets(temp_char, sizeof(zig_message),
                          ble_object_file);

                    /* Add the result of BLE scanning to the message that is
                    going to be sent to the gateway */
                    strcat(zig_message, temp_char);

                    fclose(ble_object_file);

                }


#ifdef Debugging

                zlog_debug(category_debug,
                           "Message: %s", zig_message);

#endif

                zlog_info(category_health_report,
                          "Sent Message: %s", zig_message);



                /* Add a work item to be executed by a work thread */
                if(thpool_add_work(thpool,
                                   (void*)zigbee_send_file,
                                   zig_message, 2) != 0){

                    /* Error handling */
                    /* Set ready_to_work to false to let other theeads know
                       of the error */
                    ready_to_work = false;
                    perror(errordesc[E_ADD_WORK_THREAD].message);
                    zlog_info(category_health_report,
                              errordesc[E_ADD_WORK_THREAD].message);
                    cleanup_exit();
                    return;

                }

                break;

            case HEALTH_REPORT:

            /* TODO:
               Create the file for the health report. The files contains the
               error log. will be done as soon as possible */

                break;

            case E_CALL_BACK:

                  /* Error of call back function, set network_is_down to
                  true */
                  network_is_down = true;

                break;

            default:

                /* Error happened. set network_is_down to true */
                network_is_down = true;

                break;

        }



    } // end of the while


    /* After the ready_to_work is set to false, clean up the zigbee and the
       thread pool */
    zigbee_free();

    /* Free the thread pool */
    thpool_destroy(thpool);
    #ifdef Debugging
        zlog_debug(category_debug,
                    "<< manage_communication ");
    #endif
    return;

}

ErrorCode copy_object_data_to_file(char *file_name, ObjectListHead list) {

    /* Check the input parameter if is valid */
    if(&list != &BR_object_list_head ||
       &list != &BLE_object_list_head){

        perror(errordesc[E_INPUT_PARAMETER].message);
        zlog_info(category_health_report,
                  errordesc[E_INPUT_PARAMETER].message);

        return E_INPUT_PARAMETER;

    }

    FILE *track_file;

    /* Head of a local list for tracked object */
    List_Entry local_object_list_head;
    /* Initilize the local list */
    init_entry(&local_object_list_head);

    /* Two pointers to be used locally */
    struct List_Entry *list_pointers, *tail_pointers;

    ScannedDevice *temp;
    int number_in_list;
    int number_to_send;
    char timestamp_initial_str[LENGTH_OF_TIME];
    char timestamp_final_str[LENGTH_OF_TIME];
    char basic_info[LENGTH_OF_INFO];

    DeviceType device_type = list.device_type;


    /* Create a new file to store data in the tracked_BLE_object_list */
    track_file = fopen(file_name, "w");

    if(track_file == NULL){

        track_file = fopen(file_name, "wt");

    }
    if(track_file == NULL){

        perror(errordesc[E_OPEN_FILE].message);
        zlog_info(category_health_report,
                  errordesc[E_OPEN_FILE].message);
        return E_OPEN_FILE;

    }

    /* Get the number of objects with data to be transmitted */
    number_in_list = get_list_length(&list.list_entry);
    number_to_send = min(MAX_NUM_OBJECTS, number_in_list);


    /*Check if number_to_send is zero. If yes, no need to do more; close
    file and return */
    if(number_to_send == 0){

       fclose(track_file);
       return WORK_SUCCESSFULLY;

    }

    /* Insert device_type and number_to_send at the struct of the track
    file */
    sprintf(basic_info, "%d; %d;", device_type, number_to_send);
    fputs(basic_info, track_file);

#ifdef Debugging

    zlog_debug(category_debug, "Number to send: %d", number_to_send);

#endif


    pthread_mutex_lock(&list_lock);

    /* Set temporary pointer to point to the head of the input list */
    list_pointers = list.list_entry.next;

    /* Set the pointer of the local list head to the head of the input
    list */
    local_object_list_head.next = list_pointers;

    int node_count;

    /* Go through the input tracked_object list to move number_to_send nodes
    in the list to local list */
    for (node_count = 1; node_count <= number_to_send;
                         list_pointers = list_pointers->next){

        /* If the node is the last in the list */
        if(node_count == number_to_send){

            /* Set a marker for the last pointer of the last node */
            tail_pointers = list_pointers;

        }

        node_count = node_count + 1;

    }

    /* Set the head of the input list to point to the last node */
    list.list_entry.next = list_pointers->next;

    /*Set the last node pointing to the local_object_list_head */
    tail_pointers->next = &local_object_list_head;

    pthread_mutex_unlock(&list_lock);

    /* Go throngh the local object list to get the content and write the
       content to file */
    list_for_each(list_pointers, &local_object_list_head){


        temp = ListEntry(list_pointers, ScannedDevice, tr_list_entry);

        /* Convert the timestamp to string */
        unsigned timestamp_init = (unsigned)&temp->initial_scanned_time;
        unsigned timestamp_end = (unsigned)&temp->final_scanned_time;

        /* sprintf() is the function to set a format and convert the
           datatype to char */
        sprintf(timestamp_initial_str, ", %u", timestamp_init);
        sprintf(timestamp_final_str, ", %u", timestamp_end);

        /* Write the content to the file */
        fputs(&temp->scanned_mac_address[0], track_file);
        fputs(timestamp_initial_str, track_file);
        fputs(timestamp_final_str, track_file);
        fputs(";", track_file);

    }

    /* Remove nodes from the local list and release memory allocated to
       nodes that are also not in scanned_device_list */
    free_list(&local_object_list_head, device_type);

    /* Close the file for storing data in the input list */
    fclose(track_file);

    return WORK_SUCCESSFULLY;

}


void free_list(List_Entry *list_entry, DeviceType device_type){


    struct List_Entry *list_pointers, *save_list_pointers;
    ScannedDevice *temp;


    list_for_each_safe(list_pointers,
                       save_list_pointers,
                       list_entry){

        temp = ListEntry(list_pointers, ScannedDevice, tr_list_entry);
        
	pthread_mutex_lock(&list_lock);

        remove_list_node(list_pointers);
    	
	pthread_mutex_unlock(&list_lock);

        if(device_type == BLE){

            mp_free(&mempool, temp);

        }else{

            /* If the node is no longer in scanned list, return the space
             back to the memory pool. */
            if(temp->sc_list_entry.next == temp->sc_list_entry.prev){

                mp_free(&mempool, temp);

            }

        }
    }
    return;
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

void *start_ble_scanning(void){

#ifdef Debugging
        zlog_debug(category_debug, 
		">> start_ble_scanning... ");
#endif
    unsigned char ble_buffer[HCI_MAX_EVENT_SIZE]; /*A buffer for the
                                                      callback event */
    unsigned char *ble_buffer_pointer; /*A pointer for the event buffer */
    int socket = 0; /*Number of the socket */
    int dongle_device_id = 0; /*dongle id */
    struct hci_filter new_filter, original_filiter; /*Filter for controling
                                                      the events*/
    socklen_t olen;
    evt_le_meta_event *meta;
    le_advertising_info *info;
    char addr[18];

    /* Open Bluetooth device */
    socket = hci_open_dev(dongle_device_id);

    if (0 > dongle_device_id || 0 > socket) {

         /* Error handling */
         perror(errordesc[E_OPEN_SOCKET].message);
         zlog_info(category_health_report,
                   errordesc[E_OPEN_SOCKET].message);
         cleanup_exit();
         return;

    }

    if( 0> hci_le_set_scan_parameters(socket, 0x01, htobs(0x0010),
                                      htobs(0x0010), 0x00, 0x00, 1000)){

        /* Error handling */
        perror(errordesc[E_SET_BLE_PARAMETER].message);
        zlog_info(category_health_report,
                   errordesc[E_SET_BLE_PARAMETER].message);


    }


    if( 0> hci_le_set_scan_enable(socket, 0x01, 1, 1000)){

        /* Error handling */
        perror(errordesc[E_BLE_ENABLE].message);
        zlog_info(category_health_report,
                   errordesc[E_BLE_ENABLE].message);

    }

    olen = sizeof(original_filiter);
    if(0 > getsockopt(socket, SOL_HCI, HCI_FILTER, &original_filiter, &olen))
    {

       /* Error handling */
        perror(errordesc[E_GET_BLE_SOCKET].message);
        zlog_info(category_health_report,
                   errordesc[E_GET_BLE_SOCKET].message);

    }

    hci_filter_clear(&new_filter);
    hci_filter_set_ptype(HCI_EVENT_PKT, &new_filter);
    hci_filter_set_event(EVT_LE_META_EVENT, &new_filter);

    if (0 > setsockopt(socket, SOL_HCI, HCI_FILTER, &new_filter,
            sizeof(new_filter)) ) {

        /* Error handling */
        perror(errordesc[E_SCAN_SET_HCI_FILTER].message);
        zlog_info(category_health_report,
                   errordesc[E_SCAN_SET_HCI_FILTER].message);

    }

    bool keep_scanning = true;
    while(false == g_done && true == keep_scanning){

        while (false == g_done && read(socket, ble_buffer, sizeof(ble_buffer)) < 0) {

        }

        ble_buffer_pointer = ble_buffer + (1 + HCI_EVENT_HDR_SIZE);
        meta = (void *) ble_buffer_pointer;

        if (meta->subevent != 0x02){

            keep_scanning = false;
        }

        info = (le_advertising_info *) (meta->data + 1);

        char name[30];
        ba2str(&info->bdaddr, addr);
        eir_parse_name(info->data, info->length,
                            name, sizeof(name) - 1);

        if(strcmp(name, "")!= 0){

            printf("BLE: %s %s \n", addr, name);
            send_to_push_dongle(&info->bdaddr, true);


        }

    }
#ifdef Debugging
        zlog_debug(category_debug, 
		"<< start_ble_scanning... ");
#endif

}


void *start_br_scanning(void) {
#ifdef Debugging
        zlog_debug(category_debug, 
		">> start_br_scanning... ");
#endif

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

    while(false == g_done && true == ready_to_work){

        /* Open Bluetooth device */
        dongle_device_id = hci_get_route(NULL);
        socket = hci_open_dev(dongle_device_id);

        if (0 > dongle_device_id || 0 > socket) {

            /* Error handling */
            perror(errordesc[E_OPEN_SOCKET].message);
            zlog_info(category_health_report,
                      errordesc[E_OPEN_SOCKET].message);
            cleanup_exit();
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

            /* Error handling */
            perror(errordesc[E_SCAN_SET_HCI_FILTER].message);
            zlog_info(category_health_report,
                      errordesc[E_SCAN_SET_HCI_FILTER].message);
            hci_close_dev(socket);
            return;

        }

        hci_write_inquiry_mode(socket, 0x01, 10);

        if (0 > hci_send_cmd(socket, OGF_HOST_CTL, OCF_WRITE_INQUIRY_MODE,
            WRITE_INQUIRY_MODE_RP_SIZE, &inquiry_copy)) {

            /* Error handling */
            perror(errordesc[E_SCAN_SET_INQUIRY_MODE].message);
            zlog_info(category_health_report,
                      errordesc[E_SCAN_SET_INQUIRY_MODE].message);
            hci_close_dev(socket);
            return;

        }

        memset(&inquiry_copy, 0, sizeof(inquiry_copy));

        /* Use the global inquiry access code (GIAC), which has 0x338b9e as its
        lower address part (LAP) */
        inquiry_copy.lap[2] = 0x9e;
        inquiry_copy.lap[1] = 0x8b;
        inquiry_copy.lap[0] = 0x33;

        /* No limit on number of responses per scan */
        inquiry_copy.num_rsp = 0;
        inquiry_copy.length = 0x30;

#ifdef Debugging

        zlog_debug(category_debug, "Starting inquiry with RSSI...");

#endif


        if (0 > hci_send_cmd(socket, OGF_LINK_CTL, OCF_INQUIRY, INQUIRY_CP_SIZE,
            &inquiry_copy)) {

            /* Error handling */
            perror(errordesc[E_SCAN_START_INQUIRY].message);
            zlog_info(category_health_report,
                      errordesc[E_SCAN_START_INQUIRY].message);
            hci_close_dev(socket);
            return;

        }

        output.fd = socket;
        output.events = POLLIN | POLLERR | POLLHUP;


        /* An indicator for continuing to scan the devices. */
        /* After the inquiring events completing, it should jump out of the while
        loop for getting a new socket */

        bool keep_scanning = true;

        while (false == g_done && true == keep_scanning) {

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
                event_buffer_pointer = event_buffer + (1 + HCI_EVENT_HDR_SIZE);
                results = event_buffer_pointer[0];

                switch (event_handler->evt) {

                /* Scanned device with no RSSI value */
                case EVT_INQUIRY_RESULT: {

                    for (results_id = 0; results_id < results; results_id++) {

                        info = (void *)event_buffer_pointer +
                               (sizeof(*info) * results_id) + 1;

                        print_RSSI_value(&info->bdaddr, 0, 0);


                    }

                } break;

                /* Scanned device with RSSI value; when within rangle, send
                message to bluetooth device. */
                case EVT_INQUIRY_RESULT_WITH_RSSI: {

                    for (results_id = 0; results_id < results; results_id++) {

                        info_rssi = (void *)event_buffer_pointer +
                            (sizeof(*info_rssi) * results_id) + 1;


                        if (info_rssi->rssi > RSSI_RANGE) {

                            print_RSSI_value(&info_rssi->bdaddr, 1,
                            info_rssi->rssi);

                            send_to_push_dongle(&info_rssi->bdaddr, false);

                        }

                    }

                } break;

                /* Stop the scanning process */
                case EVT_INQUIRY_COMPLETE: {

                    /* In order to jump out of the while loop. Set Keep_scanning
                    flag to false, new socket will not been received. */
                    keep_scanning = false;

                } break;


                default:

                break;

                }

            }

        } //end while

#ifdef Debugging

        zlog_debug(category_debug, "Scanning done");

#endif

        close(socket);

    }//end while

#ifdef Debugging
        zlog_debug(category_debug, 
		"<< start_br_scanning... ");
#endif

    return;
}

void *timeout_cleanup(void){

#ifdef Debugging
        zlog_debug(category_debug, 
		">> timeout_cleanup... ");
#endif
    /* Create a temporary node and set as the head */
    struct List_Entry *list_pointers, *save_list_pointers;
    ScannedDevice *temp;
    long long start_time = get_system_time();

    while(false == g_done && true == ready_to_work){

        /* In the normal situation, this function would keep sleeping, not
        be executed. */
        sleep(A_LONG_TIME);

        /* If the network is down, set a timer to count down the specific time.
        When timer expires, clean up and remove all the node. */
        while(false == g_done && true == network_is_down){

          if(start_time - get_system_time() >= A_SHORT_TIME){

              /*Check whether the list is empty */
              if(scanned_list_head.list_entry.next
                 != scanned_list_head.list_entry.prev){

                  /* Go throgth lists to release all memory allocated to the
                  nodes */
                  list_for_each_safe(list_pointers,
                                     save_list_pointers,
                                     &scanned_list_head.list_entry){

		      temp = ListEntry(list_pointers, ScannedDevice,
                                       sc_list_entry);

                      pthread_mutex_lock(&list_lock);
                      
                      remove_list_node(&temp->sc_list_entry);

                      pthread_mutex_unlock(&list_lock);
                      
                      if(temp->tr_list_entry.next == temp->tr_list_entry.prev){
                      	  mp_free(&mempool, temp);
                      }
                  }


              }

              if(BR_object_list_head.list_entry.next
                 != BR_object_list_head.list_entry.prev){

                  list_for_each_safe(list_pointers,
                                     save_list_pointers,
                                     &BR_object_list_head.list_entry){

                      temp = ListEntry(list_pointers, ScannedDevice,
                                       tr_list_entry);

                      pthread_mutex_lock(&list_lock);
                      
		      remove_list_node(list_pointers);
                      
		      pthread_mutex_unlock(&list_lock);

                      if(temp->sc_list_entry.next == temp->sc_list_entry.prev){
                      	  mp_free(&mempool, temp);
                      }

                  }
              }


              if(BLE_object_list_head.list_entry.next
                != BLE_object_list_head.list_entry.prev){


                  list_for_each_safe(list_pointers,
                                     save_list_pointers,
                                     &BLE_object_list_head.list_entry){

                      temp = ListEntry(list_pointers, ScannedDevice,
                                       tr_list_entry);

                      pthread_mutex_lock(&list_lock);
                      
		      remove_list_node(list_pointers);
                      
		      pthread_mutex_unlock(&list_lock);

                      mp_free(&mempool, temp);

                  }
              }

              start_time = get_system_time();

          }


        }

    }

#ifdef Debugging
        zlog_debug(category_debug, 
		"<< timeout_cleanup... ");
#endif
    return;

}



ErrorCode startThread(pthread_t *threads ,
                      void * (*threadfunct)(void*),
                      void *arg){

    pthread_attr_t attr;

    if ( pthread_attr_init(&attr) != 0
      || pthread_create(threads, &attr, threadfunct, arg) != 0
      || pthread_attr_destroy(&attr) != 0
      ) {

    return E_START_THREAD;
  }

  return WORK_SUCCESSFULLY;

}


void cleanup_exit(){

#ifdef Debugging
        zlog_debug(category_debug, 
		">> cleanup_exit... ");
#endif
    /* Create a temporary node and set as the head */
    struct List_Entry *list_pointers, *save_list_pointers;
    ScannedDevice *temp;

    /* Set flag to false */
    ready_to_work = false;

    if(&mempool != NULL){

        /* Go throgth two lists to release all memory allocated to the nodes */
        list_for_each_safe(list_pointers,
                       save_list_pointers,
                       &scanned_list_head.list_entry){

            
	    temp = ListEntry(list_pointers, ScannedDevice, sc_list_entry);
            
	    pthread_mutex_lock(&list_lock);
            
	    remove_list_node(list_pointers);
            
            /* Make sure that the node is removed from the
			tracked_BR_object_list. */
            if(temp->tr_list_entry.next != temp->tr_list_entry.prev){
		remove_list_node(&temp->tr_list_entry);
            }
         
	    pthread_mutex_unlock(&list_lock);
         
	    mp_free(&mempool, temp);
	}            

        list_for_each_safe(list_pointers,
                      save_list_pointers,
                      &BR_object_list_head.list_entry){

            temp = ListEntry(list_pointers, ScannedDevice, tr_list_entry);
            
	    pthread_mutex_lock(&list_lock);
            
	    remove_list_node(list_pointers);
            /* Make sure that the node is removed from the
			scanned_list. */
            if(temp->tr_list_entry.next != temp->sc_list_entry.prev){
		remove_list_node(&temp->sc_list_entry);
            }
            
	    pthread_mutex_unlock(&list_lock);
            
            mp_free(&mempool, temp);
        }
        
	list_for_each_safe(list_pointers,
                      save_list_pointers,
                      &BLE_object_list_head.list_entry){

	    temp = ListEntry(list_pointers, ScannedDevice, tr_list_entry);
            
	    pthread_mutex_lock(&list_lock);
            
	    remove_list_node(list_pointers);
            
	    pthread_mutex_unlock(&list_lock);
            
	    mp_free(&mempool, temp);
        }

        mp_destroy(&mempool);


    }

    /* Release the handler for Bluetooth */
    free(g_push_file_path);
#ifdef Debugging
        zlog_debug(category_debug, 
		"<< cleanup_exit... ");
#endif

    return;

}


int main(int argc, char **argv) {

#ifdef Debugging
        zlog_debug(category_debug, 
		">> main... ");
#endif
    /* An iterator through the list of ScannedDevice structs */
    int device_id;

    /* Buffer that contains the hexadecimal location of the beacon */
    char hex_c[CONFIG_BUFFER_SIZE];

    /* Return value of pthread_create used to check for errors */
    int return_value;

    /*Initialize the global flag */
    ready_to_work = true;
    network_is_down = false;

   /* Initialize the application log */
    if (zlog_init("../config/zlog.conf") != 0) {

        perror(errordesc[E_LOG_INIT].message);
    }

    category_health_report = zlog_get_category(LOG_CATEGORY_HEALTH_REPORT);


    if (!category_health_report) {

        zlog_fini();
        perror(errordesc[E_LOG_GET_CATEGORY].message);
        return E_LOG_GET_CATEGORY;
    }

#ifdef Debugging

    category_debug = zlog_get_category(LOG_CATEGORY_DEBUG);
    if (!category_debug) {

        zlog_fini();
        perror(errordesc[E_LOG_GET_CATEGORY].message);
        return E_LOG_GET_CATEGORY;
    }

    zlog_debug(category_debug, "Finish initializing zlog");

#endif



    /*Initialize the global lists */
    init_entry(&scanned_list_head.list_entry);
    scanned_list_head.device_type = BR_EDR;
    init_entry(&BR_object_list_head.list_entry);
    BR_object_list_head.device_type = BR_EDR;
    init_entry(&BLE_object_list_head.list_entry);
    BLE_object_list_head.device_type = BLE;

    /* Initialize the memory pool */
    if(mp_init(&mempool, sizeof(struct ScannedDevice), SLOTS_IN_MEM_POOL)
            == NULL){

        /* Error handling */
        perror(errordesc[E_MALLOC].message);
        zlog_info(category_health_report, errordesc[E_MALLOC].message);
        cleanup_exit();
        return E_MALLOC;

    }

     /* Initialize the zigbee at a very beginning. Because it takes longer
        time  */
    /*
    if(zigbee_init() != XBEE_SUCCESSFULLY){

        /* Could not initialize the zigbee, handle error */
    /*
        perror(errordesc[E_INIT_ZIGBEE].message);
        zlog_info(category_health_report,
                  errordesc[E_INIT_ZIGBEE].message);

        /* Set the flag of network_is_down to true. It can inform the timeout
        cleanup function to be executed */
        //network_is_down == true;

        /* Not a deadly error, should not shut down the system */
        //cleanup_exit();

        //return;

    //}

    /* Initialize the lock for accessing the lists */
    pthread_mutex_init(&list_lock,NULL);

    /* Load config struct */
    g_config = get_config(CONFIG_FILE_NAME);
    g_push_file_path =
        malloc(g_config.file_path_length + g_config.file_name_length);


    if (g_push_file_path == NULL) {

         /* Error handling */
        perror(errordesc[E_MALLOC].message);
        zlog_info(category_health_report, errordesc[E_MALLOC].message);
        cleanup_exit();
        return E_MALLOC;

    }

    memcpy(g_push_file_path, g_config.file_path,
           g_config.file_path_length - 1);
    memcpy(g_push_file_path + g_config.file_path_length - 1,
           g_config.file_name, g_config.file_name_length - 1);


    coordinate_X.f = (float)atof(g_config.coordinate_X);
    coordinate_Y.f = (float)atof(g_config.coordinate_Y);
    coordinate_Z.f = (float)atof(g_config.coordinate_Z);


    /* the  maximum number of devices of an array */
    int maximum_number_of_devices = atoi(g_config.maximum_number_of_devices);



    /* Initialize each ThreadStatus struct in the g_idle_handler array */
    for (device_id = 0; device_id < maximum_number_of_devices; device_id++) {

         strncpy(g_idle_handler[device_id].scanned_mac_address, "0",
         LENGTH_OF_MAC_ADDRESS);
        g_idle_handler[device_id].idle = true;
        g_idle_handler[device_id].is_waiting_to_send = false;

    }


    /* Store coordinates of the beacon */
    sprintf(hex_c,
            "OPENISDMN402%02x%02x%02x%02xD0F5%02x%02x%02x%02x48D2B060",
            coordinate_X.b[0], coordinate_X.b[1], coordinate_X.b[2],
            coordinate_X.b[3], coordinate_Y.b[0], coordinate_Y.b[1],
            coordinate_Y.b[2], coordinate_Y.b[3]);


    strcpy(g_config.uuid, hex_c);

    /* Create the thread for advertising to bluetooth devices */
    pthread_t stop_broadcast_thread;

    return_value = startThread(&stop_broadcast_thread,
                               stop_broadcast, hex_c);

    if(return_value != WORK_SUCCESSFULLY){

        perror(errordesc[E_START_THREAD].message);
        zlog_info(category_health_report,
                  errordesc[E_START_THREAD].message);
        cleanup_exit();
        return E_START_THREAD;
    }


    /* Create the thread for track BR_EDR device */
    pthread_t br_scanning_thread;

    return_value = startThread(&br_scanning_thread,
                               start_br_scanning, NULL);

    if(return_value != WORK_SUCCESSFULLY){

        perror(errordesc[E_START_THREAD].message);
        zlog_info(category_health_report,
                  errordesc[E_START_THREAD].message);
        cleanup_exit();
        return E_START_THREAD;
    }

    /* Create the thread for track BLE device */

    pthread_t ble_scanning_thread;

    return_value = startThread(&ble_scanning_thread,
                               start_ble_scanning, NULL);

    if(return_value != WORK_SUCCESSFULLY){

        perror(errordesc[E_START_THREAD].message);
        zlog_info(category_health_report,
                  errordesc[E_START_THREAD].message);
        cleanup_exit();
        return E_START_THREAD;
    }


    /* Create the the cleanup_scanned_list thread */

    pthread_t cleanup_scanned_list_thread;

    return_value = startThread(&cleanup_scanned_list_thread,
                               cleanup_scanned_list, &scanned_list_head);

    if(return_value != WORK_SUCCESSFULLY){

        perror(errordesc[E_START_THREAD].message);
        zlog_info(category_health_report,
                  errordesc[E_START_THREAD].message);
        cleanup_exit();
        return E_START_THREAD;
    }

    /* Create the thread for track device */
    pthread_t track_communication_thread;

    return_value = startThread(&track_communication_thread,
                               manage_communication, NULL);

    if(return_value != WORK_SUCCESSFULLY){

        perror(errordesc[E_START_THREAD].message);
        zlog_info(category_health_report,
                  errordesc[E_START_THREAD].message);
        cleanup_exit();
        return E_START_THREAD;
    }


    /* Create the thread for track device */
    pthread_t timer_thread;

    return_value = startThread(&timer_thread,
                               timeout_cleanup, NULL);

    if(return_value != WORK_SUCCESSFULLY){

        perror(errordesc[E_START_THREAD].message);
        zlog_info(category_health_report,
                  errordesc[E_START_THREAD].message);
        cleanup_exit();
        return E_START_THREAD;
    }


/* The code for communication over Bluetooth BR/EDR protocol path using
   additional device  */
#ifdef Bluetooth_classic

    int number_of_push_dongles = atoi(g_config.number_of_push_dongles);
    int maximum_number_of_devices_per_dongle =
        maximum_number_of_devices / number_of_push_dongles;

    /* An iterator through each push dongle */
    int push_dongle_id;

    /* An iterator through a block of devices per dongle */
    int block_id;

    int dongle_device_id = 0; /*Device ID of dongle */


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

            perror(errordesc[E_START_THREAD].message);
            zlog_info(category_health_report,
                      errordesc[E_START_THREAD].message);
            cleanup_exit();
            return 1;
        }


    }

    /*Set send_message_cancelled flag to false now. All the thread are
      ready.*/
    send_message_cancelled = false;


     /* ready_to_work = false , shut down.
     * wait for send_file_thread to exit. */

    for (device_id = 0; device_id < maximum_number_of_devices; device_id++) {

        return_value = pthread_join(send_file_thread[device_id], NULL);

        if (return_value != 0) {

            perror(strerror(errno));
            zlog_info(category_health_report, strerror(errno));
            cleanup_exit();
            return;

        }
    }

#endif //Bluetooth_classic


#ifdef Debugging

    zlog_debug(category_debug, "All the threads are created.");

#endif

    return_value = pthread_join(cleanup_scanned_list_thread, NULL);

    if (return_value != 0) {

        perror(strerror(errno));
        zlog_info(category_health_report, strerror(errno));
        cleanup_exit();
        return;

    }

    return_value = pthread_join(stop_broadcast_thread, NULL);

    if (return_value != 0) {
        perror(strerror(errno));
        zlog_info(category_health_report, strerror(errno));
        cleanup_exit();
        return;

    }

    cleanup_exit();

#ifdef Debugging
        zlog_debug(category_debug, 
		"<< main... ");
#endif

    return 0;
}



/* Follow are functions for communication via BR/EDR path to Bluetooth
   classic devices */
#ifdef Bluetooth_classic


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
        perror("Directories do not exist");
        return NULL;
    }

    /* Stores file path of message_to_send */
    char file_path[FILE_NAME_BUFFER];
    memset(file_path, 0, FILE_NAME_BUFFER);
    message_id = 0;

    /* Go through each message in directory and store each file name */
    for (group_id = 0; group_id < number_of_groups; group_id++) {
        /* Concatenate strings to make file path */
        sprintf(file_path, "/home/pi/LBeacon/messages/");
        strcat(file_path, groups[group_id]);

        DIR *messagedir;
        struct dirent *messageent;
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
            perror("Message files do not exist");
            return NULL;
        }

        return;

    }

    /* Error handling */
    perror("Message files do not exist");
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


    while (false == g_done && false == send_message_cancelled) {

        for (device_id = 0; device_id < maximum_number_of_devices;
            device_id++) {

            if (device_id == thread_id &&
                g_idle_handler[device_id].is_waiting_to_send == true) {


                /* Open socket and use current time as start time to keep
                 * of how long has taken to send the message to the device */
                socket = hci_open_dev(dongle_device_id);


                if (0 > dongle_device_id || 0 > socket) {

                    /* Error handling */
                    perror(errordesc[E_SEND_OPEN_SOCKET].message);
                    strncpy(
                            g_idle_handler[device_id].scanned_mac_address,
                            "0",
                            LENGTH_OF_MAC_ADDRESS);

                    g_idle_handler[device_id].idle = true;
                    g_idle_handler[device_id].is_waiting_to_send = false;
                    break;

                }

                long long start = get_system_time();
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
                long long end = get_system_time();
                printf("Time to open connection: %lld ms\n", end - start);

                if (client == NULL) {

                    /* Error handling */
                    perror(errordesc[E_SEND_OBEXFTP_CLIENT].message);
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
                    perror(errordesc[E_SEND_CONNECT_DEVICE].message);
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
                    perror(errordesc[E_SEND_PUSH_FILE].message);
                }

                /* Disconnect connection */
                return_value = obexftp_disconnect(client);
                if (0 > return_value) {

                    /* TODO: Error handling  */
                    perror(errordesc[E_SEND_DISCONNECT_CLIENT].message);
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
