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
*      This is the header file containing the function declarations and
*      variables used in the LBeacon.c file.
*
* File Name:
*
*      LBeacon.h
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
*      Han Wang, hollywang@iis.sinica.edu.tw
*/

/*
* INCLUDES
*/

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <obexftp/client.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "LinkedList.h"
#include "Utilities.h"


/*
* CONSTANTS
*/

/* Command opcode pack/unpack from HCI library */
#define cmd_opcode_pack(ogf, ocf) (uint16_t)((ocf & 0x03ff) | (ogf << 10))

/* Maximum number of characters in each line of config file */
#define CONFIG_BUFFER_SIZE 64

/* Filepath of the config file */
#define CONFIG_FILENAME "../config/config.conf"

/* Parameter that determines the start of the config file */
#define DELIMITER "="

/* BlueZ bluetooth extended inquiry response protocol: flags */
#define EIR_FLAGS 0X01

/* BlueZ bluetooth extended inquiry response protocol: Manufacturer Specific
* Data */
#define EIR_MANUFACTURE_SPECIFIC_DATA 0xFF

/* BlueZ bluetooth extended inquiry response protocol: complete local name */
#define EIR_NAME_COMPLETE 0x09

/* BlueZ bluetooth extended inquiry response protocol: shorten local name */
#define EIR_NAME_SHORT 0x08

/* Maximum number of characters in message filenames */
#define FILENAME_BUFFER 256

/* Length of time in Epoch */
#define LENGTH_OF_TIME 10

/* Transmission range limited. Only devices in this RSSI range are allowed to
* connect */
#define RSSI_RANGE -60

/* Time interval, in milliseconds, that determines whether the bluetooth device
* is to be removed from the push list */
#define TIMEOUT 30000

/* Maximum number of characters in each line of output file used for tracking
* scanned devices */
#define TRACKING_BUFFER 1024



/*
* GLOBAL VARIABLES
*/

/* The path of the object push file */
char *g_filepath;

/* The first timestamp of the output file used for tracking scanned devices */
unsigned g_initial_timestamp_of_file = 0;

/* The most recent time of the output file used for tracking scanned devices */
unsigned g_most_recent_timestamp_of_file = 0;

/* Number of lines in the output file used for tracking scanned devices */
int g_size_of_file = 0;

/*
* UNION
*/

/* This union will convert floats into Hex code used for the beacon location */
union {
    float f;
    unsigned char b[sizeof(float)];
    int d[2];
} coordinate_X;

union {
    float f;
    unsigned char b[sizeof(float)];
} coordinate_Y;

union {
    float f;
    unsigned char b[sizeof(float)];
} coordinate_Z;

/*
* TYPEDEF STRUCTS
*/

typedef struct Config {
    /* A string representation of the X coordinate of the beacon location */
    char coordinate_X[CONFIG_BUFFER_SIZE];

    /* A string representation of the Y coordinate of the beacon location */
    char coordinate_Y[CONFIG_BUFFER_SIZE];

    /* A string representation of the Z coordinate of the beacon location */
    char coordinate_Z[CONFIG_BUFFER_SIZE];

    /* A string representation of the message filename */
    char filename[CONFIG_BUFFER_SIZE];

    /* A string representation of the message filename's filepath */
    char filepath[CONFIG_BUFFER_SIZE];

    /* A string representation of the maximum number of devices to be handled by
    * all push dongles */
    char maximum_number_of_devices[CONFIG_BUFFER_SIZE];

    /* A string representation of number of message groups */
    char number_of_groups[CONFIG_BUFFER_SIZE];

    /* A string representation of the number of messages */
    char number_of_messages[CONFIG_BUFFER_SIZE];

    /* A string representation of the number of push dongles */
    char number_of_push_dongles[CONFIG_BUFFER_SIZE];

    /* A string representation of the required signal strength */
    char rssi_coverage[CONFIG_BUFFER_SIZE];

    /* A string representation of the universally unique identifer */
    char uuid[CONFIG_BUFFER_SIZE];

    /* The string length needed to store coordinate_X */
    int coordinate_X_length;

    /* The string length needed to store coordinate_Y */
    int coordinate_Y_length;

    /* The string length needed to store coordinate_Z */
    int coordinate_Z_length;

    /* The string length needed to store filename */
    int filename_length;

    /* The string length needed to store filepath */
    int filepath_length;

    /* The string length needed to store maximum_number_of_devices */
    int maximum_number_of_devices_length;

    /* The string length needed to store number_of_groups */
    int number_of_groups_length;

    /* The string length needed to store number_of_messages */
    int number_of_messages_length;

    /* The string length needed to store number_of_push_dongles */
    int number_of_push_dongles_length;

    /* The string length needed to store rssi_coverage */
    int rssi_coverage_length;

    /* The string length needed to store uuid */
    int uuid_length;
} Config;

/* Struct for storing config information from the input file */
Config g_config;

typedef struct ThreadStatus {
    char scanned_mac_address[LENGTH_OF_MAC_ADDRESS];
    int idle;
    bool is_waiting_to_send;
} ThreadStatus;



/* An array of struct for storing information and status of each thread */
ThreadStatus *g_idle_handler;


/*Two list of struct for recording scanned devices */
List_Entry *scanned_list;
List_Entry *waiting_list;


/*
* FUNCTIONS
*/

Config get_config(char *filename);
long long get_system_time();
bool check_is_used_address(char address[]);
void send_to_push_dongle(bdaddr_t *bluetooth_device_address);
void *queue_to_array();
void *send_file(void *id);
void print_RSSI_value(bdaddr_t *bluetooth_device_address, bool has_rssi,
    int rssi);
void start_scanning();
void *cleanup_scanned_list(void);
void track_devices(bdaddr_t *bluetooth_device_address, char *filename);
char *choose_file(char *message_to_send);
int enable_advertising(int advertising_interval, char *advertising_uuid,
    int rssi_value);
int disable_advertising();
void *ble_beacon(void *beacon_location);
void startThread(void * (*run)(void*), void *arg);