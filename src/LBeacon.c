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
*      Jake Lee, jakelee@iis.sinica.edu.tw
*      Johnson Su, johnsonsu@iis.sinica.edu.tw
*      Shirley Huang, shirley.huang.93@gmail.com
*      Han Hu, hhu14@illinois.edu
*      Jeffrey Lin, lin.jeff03@gmail.com
*      Howard Hsu, haohsu0823@gmail.com
*      Han Wang, hollywang@iis.sinica.edu.tw    
*/

#include "LBeacon.h"


/*
*  get_config:
*
*  This function goes through the config file, reads line by line until the
*  end of file, and stores the data into the global variable of a Config 
*  struct.
*
*  Parameters:
*
*  file_name - the name of the config file that stores all the beacon data
*
*  Return value:
*
*  config - Config struct including file path, coordinates, etc.
*/
Config get_config(char *file_name) {
    
    /* Return value that contains a struct of all config information */
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
    char *config_message[11];

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

/*
*  get_system_time:
*
*  This helper function fetches the current time according to the system
*  clock in terms of the number of milliseconds since January 1, 1970.
*
*  Parameters:
*
*  None
*
*  Return value:
*
*  system_time - system time in milliseconds
*/
long long get_system_time() {
    /* A struct that stores the time */
    struct timeb t;

    /* Return value as a long long type */
    long long system_time;

    /* Convert time from Epoch to time in milliseconds as a long long type */
    ftime(&t);
    system_time = 1000 * t.time + t.millitm;

    return system_time;
}


/*
*  send_to_push_dongle:
*
*  For each newly scanned bluetooth device, this function adds the Scanned 
*  Device struct of the device to the scanned list of ScannedDevice struct 
*  that stores  its scanned timestamp and MAC address and to the waiting list 
*  of MAC addresses  waiting for an available thread to send a message to the 
*  device with the address.
*
*  Parameters:
*
*  bluetooth_device_address - bluetooth device address
*
*  Return value:
*
*  None
*/
void send_to_push_dongle(bdaddr_t *bluetooth_device_address) {
    
    /* Stores the MAC address as a string */
    char address[LENGTH_OF_MAC_ADDRESS];
    
    /* Converts the bluetooth device address to a string */
    ba2str(bluetooth_device_address, address);
    strcat(address, "\0");
    
    /* Add newly scanned devices to the scanned list and waiting list for new
     * scanned devices */
    if (check_is_in_list(scanned_list, address) == false) {       
        
        ScannedDevice data;
        data.initial_scanned_time = get_system_time();
        strncpy(data.scanned_mac_address, address, LENGTH_OF_MAC_ADDRESS); 
        struct Node *node_s, *node_w;
        node_s = (struct Node*)malloc(sizeof(struct Node));
        node_w = (struct Node*)malloc(sizeof(struct Node));
        list_insert_first(&node_s->ptrs, scanned_list);
        list_insert_first(&node_w->ptrs, waiting_list);
        node_s->data = &data;
        node_w->data = &data;
        
    }
}



/*
*  print_RSSI_value:
*
*  This function prints the RSSI value along with the MAC address of the 
*  user's scanned bluetooth device. When the LBeacon is running, we will 
*  continuously see a list of scanned bluetooth devices running in the 
*  console.
*
*  Parameters:
*
*  bluetooth_device_address - bluetooth device address
*  has_rssi - whether the bluetooth device has an RSSI value or not
*  rssi - RSSI value of bluetooth device
*
*  Return value:
*
*  None
*/
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



/*
*  track_devices:
*
*  This function tracks the MAC addresses of scanned bluetooth devices under
*  the beacon. An output file will contain for each timestamp and the MAC
*  addresses of the scanned bluetooth devices at the given timestamp.
*
*  Parameters:
*
*  bluetooth_device_address - bluetooth device address
*  file_name - name of the file where all the data will be stored
*
*  Return value:
*
*  None
*/
void track_devices(bdaddr_t *bluetooth_device_address, char *file_name) {
    /* Scanned MAC address */
    char address[LENGTH_OF_MAC_ADDRESS];

    /* Converts long long type to a string */
    char long_long_to_string[LENGTH_OF_TIME];
    char long_long_to_string_init[LENGTH_OF_TIME];
 
    /* Get current timestamp when tracking bluetooth devices */
    unsigned timestamp = (unsigned)time(NULL);
    sprintf(long_long_to_string, "%u", timestamp);

    /* If file is empty, create new file with LBeacon UUID */
    if (0 == g_size_of_file) {
    
        FILE *output = fopen(file_name, "w+"); /* w+ overwrites the file */
    
        if (output == NULL) {
    
            /* Error handling */
            perror(errordesc[E_OPEN_FILE].message);
            return;
    
        }
    
        fputs("LBeacon UUID: ", output);
        fputs(g_config.uuid, output);
        fclose(output);
        g_size_of_file++;
        g_initial_timestamp_of_tracking_file = timestamp;
        sprintf(long_long_to_string_init, "%u",
                g_initial_timestamp_of_tracking_file);
        memset(&address[0], 0, sizeof(address));
    
    }

    /* Converts the bluetooth device address to a string */
    ba2str(bluetooth_device_address, address);
    strcat(address, "\0");

    FILE *output;
    char line[TRACKING_FILE_LINE_LENGTH ];
    output = fopen(file_name, "a+"); /* a+ appends to the file */

    if (output == NULL) {
    
        /* Error handling */
        perror(errordesc[E_OPEN_FILE].message);
        return;
    
    }

    /* Go through the whole file to get to the last line */
    while (fgets(line, TRACKING_FILE_LINE_LENGTH , output) != NULL) {
    }

    /* If timestamp already exists, add MAC address to end of the previous 
     * line otherwise create a new line */
    if (timestamp != g_most_recent_timestamp_of_tracking_file) {
        fputs("\n", output);
        fputs(long_long_to_string, output);
        fputs(" - ", output);
        fputs(long_long_to_string_init, output);
        fputs(" - ", output);
        fputs(address, output);
        fclose(output);

        g_most_recent_timestamp_of_tracking_file = timestamp;
        g_size_of_file++;
    }
    else {
    
        /* Double check that the MAC address is not already added at a given
        * timestamp */
        if (strstr(line, address) == NULL) {
    
            fputs(", ", output);
            fputs(address, output);
        }
        fclose(output);
    }

    /* Send to gateway every TIME_INTERVAL_OF_SEND_TO_GATEWAY minutes */
    if (TIME_INTERVAL_OF_SEND_TO_GATEWAY <=
        timestamp - g_initial_timestamp_of_tracking_file) {
        
        g_size_of_file = 0;
        g_most_recent_timestamp_of_tracking_file = 0;
        /* @todo: send to gateway function */
    
    }
}


/*
*  check_is_in_list:
*
*  This helper function checks whether the specified MAC address given as 
*  input is in the scanned list with recently scanned bluetooth devices. If 
*  it is, the function returns true, else the function returns false.
*
*  Parameters:
*
*  list - the list is goning to check 
*  address - scanned MAC address of bluetooth device
* 
*  Return value:
*
*  true - used MAC address
*  false - new MAC address
*/
bool check_is_in_list(List_Entry *list, char address[]) {
    
    /* Create a temporary node and set as the head */
    struct List_Entry *listptrs;
    Node *temp;

    /* Go through list */
    list_for_each(listptrs, list) {

        /* Input MAC address exists in the linked list */
        temp = ListEntry(listptrs, Node, ptrs);       
        int len = strlen(address);
        ScannedDevice *temp_data; 
        temp_data = (struct ScannedDevice *)temp->data;
        
        if (strcmp(address, &temp_data->scanned_mac_address[len + 10]) > 0) {
        
            return true;
        
        }
               
    }

    /* Input MAC address is new and unused */
    return false;
}



/*
 *  print_linked_list:
 *
 *  This function prints all the MAC addresses in the list. When printing, 
 *  the MAC addresses will be in the order of starting from the first node
 *  to the last in the list.
 *
 *  Parameters:
 *
 *  entry - the head of the list for determining which list is goning to be
 *  printed.
 *
 *  Return value:
 *
 *  None
 */
void print_list(List_Entry *entry) {

    /*Check whether the list is empty */
    if (get_list_length(entry) == 0 ) {
        return;
    }

    struct List_Entry *listptrs = NULL;
    struct Node *node;
    
    for (listptrs = (entry)->next; listptrs != (entry);
        listptrs = listptrs->next) {
        
        node = ListEntry(listptrs, Node, ptrs);
        ScannedDevice *data;
        data = (struct ScannedDevice *)node->data;
        printf("%s ", &data->scanned_mac_address[0]);
    
    }
    
    printf("\n");

}



/*
*  enable_advertising:
*
*  This function enables the LBeacon to start advertising, sets the time
*  interval for advertising, and calibrates the RSSI value.
*
*  Parameters:
*
*  advertising_interval - the time interval for which the LBeacon can 
*  advertise advertising_uuid - universally unique identifier for advertising
*  rssi_value - RSSI value of the bluetooth device
*
*  Return value:
*
*  1 - If there is an error, 1 is returned.
*  0 - If advertising was successfullly enabled, then the function returns 0.
*/
int enable_advertising(int advertising_interval, char *advertising_uuid,
    
    int rssi_value) {
    
    int dongle_device_id = hci_get_route(NULL);
    int device_handle = 0;
    
    if ((device_handle = hci_open_dev(dongle_device_id)) < 0) {
        /* Error handling */
        perror("Error opening device");
        return (1);
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
        return (1);
    
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
        return (1);
    
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
        return (1);
    }

    if (status) {
        /* Error handling */
        fprintf(stderr, "LE set advertise returned status %d\n", status);
        return (1);
    }
}


/*
*  disable_advertising:
*
*  This function disables the advertising capabilities of the beacon
*
*  Parameters:
*
*  None
*
*  Return value:
*
*  1 - If there is an error, 1 is returned.
*  0 - If advertising was successfullly disabled, 0 is returned.
*/
int disable_advertising() {
    
    int dongle_device_id = hci_get_route(NULL);
    int device_handle = 0;
    if ((device_handle = hci_open_dev(dongle_device_id)) < 0) {
        /* Error handling */
        perror("Could not open device");
        return (1);
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
        return (1);
    
    }

    if (status) {
        
        /* Error handling */
        fprintf(stderr, "LE set advertise enable on returned status %d\n",
            status);
        return (1);
    
    }
}

/*
*  ble_beacon:
*
*  This function allows avertising to be stopped with ctrl-c if
*  enable_advertising was a success.
*
*  Parameters:
*
*  beacon_location - advertising uuid
*
*  Return value:
*
*  None
*/
void *ble_beacon(void *beacon_location) {
    
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

        pthread_exit(NULL);
        return;
    }

}


/*
*  cleanup_scanned_list:
*
*  This function determines when scernned Device struct of each discovered
*  device remains in the seanned list for at most TIME_IN_SCANNED_LIST sec
*  scanned data of device in the scanned list. In the background, This work
*  thread continuously check the scanned list. If so, the ScannedDevice
*  struct will be removed.
*
*  Parameters:
*
*  None
*
*  Return value:
*
*  None
*/
void *cleanup_scanned_list(void) {
    

    while (ready_to_work == true) {
        
        struct List_Entry *listptrs;
        Node *temp;        
        /* Go through list */
        list_for_each(listptrs, scanned_list){
            
            temp = ListEntry(listptrs, Node, ptrs);
            ScannedDevice *temp_data;
            temp_data = (struct ScannedDevice *)temp->data;
           
            /* Device has been in the scanned list for at least 30 seconds */
            if (get_system_time() - temp_data->initial_scanned_time > TIMEOUT) {
                
                list_remove_node(&temp->ptrs);
                free(temp);
              
            }
            else {
                break;
            }
        }

    }
    
    /* Exiting this thread and sending message to main thread by using pthread
     * exit and join. */
    pthread_exit(NULL);
    return;

}


/*
*  queue_to_array:
*
*  This function continuously looks through the ThreadStatus array that
*  contains all the send_file thread status. When a thread becomes available
*  and the waiting list is not empty, the first MAC address in the waiting 
*  list will be added to the ThreadStatus array and removed from the waiting 
*  list.
*
*  Parameters:
*
*  None
*
*  Return value:
*
*  None
*/
void *queue_to_array() {
    
    /* Maximum number of devices to be handled by all push dongles */
    int maximum_number_of_devices = atoi(g_config.maximum_number_of_devices);

    /* An iterator through the array of ScannedDevice struct */
    int device_id;


    while (ready_to_work == true) {
        
        /* Go through the array of ThreadStatus */
        for (device_id = 0; device_id < maximum_number_of_devices;
            device_id++) {
            
            void *data;
            data = get_list_head(waiting_list);
            
            /* Check whether the return value from get_list_head is NULL */ 
            if(data == NULL){

                continue;
            
            }

            ScannedDevice *sc_data;
            sc_data = (struct ScannedDevice *) data;
            char *address = &sc_data->scanned_mac_address[0];
            
            /* Remove from waiting_list and add MAC address to the array when 
             * a thread becomes available */
            if (g_idle_handler[device_id].idle == true && address != NULL) {
                strncpy(g_idle_handler[device_id].scanned_mac_address, 
                        address,
                        LENGTH_OF_MAC_ADDRESS);

                struct Node *node = ListEntry(waiting_list->next, Node,
                                              ptrs);
                
                list_remove_node(waiting_list->next);
                free(node);
                g_idle_handler[device_id].idle = false;
                g_idle_handler[device_id].is_waiting_to_send = true;
            }
        }
    }
  
    
    /* Exiting this thread and sending message to main thread by using pthread
     * exit and join. */
    pthread_exit(NULL);
    return;

}


/*
*  send_file:
*
*  This function enables the caller to send the push message asynchronously 
*  using the specified thread. 
*  
*  [N.B. The beacon may still be scanning for other bluetooth devices.]
*
*  Parameters:
*
*  id - ID of the thread used to send the push message 
*
*  Return value:
*
*  None
*/
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


    while (send_message_cancelled = false) {

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
                    perror(errordesc[E_SEND_PUT_FILE].message);
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
        pthread_exit(NULL);
        return;
    }

}

/*
*  start_scanning:
*
*  This function scans continuously for bluetooth devices under the coverage
*  of the  beacon until there is a need to cancel scanning. Each scanned
*  device will fall under one of three cases: a bluetooth device with no RSSI
*  value and a bluetooth device with a RSSI value, When the device is within 
*  RSSI value, the bluetooth device will  be added to the linked list so a
*  message can be sent to the device.
*  
*  Parameters:
*
*  None
*
*  Return value:
*
*  None
*/
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
         perror(errordesc[E_SCAN_OPEN_SOCKET].message);
         ready_to_work = false;
         send_message_cancelled = true;
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
                    track_devices(&info->bdaddr, "output.txt");
                     
                }

            } break;    
                
            /* Scanned device with RSSI value; when within rangle, send
            * message to bluetooth device. */
            case EVT_INQUIRY_RESULT_WITH_RSSI: {
                 
                for (results_id = 0; results_id < results; results_id++) {  
                    info_rssi = (void *)event_buffer_pointer +
                         (sizeof(*info_rssi) * results_id) + 1;
                     
                     track_devices(&info_rssi->bdaddr, "output.txt");
                     print_RSSI_value(&info_rssi->bdaddr, 1,
                         info_rssi->rssi);
                     
                     if (info_rssi->rssi > RSSI_RANGE) {
                     
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


/*
*  startThread:
*
*  This function initializes the threads.
*
*  Parameters:
*
*  threads - name of the thread
*  run - the function for thread to do
*  arg - the argument for thread's function 
*
*  Return value:
*
*  None
*/
void startThread(pthread_t threads ,void * (*run)(void*), void *arg){

    pthread_attr_t attr;
    if (pthread_attr_init(&attr) != 0
      || pthread_create(&threads, &attr, run, arg) != 0
      || pthread_attr_destroy(&attr) != 0
      || pthread_detach(threads) != 0) {

    perror(strerror(errno));
    return;
  }

  return;

}

/*
*  clean_exit:
*
*  This function releases all the resources and set the flag.
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

    ready_to_work = false;
    send_message_cancelled = true;
    free_list(scanned_list);
    free_list(waiting_list);
    free(g_idle_handler);
    free(g_push_file_path);
    return;

}



int main(int argc, char **argv) {
    
    /* An iterator through the array of ScannedDevice struct */
    int device_id;

    /* Buffer that contains the location of the beacon */
    char hex_c[CONFIG_BUFFER_SIZE];

    /* Return value of pthread_create used to check for errors */
    int return_value;   

    /* Load config struct */
    g_config = get_config(CONFIG_FILE_NAME);
    g_push_file_path =
        malloc(g_config.file_path_length + g_config.file_name_length);
    
    if (g_push_file_path == NULL) {
        
        /* Error handling */
        perror(strerror(errno));
        cleanup_exit();
        return;

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
    g_idle_handler =
        malloc(maximum_number_of_devices * sizeof(ThreadStatus));
    if (g_idle_handler == NULL) {
        
        /* Error handling */
        perror(strerror(errno));
        cleanup_exit();
        return;

    }

    /* Initialize each ThreadStatus struct in the array */
    for (device_id = 0; device_id < maximum_number_of_devices; device_id++) {
         strncpy(g_idle_handler[device_id].scanned_mac_address, "0",
         LENGTH_OF_MAC_ADDRESS);
        g_idle_handler[device_id].idle = true;
        g_idle_handler[device_id].is_waiting_to_send = false;
    }

    /*Initialize two lists for the scanned data and waiting queue*/
    scanned_list = (struct List_Entry*)malloc(sizeof(struct List_Entry));
    scanned_list->next = scanned_list;
    scanned_list->prev = scanned_list;
    waiting_list = (struct List_Entry*)malloc(sizeof(struct List_Entry));
    waiting_list->next = waiting_list;
    waiting_list->prev = waiting_list;
    

    /* Store coordinates of the beacon location */
    sprintf(hex_c,
            "E2C56DB5DFFB48D2B060D0F5%02x%02x%02x%02x%02x%02x%02x%02x",
            coordinate_X.b[0], coordinate_X.b[1], coordinate_X.b[2],
            coordinate_X.b[3], coordinate_Y.b[0], coordinate_Y.b[1],
            coordinate_Y.b[2], coordinate_Y.b[3]);

  
    /* Create the thread for message advertising to BLE bluetooth devices */
    pthread_t ble_beacon_thread;
    
    startThread(ble_beacon_thread, ble_beacon, hex_c);
    
    

    /* Create the the cleanup_scanned_list thread */
    pthread_t cleanup_scanned_list_thread;
    
    startThread(cleanup_scanned_list_thread,cleanup_scanned_list, NULL);


  
    /* Create the thread for sending MAC address in waiting list to an 
     * available thread */
    pthread_t queue_to_array_thread;
    
    startThread(queue_to_array_thread, queue_to_array, NULL);



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

        startThread(send_file_thread[device_id], send_file, 
                    (void *)dongle_device_id);
      
    }


   
    while(ready_to_work == true){
        
        start_scanning();
    
    }

    /* ready_to_work = false , shut down. 
     * wait for send_file_thread to exit. */    
    
    for (device_id = 0; device_id < maximum_number_of_devices; device_id++) {
        
        return_value = pthread_join(send_file_thread[device_id], NULL);        
        
        if (return_value != 0) {
            perror(strerror(errno));
            cleanup_exit();
            return;
           
        }
    }
    

    return_value = pthread_join(queue_to_array_thread, NULL);
    
    if (return_value != 0) {
        perror(strerror(errno));
        cleanup_exit();
        return;
    }

    return_value = pthread_join(cleanup_scanned_list_thread, NULL);
     
    if (return_value != 0) {
        perror(strerror(errno));
        cleanup_exit();
        return;
        
    }
    

    pthread_cancel(ble_beacon_thread);
    return_value = pthread_join(ble_beacon_thread, NULL);
    
    if (return_value != 0) {
        perror(strerror(errno));
        cleanup_exit();
        return;
        
    }
       
    cleanup_exit();
        

    return 0;
}
