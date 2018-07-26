/*
* Copyright (c) 2016 Academia Sinica, Institute of Information Science
*
* License:
*
*      GPL 3.0 : The content of this file is subject to the terms and
*      cnditions defined in file 'COPYING.txt', which is part of this source
*      code package.
*
* Project Name:
*
*      BeDIPS
*
* File Description:
*
*      This file contains the program to allow the beacon to discover
*   bluetooth devices and then scan the Bluetooth addresses of the devices.
*   Depending on the RSSI value of each discovered and scanned deviced,
*   the beacon determines whether it should send location related files to
*   the device.
*
* File Name:
*
*      LBeacon.c
*
* Version:
* 
*       1.2
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
*
*      Han Wang, hollywang@iis.sinica.edu.tw
*      Jake Lee, jakelee@iis.sinica.edu.tw
*      Johnson Su, johnsonsu@iis.sinica.edu.tw
*      Shirley Huang, shirley.huang.93@gmail.com
*      Han Hu, hhu14@illinois.edu
*      Jeffrey Lin, lin.jeff03@gmail.com
*      Howard Hsu, haohsu0823@gmail.com
*      
*/

#include "LBeacon.h"




Config get_config(char *file_name) {

    /* Return value is a struct containing all config information */
    Config config;

    FILE *file = fopen(file_name, "r");
    if (file == NULL) {

        /* Error handling */
        perror(errordesc[E_OPEN_FILE].message);
        cleanup_exit();
        return;

    }
    else {
    /* Create spaces for storing the string of the current line being read */
    char config_setting[CONFIG_BUFFER_SIZE];
    char *config_message[ConFIG_FILE_LENGTH];

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





void send_to_push_dongle(bdaddr_t *bluetooth_device_address) {

    /* Stores the MAC address as a string */
    char address[LENGTH_OF_MAC_ADDRESS];

    /* Converts the bluetooth device address to a string */
    ba2str(bluetooth_device_address, address);
    strcat(address, "\0");

    struct ScannedDevice *new_node;
    new_node = check_is_in_scanned_list(address);
   
    
    /* If check_is_in_scanned_list() returns null, allocate memory form
       memory pool for a new node, initilize the node, and insert the 
       new  node to the scanned list. */
    if (new_node == NULL) {

      

        printf("******Get the memory from the pool. ****** \n");
        new_node = (struct ScannedDevice*) mp_alloc(&mempool);
        
        /* Initialize the list entries */
        init_entry(&new_node->sc_list_entry);
        init_entry(&new_node->tr_list_entry);

        /* Get the initial scan time for the new node. */
        new_node->initial_scanned_time = get_system_time();
        new_node->final_scanned_time = get_system_time();

        /* Copy the MAC address to the node */
        strncpy(new_node->scanned_mac_address, 
                address, 
                LENGTH_OF_MAC_ADDRESS);

        
        pthread_mutex_lock(&list_lock);
        /* Insert the new code to the scanned list */
        insert_list_first(&new_node->sc_list_entry, &scanned_list_head);

        /* Insert the new node to the track_object_list  */
        insert_list_first(&new_node->tr_list_entry, 
                            &tracked_object_list_head);

        pthread_mutex_unlock(&list_lock);
       

    
    }else{
        

        pthread_mutex_lock(&list_lock);
         
        /* Update the final scan time */
        new_node->final_scanned_time = get_system_time();
        
        pthread_mutex_unlock(&list_lock);
       
    }

}




struct ScannedDevice *check_is_in_scanned_list(char address[]) {

    /* Create a temporary node and set as the head */
    struct List_Entry *listptrs;
    ScannedDevice *temp;

    /* If there is no node in the list, reutrn NULL directly. */
    if(scanned_list_head.next == scanned_list_head.prev){
       
        return NULL;

    }

   
    /* Go through list */
    list_for_each(listptrs, &scanned_list_head) {

        temp = ListEntry(listptrs, ScannedDevice, sc_list_entry);
        
        int len = strlen(address);

        char *addr_last_two = &address[len - NO_DIGITS_TO_COMPARE];
        char *temp_last_two = &temp->scanned_mac_address[len - NO_DIGITS_TO_COMPARE];

        /* Compare the last two digits of the MAC address */
        if ((!strncmp(address, temp->scanned_mac_address, NO_DIGITS_TO_COMPARE))&&
            (!strncmp(addr_last_two, temp_last_two, NO_DIGITS_TO_COMPARE))) {

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
        fprintf(stderr, "Can't send request %s (%d)\n", strerror(errno),
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
        fprintf(stderr, "Can't send request %s (%d)\n", strerror(errno),
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
        fprintf(stderr, "Can't send request %s (%d)\n", strerror(errno),
                errno);
        return E_SEND_REQUEST_TIMEOUT;
    }

    if (status) {
        /* Error handling */
        fprintf(stderr, "LE set advertise returned status %d\n", status);
        return E_ADVERTISE_STATUS;
    }

    return WORK_SCUCESSFULLY;
}



ErrorCode disable_advertising() {

    int dongle_device_id = hci_get_route(NULL);
    int device_handle = 0;
    if ((device_handle = hci_open_dev(dongle_device_id)) < 0) {
        /* Error handling */
        perror(errordesc[E_OPEN_FILE].message);
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
        fprintf(stderr, "Can't set advertise mode: %s (%d)\n",
                strerror(errno), errno);
        return E_ADVERTISE_MODE;

    }

    if (status) {

        /* Error handling */
        fprintf(stderr, "LE set advertise enable on returned status %d\n",
            status);
        return E_ADVERTISE_STATUS;

    }


    return WORK_SCUCESSFULLY;

}




void *stop_ble_beacon(void *beacon_location) {

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
            return;
        }

        perror("Hit ctrl-c to stop advertising");

        while (g_done == false) {
            sleep(1);
        }

        /* When signal is received, disable message advertising */
        perror("Shutting down");
        disable_advertising();

    }

    if(ready_to_work == false){

        return;
    }

}





void *cleanup_scanned_list(void) {


    struct List_Entry *listptrs;
    ScannedDevice *temp;

    while (ready_to_work == true) {

        /*Check whether the list is empty */
        while(scanned_list_head.next == scanned_list_head.prev){
            
            sleep(TIMEOUT_WAITING);

        }
 
       pthread_mutex_lock(&list_lock);

        /* Go through list */
        list_for_each(listptrs, &scanned_list_head){

            temp = ListEntry(listptrs, ScannedDevice, sc_list_entry);


            /* Device has been in the scanned list for at least 30 seconds */
            if (get_system_time() - temp->initial_scanned_time > TIMEOUT) {

                
                /* Remove this scanned device node from the scanned list */
                remove_list_node(&temp->sc_list_entry);


                /* If the node no longer in the other list, free the space
                   back to the memory pool. */
                if(&temp->tr_list_entry.next 
                                        == &temp->tr_list_entry.prev){

                    mp_free(&mempool, temp);

                }
                
            /* Because of setting the current node's pointer to NULL, this 
               function breaks the loop (list_for_each) in order to aviod 
               continuously visiting to NULL.
               The process will back to the beginning of while loop.
            */
                break;
               

            }
            else {
                continue;
            }
        }

        pthread_mutex_unlock(&list_lock);

        

    }


    return;

}




void *manage_communication(void) {

    /* Struct for storing necessary objects for zigbee connection */
    Zigbee zigbee;
    Threadpool thpool;
    FILE *file_to_send;
    
    zigbee_init(zigbee);

    /* Initialize the thread pool and create two sub-threads waiting for the 
       new work/job assigned. */
    thpool = thpool_init(NO_WORK_THREADS); 
    if(thpool == NULL){
        
        /* Could not create thread pool, handle error */
        perror(errordesc[E_INIT_THREAD_POOL].message);
        cleanup_exit();

        return;
        
    }
    
    while(ready_to_work == true){

        /* Checking the call back from the gateway. If not getting anything 
           from the gateway, sleep for  a short time. If polled, according
           to the received message, different action would take. */
        int polled_type = 0;
        polled_type = receive_call_back(zigbee);
        printf("Polled: %d \n", polled_type);
        
        while(polled_type == NOT_YET_POLLED){

            printf("Polled is going to sleep \n");
            sleep(TIMEOUT_WAITING);
        
        }

        /* According to the polled data type, different work item to be 
        prepared */
        switch(polled_type){

            case TRACK_OBJECT_DATA:

                /* Function call to execute copy_track_object_to_data in order
                   to create file to be transmit */
                copy_object_data_to_file("output.txt");


                /* Open the file is going to sent to the gateway */
                file_to_send = fopen("output.txt", "r");
                if (file_to_send == NULL) {

                    /* Error handling */
                    perror(errordesc[E_OPEN_FILE].message);
                    strcpy(zigbee.zig_message, 
                                "Nothing can be found in this LBeacon");
                    
                    return;
                    }

                /* Read the file to get the content for the message to send */
                fgets(zigbee.zig_message, sizeof(zigbee.zig_message), 
                                                            file_to_send);
            
                printf("Message: %s \n", zigbee.zig_message);


             break;

            case HEALTH_REPORT:

            /* TODO:
               Create the file for the health report. The files contains the 
               error log. will be done as soon as possible */ 


            break;

            default:

            break;
        
        }
        
        /* Add the work item to the work thread */
        if(thpool_add_work(thpool, (void*)zigbee_send_file, &zigbee) != 0){

            /* Error handling */
            perror(errordesc[E_OPEN_FILE].message);
            cleanup_exit();
            return;
                
        }


    } // end of the while 

    /*Close the file for transmission */
    fclose(file_to_send);

    /* After the ready_to_work is set to false, clean up the zigbee and the 
       thread pool */
    zigbee_free(zigbee);

    /* Free the thread pool */
    thpool_destroy(thpool);

    return;

}



void copy_object_data_to_file(char *file_name) {

    FILE *track_file;
    
    /* Head of a local list for track object */
    List_Entry local_object_list_head;

    /* Two pointers to be used locally */
    struct List_Entry *lisptrs, *tailptrs;
    
    ScannedDevice *temp;
    int number_in_list;
    int number_to_send;
    char timestamp_initial_str[LENGTH_OF_TIME];
    char timestamp_final_str[LENGTH_OF_TIME];

    /* Initilize the local list */
    init_entry(&local_object_list_head);
    number_in_list = get_list_length(&tracked_object_list_head);
    
    /* Get the smaller amount for transmission */
    number_to_send = min(MAX_NO_OBJECTS, number_in_list);

    printf("Number in list: %d\n", number_in_list);

    /*Check the list if it is empty, if yes, return false */
    if(number_in_list == 0){  
       sleep(30); 
       return;
        
    }
        

    /* Create a new file to store data in the tracked_object_list */        
    track_file = fopen(file_name, "w");
        
    if(track_file == NULL){

        track_file = fopen(file_name, "wt");
        
    }
    if(track_file == NULL){
        
        perror(errordesc[E_OPEN_FILE].message);
        return false;

    }
    
    pthread_mutex_lock(&list_lock);

    lisptrs = tracked_object_list_head.next;

    /* Set the pointer of the local list head to the current */
    local_object_list_head.next = lisptrs;
    
    int node_count;
    
    /* Go through the track_object_list to move the nodes in the list
       to local list */
    for (node_count = 1; node_count <= number_to_send || 
                        lisptrs != (&tracked_object_list_head); 
                        lisptrs = lisptrs->next){

        /* If the node is the last in the list */
        if(node_count == number_to_send){

            /* A marker for the last pointer */
            tailptrs = lisptrs;
            
        }
        node_count++;

    }

    /* Set the track_object_list_head points to the last node */
    tracked_object_list_head.next = lisptrs;
    /*Set the last node pointing to the local_object_list_head */
    tailptrs->next = &local_object_list_head;

    pthread_mutex_unlock(&list_lock);

    /* Go throngh the local object list to get the content and to write
       the file */
    list_for_each(lisptrs, &local_object_list_head){


        temp = ListEntry(lisptrs, ScannedDevice, tr_list_entry);

        /* Convert the timestamp from list to string */
        unsigned timestamp_init = (unsigned)&temp->initial_scanned_time;
        unsigned timestamp_end = (unsigned)&temp->final_scanned_time;
        sprintf(timestamp_initial_str, ", %u", timestamp_init);
        sprintf(timestamp_final_str, ", %u", timestamp_end);

        /* Write the content to the file */
        fputs(&temp->scanned_mac_address[0], track_file);               
        fputs(timestamp_initial_str, track_file);
        fputs(timestamp_final_str, track_file);
        fputs(";", track_file);

    }


    /* Free the local list */
    free_List(&local_object_list_head, number_to_send);

    /* If the node no longer in the other list, free the space
       back to the memory pool. */
    if(&temp->sc_list_entry.next == &temp->sc_list_entry.prev){

        mp_free(&mempool, temp);

    }


    /* Close the file for tracking */
    fclose(track_file);
    
    return;

}


void free_List(List_Entry *entry, int numnode){

    /* Create a temporary node and set as the head */
    struct List_Entry *lisptrs;
    ScannedDevice *temp;

   
    while(numnode != 0){

        /* Always get the head of the list */ 
        lisptrs = entry->next;
        remove_list_node(lisptrs);

        numnode --;

    }

    return;
}



void start_scanning() {



    struct hci_filter filter; /*Filter for controling the events*/
    struct pollfd output; /*A callback event from the socket */
    unsigned char event_buffer[HCI_MAX_EVENT_SIZE]; /*A buffer for the
                                                      *callback event*/
    unsigned char *event_buffer_pointer; /*A pointer for the event buffer */
    hci_event_hdr *event_handler; /*Record the event type */
    inquiry_cp inquiry_copy; /*Storing the message from the socket */
    inquiry_info_with_rssi *info_rssi; /*Record an
                                         *EVT_INQUIRY_RESULT_WITH_RSSI message
                                         */
    inquiry_info *info; /*Record an EVT_INQUIRY_RESULT message */
    int event_buffer_length; /*Length of the event buffer */
    int dongle_device_id = 0; /*dongle id */
    int socket = 0; /*Number of the socket */
    int results; /*Return the result form the socket */
    int results_id; /*ID of the result */

    /* Open Bluetooth device */
    socket = hci_open_dev(dongle_device_id);

    if (0 > dongle_device_id || 0 > socket) {

         /* Error handling */
         perror(errordesc[E_OPEN_SOCKET].message);
         ready_to_work = false;
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
         hci_close_dev(socket);
         return;

    }

    hci_write_inquiry_mode(socket, 0x01, 10);

    if (0 > hci_send_cmd(socket, OGF_HOST_CTL, OCF_WRITE_INQUIRY_MODE,
         WRITE_INQUIRY_MODE_RP_SIZE, &inquiry_copy)) {

         /* Error handling */
         perror(errordesc[E_SCAN_SET_INQUIRY_MODE].message);
         hci_close_dev(socket);
         return;

    }

    memset(&inquiry_copy, 0, sizeof(inquiry_copy));
    inquiry_copy.lap[2] = 0x9e;
    inquiry_copy.lap[1] = 0x8b;
    inquiry_copy.lap[0] = 0x33;
    inquiry_copy.num_rsp = 0;
    inquiry_copy.length = 0x30;
    printf("Starting inquiry with RSSI...\n");

    if (0 > hci_send_cmd(socket, OGF_LINK_CTL, OCF_INQUIRY, INQUIRY_CP_SIZE,
         &inquiry_copy)) {

         /* Error handling */
         perror(errordesc[E_SCAN_START_INQUIRY].message);
         hci_close_dev(socket);
         return;

    }

    output.fd = socket;
    output.events = POLLIN | POLLERR | POLLHUP;


    /* An indicator for continuing to scan the devices. */
    /* After the inquiring events completing, it should jump out of the while
     * loop for getting a new socket */
    bool keep_scanning = true;

    while (keep_scanning == true) {

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
            * message to bluetooth device. */
            case EVT_INQUIRY_RESULT_WITH_RSSI: {

                for (results_id = 0; results_id < results; results_id++) {
                    info_rssi = (void *)event_buffer_pointer +
                         (sizeof(*info_rssi) * results_id) + 1;

                   

                     if (info_rssi->rssi > RSSI_RANGE) {

                         
                         print_RSSI_value(&info_rssi->bdaddr, 1,
                         info_rssi->rssi);

                         send_to_push_dongle(&info_rssi->bdaddr);

                     }

                 }

            } break;

            /* Stop the scanning process */
            case EVT_INQUIRY_COMPLETE: {

                 /* In order to jump out of the while loop. If without this
                  * flag, new socket will not been received. */
                 keep_scanning = false;

            } break;

            default:

            break;

            }

         }

    } //end while



    printf("Scanning done\n");
    close(socket);

    return;
}



ErrorCode startThread(pthread_t threads ,void * (*threadfunct)(void*), void *arg){

    pthread_attr_t attr;

    if ( pthread_attr_init(&attr) != 0
      || pthread_create(&threads, &attr, threadfunct, arg) != 0
      || pthread_attr_destroy(&attr) != 0
      || pthread_detach(threads) != 0) {

    return E_START_THREAD;
  }

  return WORK_SCUCESSFULLY;

}




/*
*  cleanup_exit:
*
*  This function releases all the resources.
*
*  Parameters:
*
*  None
*
*  Return value:
*
*  None
*/

void cleanup_exit(){

    /* Set two flags to false */
    ready_to_work = false;
  
    
    /* Free the memory pool */
    mp_destroy(&mempool);
    
    /* Release the handler for Bluetooth */ 
    free(g_push_file_path);
    
   
    return;

}



int main(int argc, char **argv) {

    /* An iterator through the list of ScannedDevice structs */
    int device_id;

    /* Buffer that contains the hexadecimal location of the beacon */
    char hex_c[CONFIG_BUFFER_SIZE];

    /* Return value of pthread_create used to check for errors */
    int return_value;

    /*Initialize the global flag */
    ready_to_work = true;
   



    /*Initialize the lists */
    init_entry(&scanned_list_head); 
    init_entry(&tracked_object_list_head);

    /* Initialize the memory pool */
    if(mp_init(&mempool, sizeof(struct ScannedDevice), SLOTS_FOR_MEM_POOL) 
            == NULL){

        /* Error handling */
        perror(errordesc[E_MALLOC].message);
        cleanup_exit();
        return E_MALLOC;

    }
    
    /* Initialize two locks for two lists */
    pthread_mutex_init(&list_lock,NULL);


    /* Load config struct */
    g_config = get_config(CONFIG_FILE_NAME);
    g_push_file_path =
        malloc(g_config.file_path_length + g_config.file_name_length);


    if (g_push_file_path == NULL) {

         /* Error handling */
        perror(errordesc[E_MALLOC].message);
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


    /* Allocate an array with the size of maximum number of devices */
    int maximum_number_of_devices = atoi(g_config.maximum_number_of_devices);
    


    /* Initialize each ThreadStatus struct in the array */
    for (device_id = 0; device_id < maximum_number_of_devices; device_id++) {

         strncpy(g_idle_handler[device_id].scanned_mac_address, "0",
         LENGTH_OF_MAC_ADDRESS);
        g_idle_handler[device_id].idle = true;
        g_idle_handler[device_id].is_waiting_to_send = false;

    }


    /* Store coordinates of the beacon location */
    sprintf(hex_c,
            "OPENISDMN402%02x%02x%02x%02xD0F5%02x%02x%02x%02x48D2B060",
            coordinate_X.b[0], coordinate_X.b[1], coordinate_X.b[2],
            coordinate_X.b[3], coordinate_Y.b[0], coordinate_Y.b[1],
            coordinate_Y.b[2], coordinate_Y.b[3]);



    /* Create the thread for message advertising to BLE bluetooth devices */
    pthread_t stop_ble_beacon_thread;

    return_value = startThread(stop_ble_beacon_thread, 
                                    stop_ble_beacon, hex_c);

    if(return_value != WORK_SCUCESSFULLY){
         perror(errordesc[E_START_THREAD].message);
        cleanup_exit();
    }


    /* Create the the cleanup_scanned_list thread */
    pthread_t cleanup_scanned_list_thread;

    return_value = startThread(cleanup_scanned_list_thread,
                                cleanup_scanned_list, NULL);

    if(return_value != WORK_SCUCESSFULLY){
         perror(errordesc[E_START_THREAD].message);
        cleanup_exit();
    }



    /* Create the thread for track device */
    pthread_t track_communication_thread;

    return_value = startThread(track_communication_thread, 
                                    manage_communication, NULL);

    if(return_value != WORK_SCUCESSFULLY){
         perror(errordesc[E_START_THREAD].message);
        cleanup_exit();
    }


  
    while(ready_to_work == true){

        start_scanning();

    }

   


    return_value = pthread_join(cleanup_scanned_list_thread, NULL);

    if (return_value != 0) {
        perror(strerror(errno));
        cleanup_exit();
        return;

    }


    pthread_cancel(stop_ble_beacon_thread);
    return_value = pthread_join(stop_ble_beacon_thread, NULL);

    if (return_value != 0) {
        perror(strerror(errno));
        cleanup_exit();
        return;

    }

    cleanup_exit();


    return 0;
}
