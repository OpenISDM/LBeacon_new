/*
 * Copyright (c) 2016 Academia Sinica, Institute of Information Science
 *
 * License:
 *
 *      GPL 3.0 : The content of this file is subject to the terms and
 *      conditions defined in file 'COPYING.txt', which is part of this source
 *      code package.
 *
 * Project Name:
 *
 *      BeDIPS
 *
 * File Description:
 *
 *      This file contains the program to allow the beacon to discover bluetooth
 *	devices and then scan the Bluetooth addresses of the devices. Depending
 *	on the RSSI value of each discovered and scanned deviced, the beacon
 * 	determines whether it should send location related files to the device.
 *
 * File Name:
 *
 *      LBeacon.c
 *
 * Abstract:
 *
 *      BeDIPS uses LBeacons to deliver 3D coordinates and textual
 *      descriptions of their locations to users' devices. Basically, a LBeacon
 *      is an inexpensive, Bluetooth Smart Ready device. The 3D coordinates and
 *      location description of every LBeacon are retrieved from BeDIS
 *      (Building/environment Data and Information System) and stored locally
 *      during deployment and maintenance times. Once initialized, each LBeacon
 *      broadcasts its coordinates and location description to Bluetooth
 *      enabled user devices within its coverage area.
 *
 * Authors:
 *
 *      Jake Lee, jakelee@iis.sinica.edu.tw
 *      Johnson Su, johnsonsu@iis.sinica.edu.tw
 *      Shirley Huang, shirley.huang.93@gmail.com
 *      Han Hu, hhu14@illinois.edu
 *      Jeffrey Lin, lin.jeff03@gmail.com
 *      Howard Hsu, haohsu0823@gmail.com
 */

#include "LBeacon.h"

/*
 *  get_config:
 *
 *  This function will go through the config file, read line by line until the
 *  end of file, and store the data into the global variable of a Config struct.
 *
 *  Parameters:
 *
 *  filename - the name of the config file that stores all the beacon data
 *
 *  Return value:
 *
 *  config - Config struct including filepath, coordinates, etc.
 */
Config get_config(char *filename) {
    /* Return value that contains a struct of all config information */
    Config config;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        /* Error handling */
        perror("Error opening file");
	fprintf(stderr, "Error: %s\n", strerror(errno));
    } else {
        /* Stores the string of the current line being read */
        char config_setting[CONFIG_BUFFER_SIZE];

        /* Keeps track of which line is being processed */
        int line = 0;

        /* Keep reading each line and store into the config struct */
        while (fgets(config_setting, sizeof(config_setting), file) != NULL) {
            char *config_message;
            config_message = strstr((char *)config_setting, DELIMITER);
            config_message = config_message + strlen(DELIMITER);
	    switch (line) {
            case 0:
                memcpy(config.coordinate_X, config_message,
                       strlen(config_message));
                config.coordinate_X_length = strlen(config_message);
		break;
            case 1:
                memcpy(config.coordinate_Y, config_message,
                       strlen(config_message));
                config.coordinate_Y_length = strlen(config_message);
		break;
            case 2:
                memcpy(config.coordinate_Z, config_message,
                       strlen(config_message));
                config.coordinate_Z_length = strlen(config_message);
		break;
            case 3:
                memcpy(config.filename, config_message, strlen(config_message));
                config.filename_length = strlen(config_message);
		break;
            case 4:
                memcpy(config.filepath, config_message, strlen(config_message));
                config.filepath_length = strlen(config_message);
		break;
            case 5:
                memcpy(config.maximum_number_of_devices, config_message,
                       strlen(config_message));
                config.maximum_number_of_devices_length =
                    strlen(config_message);
		break;
            case 6:
                memcpy(config.number_of_groups, config_message,
                       strlen(config_message));
                config.number_of_groups_length = strlen(config_message);
		break;
            case 7:
                memcpy(config.number_of_messages, config_message,
                       strlen(config_message));
                config.number_of_messages_length = strlen(config_message);
		break;
            case 8:
                memcpy(config.number_of_push_dongles, config_message,
                       strlen(config_message));
                config.number_of_push_dongles_length = strlen(config_message);
		break;
            case 9:
                memcpy(config.rssi_coverage, config_message,
                       strlen(config_message));
                config.rssi_coverage_length = strlen(config_message);
		break;
            case 10:
                memcpy(config.uuid, config_message, strlen(config_message));
                config.uuid_length = strlen(config_message);
		break;
            }
            line++;
        }
        fclose(file);
    }

    return config;
}

/*
 *  get_system_time:
 *
 *  This helper function fetches the current time according to the system clock
 *  in terms of the number of milliseconds since January 1, 1970.
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
 *  check_is_used_address:
 *
 *  This helper function checks whether the specified MAC address given as input
 *  is in the linked list with recently scanned bluetooth devices. If it is, the
 *  function returns true, else the function returns false.
 *
 *  Parameters:
 *
 *  address - scanned MAC address of bluetooth device
 *
 *  Return value:
 *
 *  true - used MAC address
 *  false - new MAC address
 */
bool check_is_used_address(char address[]) {
    /* Create a temporary node and set as the head */
    LinkedListNode *temp = linked_list_head;

    /* Go through list */
    while (temp != NULL) {
        /* Input MAC address exists in the linked list */
        if (0 == strcmp(address, temp->data.scanned_mac_address)) {
            return true;
        }
        temp = temp->next;
    }

    /* Input MAC address is new and unused */
    return false;
}

/*
 *  send_to_push_dongle:
 *
 *  For each new scanned bluetooth device, this function adds the scanned device
 *  to the linked list of ScannedDevice struct that stores its scanned timestamp
 *  and MAC address and to the queue of MAC addresses waiting for an available
 *  thread to send the message to its device.
 *
 *  Parameters:
 *
 *  bluetooth_device_address - bluetooth device address
 *  rssi - RSSI value of bluetooth device
 *
 *  Return value:
 *
 *  None
 */
void send_to_push_dongle(bdaddr_t *bluetooth_device_address, int rssi) {
    /* Stores the MAC address as a string */
    char address[LENGTH_OF_MAC_ADDRESS];

    /* An iterator through each MAC address character */
    int mac_address_iterator;

    /* Converts the bluetooth device address to a string */
    ba2str(bluetooth_device_address, address);

    /* Add to the linked list and the queue for new scanned devices */
    if (check_is_used_address(address) == false) {
        ScannedDevice data;
        data.initial_scanned_time = get_system_time();
        for (mac_address_iterator = 0;
             mac_address_iterator < LENGTH_OF_MAC_ADDRESS;
             mac_address_iterator++) {
            data.scanned_mac_address[mac_address_iterator] =
                address[mac_address_iterator];
        }
        insert_first(data);
        enqueue(address);
        print_linked_list();
        print_queue();
    }
}

/*
 *  queue_to_array:
 *
 *  This function will continuously look through the ThreadStatus array that
 *  contains all the send_file thread statuses. Once a thread becomes available
 *  and the queue is not empty, the first MAC address in the queue will be
 *  added to the ThreadStatus array and removed from the queue.
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

    /* An iterator through each MAC address character */
    int mac_address_iterator;

    /* An indicator for continuing to check for unused threads */
    cancelled = false;

    while (cancelled == false) {
        /* Go through the array of ThreadStatus */
        for (device_id = 0; device_id < maximum_number_of_devices;
             device_id++) {
            char *address = peek();
            /* Add MAC address to the array and dequeue when a thread becomes
             * available */
            if (g_idle_handler[device_id].idle == -1 && address != NULL) {
                for (mac_address_iterator = 0;
                     mac_address_iterator < LENGTH_OF_MAC_ADDRESS;
                     mac_address_iterator++) {
                    g_idle_handler[device_id]
                        .scanned_mac_address[mac_address_iterator] =
                        address[mac_address_iterator];
                }
                dequeue();
                g_idle_handler[device_id].idle = device_id;
                g_idle_handler[device_id].is_waiting_to_send = true;
            }
        }
    }
}

/*
 *  send_file:
 *
 *  This function enables the caller to send the push message to the specified
 *  bluetooth device asynchronously.
 *
 *  [N.B. The beacon may still be scanning for other bluetooth devices for which
 *  the message is being pushed to.]
 *
 *  Parameters:
 *
 *  id - thread ID for each send_file thread
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
    char *filename;                  /* Filename of message to be sent */
    int return_value;                /* Return value for error handling */

    int number_of_push_dongles = atoi(g_config.number_of_push_dongles);
    int maximum_number_of_devices = atoi(g_config.maximum_number_of_devices);
    int maximum_number_of_devices_per_dongle =
        maximum_number_of_devices / number_of_push_dongles;

    /* An iterator through the array of ScannedDevice struct */
    int device_id;

    /* An iterator through each push dongle */
    int push_dongle_id;

    /* An iterator through a block of devices per dongle */
    int block_id;

    /* An iterator through each MAC address character */
    int mac_address_iterator;

    /* An indicator for continuing to send messages */
    cancelled = false;

    while (cancelled == false) {
        for (device_id = 0; device_id < maximum_number_of_devices;
             device_id++) {
            if (device_id == thread_id &&
                g_idle_handler[device_id].is_waiting_to_send == true) {
                /* Depending on the number of push dongles, split the threads
                 * evenly and assign each thread to a push dongle device ID */
                for (push_dongle_id = 0;
                     push_dongle_id < number_of_push_dongles;
                     push_dongle_id++) {
                    for (block_id = 0;
                         block_id < maximum_number_of_devices_per_dongle;
                         block_id++) {
                        if (thread_id ==
                            push_dongle_id *
                                    maximum_number_of_devices_per_dongle +
                                block_id) {
                            dongle_device_id = push_dongle_id + 1;
                        }
                    }
                }

                /* Open socket and use current time as start time to determine
                 * how long it takes to send the message to the device */
                socket = hci_open_dev(dongle_device_id);
                if (0 > dongle_device_id || 0 > socket) {
                    /* Error handling */
                    perror("Error opening socket");
                    for (mac_address_iterator = 0;
                         mac_address_iterator < LENGTH_OF_MAC_ADDRESS;
                         mac_address_iterator++) {
                        g_idle_handler[device_id]
                            .scanned_mac_address[mac_address_iterator] = 0;
                    }
                    g_idle_handler[device_id].idle = -1;
                    g_idle_handler[device_id].is_waiting_to_send = false;
                    break;
                }
                long long start = get_system_time();
                address = (char *)g_idle_handler[device_id].scanned_mac_address;
                channel = obexftp_browse_bt_push(address);

                /* Extract basename from filepath */
                filename = strrchr(g_filepath, '/');
                filename[g_config.filename_length] = '\0';
                if (!filename) {
                    filename = g_filepath;
                } else {
                    filename++;
                }
                printf("Sending file %s to %s\n", filename, address);

                /* Open connection */
                client = obexftp_open(OBEX_TRANS_BLUETOOTH, NULL, NULL, NULL);
                long long end = get_system_time();
                printf("Time to open connection: %lld ms\n", end - start);
                if (client == NULL) {
                    /* Error handling */
                    perror("Error opening obexftp client");
                    for (mac_address_iterator = 0;
                         mac_address_iterator < LENGTH_OF_MAC_ADDRESS;
                         mac_address_iterator++) {
                        g_idle_handler[device_id]
                            .scanned_mac_address[mac_address_iterator] = 0;
                    }
                    g_idle_handler[device_id].idle = -1;
                    g_idle_handler[device_id].is_waiting_to_send = false;
                    close(socket);
                    break;
                }

                /* Connect to the scanned device */
                return_value = obexftp_connect_push(client, address, channel);

                /* If obexftp_connect_push returns a negative integer, then it
                 * goes into error handling */
                if (0 > return_value) {
                    /* Error handling */
                    perror("Error connecting to obexftp device");
                    obexftp_close(client);
                    client = NULL;
                    for (mac_address_iterator = 0;
                         mac_address_iterator < LENGTH_OF_MAC_ADDRESS;
                         mac_address_iterator++) {
                        g_idle_handler[device_id]
                            .scanned_mac_address[mac_address_iterator] = 0;
                    }
                    g_idle_handler[device_id].idle = -1;
                    g_idle_handler[device_id].is_waiting_to_send = false;
                    close(socket);
                    break;
                }

                /* Push file to the scanned device */
                return_value = obexftp_put_file(client, g_filepath, filename);
                if (0 > return_value) {
                    /* Error handling */
                    perror("Error putting file");
                }

                /* Disconnect connection */
                return_value = obexftp_disconnect(client);
                if (0 > return_value) {
                    /* Error handling */
                    perror("Error disconnecting the client");
                }

                /* Close socket */
                obexftp_close(client);
                client = NULL;
                for (mac_address_iterator = 0;
                     mac_address_iterator < LENGTH_OF_MAC_ADDRESS;
                     mac_address_iterator++) {
                    g_idle_handler[device_id]
                        .scanned_mac_address[mac_address_iterator] = 0;
                }
                g_idle_handler[device_id].idle = -1;
                g_idle_handler[device_id].is_waiting_to_send = false;
                close(socket);
            }
        }
    }
}

/*
 *  print_RSSI_value:
 *
 *  This function prints the RSSI value along with the MAC address of the user's
 *  scanned bluetooth device. When the LBeacon is running, we will continuously
 *  see a list of scanned bluetooth devices running in the console.
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

    /* Print bluetooth device's RSSI value */
    printf("%17s", address);
    if (has_rssi) {
        printf(" RSSI:%d", rssi);
    } else {
        printf(" RSSI:n/a");
    }
    printf("\n");
    fflush(NULL);
}

/*
 *  start_scanning:
 *
 *  This function scans continuously for bluetooth devices under the beacon
 *  until there is a need to cancel scanning. For each scanned device, it will
 *  fall under one of three cases: a bluetooth device with no RSSI value, a
 *  bluetooth device with a RSSI value, or if the user wants to cancel scanning.
 *  When the device is within RSSI value, the bluetooth device will be added to
 *  the linked list so a message can be sent to the device.
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
    struct hci_filter filter;
    struct pollfd output;
    unsigned char event_buffer[HCI_MAX_EVENT_SIZE];
    unsigned char *event_buffer_pointer;
    hci_event_hdr *event_handler;
    inquiry_cp inquiry_copy;
    inquiry_info_with_rssi *info_rssi;
    inquiry_info *info;
    int event_buffer_length;
    int dongle_device_id = 0;
    int socket = 0;
    int results;
    int results_id;

    /* Open Bluetooth device */
    socket = hci_open_dev(dongle_device_id);
    if (0 > dongle_device_id || 0 > socket) {
        /* Error handling */
        perror("Error opening socket");
        return;
    }

    /* Setup filter */
    hci_filter_clear(&filter);
    hci_filter_set_ptype(HCI_EVENT_PKT, &filter);
    hci_filter_set_event(EVT_INQUIRY_RESULT, &filter);
    hci_filter_set_event(EVT_INQUIRY_RESULT_WITH_RSSI, &filter);
    hci_filter_set_event(EVT_INQUIRY_COMPLETE, &filter);
    if (0 > setsockopt(socket, SOL_HCI, HCI_FILTER, &filter, sizeof(filter))) {
        /* Error handling */
        perror("Error setting HCI filter");
        hci_close_dev(socket);
        return;
    }
    hci_write_inquiry_mode(socket, 0x01, 10);
    if (0 > hci_send_cmd(socket, OGF_HOST_CTL, OCF_WRITE_INQUIRY_MODE,
                         WRITE_INQUIRY_MODE_RP_SIZE, &inquiry_copy)) {
        /* Error handling */
        perror("Error setting inquiry mode");
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
        perror("Error starting inquiry");
        return;
    }

    output.fd = socket;
    output.events = POLLIN | POLLERR | POLLHUP;

    /* An indicator for continuing to scan for devices */
    cancelled = false;

    while (cancelled == false) {
        output.revents = 0;

        /* Poll the bluetooth device for an event */
        if (0 < poll(&output, 1, -1)) {
            event_buffer_length =
                read(socket, event_buffer, sizeof(event_buffer));

            if (0 > event_buffer_length) {
                continue;
            } else if (0 == event_buffer_length) {
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
                            send_to_push_dongle(&info_rssi->bdaddr,
                                                info_rssi->rssi);
                        }
                    }
                } break;

                /* Stop the scanning process */
                case EVT_INQUIRY_COMPLETE: {
                    cancelled = true;
                } break;

                default:
                    break;
            }
        }
    }
    printf("Scanning done\n");
    close(socket);
}

/*
 *  cleanup_linked_list:
 *
 *  This function determines when the bluetooth device's scanned data will be
 *  removed from the linked list. In the background, this working thread will
 *  continuously check if it has been 30 seconds since the bluetooth was added
 *  to the linked list. If so, the ScannedDevice struct will be removed.
 *
 *  Parameters:
 *
 *  None
 *
 *  Return value:
 *
 *  None
 */
void *cleanup_linked_list(void) {
    /* An indicator for continuing to clean the linked list */
    cancelled = false;

    while (cancelled == false) {
        /* Create a temporary node and set as the head */
        LinkedListNode *temp = linked_list_head;

        /* Go through list */
        while (temp != NULL) {
            /* Device has been in the linked list for at least 30 seconds */
            if (get_system_time() - temp->data.initial_scanned_time > TIMEOUT) {
                printf("Removed %s from linked list\n",
                       &temp->data.scanned_mac_address[0]);
                delete_node(temp->data);
            }
            temp = temp->next;
        }
    }
}

/*
 *  track_devices:
 *
 *  This function tracks the MAC addresses of scanned bluetooth devices under
 *  the beacon. An output file will contain each timestamp and the MAC addresses
 *  of the scanned bluetooth devices at the given timestamp. Format timestamp
 *  and MAC addresses into a string and append new line to end of file. " - " is
 *  used to separate timestamp with MAC address and ", " is used to separate
 *  each MAC address.
 *
 *  Parameters:
 *
 *  bluetooth_device_address - bluetooth device address
 *  filename - name of the file where all the data will be stored
 *
 *  Return value:
 *
 *  None
 */
void track_devices(bdaddr_t *bluetooth_device_address, char *filename) {
    /* Scanned MAC address */
    char address[LENGTH_OF_MAC_ADDRESS];

    /* Converts long long type to a string */
    char long_long_to_string[LENGTH_OF_TIME];

    /* Get current timestamp when tracking bluetooth devices */
    unsigned timestamp = (unsigned)time(NULL);
    sprintf(long_long_to_string, "%u", timestamp);

    /* If file is empty, create new file with LBeacon UUID */
    if (0 == g_size_of_file) {
        FILE *output = fopen(filename, "w+"); /* w+ overwrites the file */
        if (output == NULL) {
            /* Error handling */
            perror("Error opening file");
            return;
        }
        fputs("LBeacon UUID: ", output);
        fputs(g_config.uuid, output);
        fclose(output);
        g_size_of_file++;
        g_initial_timestamp_of_file = timestamp;
        memset(&address[0], 0, sizeof(address));
    }

    /* Converts the bluetooth device address to a string */
    ba2str(bluetooth_device_address, address);

    FILE *output;
    char line[TRACKING_BUFFER];
    output = fopen(filename, "a+"); /* a+ appends to the file */

    if (output == NULL) {
        /* Error handling */
        perror("Error opening file");
        return;
    }

    /* Go through the whole file to get to the last line */
    while (fgets(line, TRACKING_BUFFER, output) != NULL) {
    }

    /* If timestamp already exists, add MAC address to end of the previous line
     * otherwise create a new line */
    if (timestamp != g_most_recent_timestamp_of_file) {
        fputs("\n", output);
        fputs(long_long_to_string, output);
        fputs(" - ", output);
        fputs(address, output);
        fclose(output);

        g_most_recent_timestamp_of_file = timestamp;
        g_size_of_file++;
    } else {
        /* Double check that the MAC address is not already added at a given
         * timestamp */
        if (strstr(line, address) == NULL) {
            fputs(", ", output);
            fputs(address, output);
        }
        fclose(output);
    }

    /* Send to gateway every 5 minutes */
    if (300 <= timestamp - g_initial_timestamp_of_file) {
        g_size_of_file = 0;
        g_most_recent_timestamp_of_file = 0;
        /* @todo: send to gateway function */
    }
}

/*
 *  choose_file:
 *
 *  This function receives the name of the message file and returns the filepath
 *  where the message is located. It goes through each directory in the messages
 *  folder and in each category, it reads each filename to find the designated
 *  message we want to broadcast to the users under the beacon.
 *
 *  Parameters:
 *
 *  message_to_send - name of the message file we want to retrieve
 *
 *  Return value:
 *
 *  return_value - message filepath
 */
char *choose_file(char *message_to_send) {
    DIR *groupdir;           /* A dirent that stores list of directories */
    struct dirent *groupent; /* A dirent struct that stores directory info */
    int message_id = 0;      /* A iterator for number of messages and groups */
    int group_id = 0;        /* A iterator for number of groups */
    char *return_value;      /* Return value which turns filepath to a string */

    /* Convert number of groups and messages from a string to an integer */
    int number_of_groups = atoi(g_config.number_of_groups);
    int number_of_messages = atoi(g_config.number_of_messages);

    /* An array of buffer for group file names */
    char groups[number_of_groups][FILENAME_BUFFER];

    /* An array of buffer for message file names */
    char messages[number_of_messages][FILENAME_BUFFER];

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
    } else {
        /* Error handling */
        perror("Directories do not exist");
        return NULL;
    }

    /* Stores filepath of message_to_send */
    char filepath[FILENAME_BUFFER];
    memset(filepath, 0, FILENAME_BUFFER);
    message_id = 0;

    /* Go through each message in directory and store each file name */
    for (group_id = 0; group_id < number_of_groups; group_id++) {
        /* Concatenate strings to make filepath */
        sprintf(filepath, "/home/pi/LBeacon/messages/");
        strcat(filepath, groups[group_id]);

        DIR *messagedir;
        struct dirent *messageent;
        messagedir = opendir(filepath);
        if (messagedir) {
            while ((messageent = readdir(messagedir)) != NULL) {
                if (strcmp(messageent->d_name, ".") != 0 &&
                    strcmp(messageent->d_name, "..") != 0) {
                    strcpy(messages[message_id], messageent->d_name);
                    /* If message name found, return filepath */
                    if (0 == strcmp(messages[message_id], message_to_send)) {
                        strcat(filepath, "/");
                        strcat(filepath, messages[message_id]);
                        return_value = &filepath[0];
                        return return_value;
                    }
                    message_id++;
                }
            }
            closedir(messagedir);
        } else {
            /* Error handling */
            perror("Message files do not exist");
            return NULL;
        }
    }

    /* Error handling */
    perror("Message files do not exist");
    return NULL;
}

/*
 *  pthread_create_error_message:
 *
 *  This function receives the return value of pthread_create when there is an
 *  error and it prints the specific error description.
 *
 *  Parameters:
 *
 *  error_code - return value of the pthread_create function
 *
 *  Return value:
 *
 *  None
 */
void pthread_create_error_message(int error_code) {
    if (error_code == 1) {
        perror("[EPERM] Operation not permitted");
    } else if (error_code == 11) {
        perror("[EAGAIN] Resource temporarily unavailable");
    } else if (error_code == 22) {
        perror("[EINAL] Invalid argument");
    }
}

/*
 *  enable_advertising:
 *
 *  This function enables the LBeacon to start advertising, sets the time
 *  interval for advertising, and calibrates the RSSI value.
 *
 *  Parameters:
 *
 *  advertising_interval - the time interval for whic the LBeacon can advertise
 *  advertising_uuid - universally unique identifier for advertising
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

    int return_value = hci_send_req(device_handle, &request, 1000);
    if (return_value < 0) {
        /* Error handling */
        hci_close_dev(device_handle);
        fprintf(stderr, "Can't send request %s (%d)\n", strerror(errno), errno);
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

    return_value = hci_send_req(device_handle, &request, 1000);

    if (return_value < 0) {
        /* Error handling */
        hci_close_dev(device_handle);
        fprintf(stderr, "Can't send request %s (%d)\n", strerror(errno), errno);
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

    return_value = hci_send_req(device_handle, &request, 1000);

    hci_close_dev(device_handle);

    if (return_value < 0) {
        /* Error handling */
        fprintf(stderr, "Can't send request %s (%d)\n", strerror(errno), errno);
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

    int return_value = hci_send_req(device_handle, &request, 1000);

    hci_close_dev(device_handle);

    if (return_value < 0) {
        /* Error handling */
        fprintf(stderr, "Can't set advertise mode: %s (%d)\n", strerror(errno),
                errno);
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
 *  @todo
 *
 *  Parameters:
 *
 *  beacon_location - @todo
 *
 *  Return value:
 *
 *  None
 */
void *ble_beacon(void *beacon_location) {
    int enable_advertising_success =
        enable_advertising(300, beacon_location, 20);

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
}

int main(int argc, char **argv) {
    /* An iterator through the array of ScannedDevice struct */
    int device_id;

    /* An iterator through each MAC address character */
    int mac_address_iterator;

    /* Buffer that contains the location of the beacon */
    char hex_c[CONFIG_BUFFER_SIZE];

    /* Return value of pthread_create used to check for errors */
    int return_value;

    /* Load config struct */
    g_config = get_config(CONFIG_FILENAME);
    g_filepath = malloc(g_config.filepath_length + g_config.filename_length);
    if (g_filepath == NULL) {
        /* Error handling */
        perror("Failed to allocate memory");
        return -1;
    }
    memcpy(g_filepath, g_config.filepath, g_config.filepath_length - 1);
    memcpy(g_filepath + g_config.filepath_length - 1, g_config.filename,
           g_config.filename_length - 1);
    coordinate_X.f = (float)atof(g_config.coordinate_X);
    coordinate_Y.f = (float)atof(g_config.coordinate_Y);
    coordinate_Z.f = (float)atof(g_config.coordinate_Z);

    /* Allocate an array with the size of maximum number of devices */
    int maximum_number_of_devices = atoi(g_config.maximum_number_of_devices);
    g_idle_handler = malloc(maximum_number_of_devices * sizeof(ThreadStatus));
    if (g_idle_handler == NULL) {
        /* Error handling */
        perror("Failed to allocate memory");
        return -1;
    }

    /* Initialize each ThreadStatus struct in the array */
    for (device_id = 0; device_id < maximum_number_of_devices; device_id++) {
        for (mac_address_iterator = 0;
             mac_address_iterator < LENGTH_OF_MAC_ADDRESS;
             mac_address_iterator++) {
            g_idle_handler[device_id]
                .scanned_mac_address[mac_address_iterator] = 0;
        }
        g_idle_handler[device_id].idle = -1;
        g_idle_handler[device_id].is_waiting_to_send = false;
    }

    /* Store coordinates of the beacon location */
    sprintf(hex_c, "E2C56DB5DFFB48D2B060D0F5%02x%02x%02x%02x%02x%02x%02x%02x",
            coordinate_X.b[0], coordinate_X.b[1], coordinate_X.b[2],
            coordinate_X.b[3], coordinate_Y.b[0], coordinate_Y.b[1],
            coordinate_Y.b[2], coordinate_Y.b[3]);

    /* Enable message advertising to BLE bluetooth devices */
    pthread_t ble_beacon_id;
    return_value =
        pthread_create(&ble_beacon_id, NULL, (void *)ble_beacon, hex_c);
    if (return_value != 0) {
        /* Error handling */
        perror("Error with ble_beacon using pthread_create");
        pthread_create_error_message(return_value);
        pthread_exit(NULL);
    }

    /* Clean up the linked list */
    pthread_t cleanup_linked_list_id;
    return_value = pthread_create(&cleanup_linked_list_id, NULL,
                                  (void *)cleanup_linked_list, NULL);
    if (return_value != 0) {
        /* Error handling */
        perror("Error with cleanup_linked_list using pthread_create");
        pthread_create_error_message(return_value);
        pthread_exit(NULL);
    }

    /* Send MAC address in queue to an available thread */
    pthread_t queue_to_array_id;
    return_value =
        pthread_create(&queue_to_array_id, NULL, (void *)queue_to_array, NULL);
    if (return_value != 0) {
        perror("Error with queue_to_array using pthread_create");
        pthread_create_error_message(return_value);
        pthread_exit(NULL);
    }

    /* Send message to the scanned MAC address */
    pthread_t send_file_id[maximum_number_of_devices];
    for (device_id = 0; device_id < maximum_number_of_devices; device_id++) {
        return_value = pthread_create(&send_file_id[device_id], NULL,
                                      (void *)send_file, (void *)device_id);
        if (return_value != 0) {
            perror("Error with send_file using pthread_create");
            pthread_create_error_message(return_value);
            pthread_exit(NULL);
        }
    }

    /* An indicator for continuing to run the beacon */
    cancelled = false;

    /* Start scanning for bluetooth devices */
    while (cancelled == false) {
        start_scanning();
    }

    free(g_filepath);
    return 0;
}
