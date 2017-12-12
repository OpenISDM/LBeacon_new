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

/* File path of the config file */
#define CONFIG_FILE_NAME "../config/config.conf"

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

/* Maximum number of characters in message file names */
#define FILE_NAME_BUFFER 256

/* Length of time in Epoch */
#define LENGTH_OF_TIME 10

/* Transmission range limited. Only devices in this RSSI range are allowed
 * to connect */
#define RSSI_RANGE -60

/* Time interval,maximum length of time in milliseconds, a bluetooth device
* stays in the push list */
#define TIMEOUT 30000

/* Maximum number of characters in each line of output file used for tracking
 * scanned devices */
#define TRACKING_FILE_LINE_LENGTH 1024

/* Length of a Bluetooth MAC address */
#define LENGTH_OF_MAC_ADDRESS 18

/* Timeout of hci_send_req  */
#define HCI_SEND_REQUEST_TIMEOUT 1000

/* Time interval in seconds for Send to gateway */
#define TIME_INTERVAL_OF_SEND_TO_GATEWAY 300

/* Time interval for which the LBeacon can */
#define ADVERTISING_INTERVAL 300

/* RSSI value of the bluetooth device */
#define RSSI_VALUE 20



/*
* UNION
*/

/* This union will convert floats into Hex code used for the beacon
* location */
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

    /* A string representation of the message file name */
    char file_name[CONFIG_BUFFER_SIZE];

    /* A string representation of the message file name's file path */
    char file_path[CONFIG_BUFFER_SIZE];

    /* A string representation of the maximum number of devices to be
    handled by all push dongles */
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

    /* The string length needed to store file name */
    int file_name_length;

    /* The string length needed to store file path */
    int file_path_length;

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


typedef struct ThreadStatus {
    char scanned_mac_address[LENGTH_OF_MAC_ADDRESS];
    bool idle;
    bool is_waiting_to_send;
} ThreadStatus;


/* Struct for storing scanned timestamp and MAC address of the user's
*  device */
typedef struct ScannedDevice {
    long long initial_scanned_time;
    char scanned_mac_address[LENGTH_OF_MAC_ADDRESS];
} ScannedDevice;



/*
* ERROR CODE
*/

enum Error_code {

    E_OPEN_FILE = 0,
    E_SEND_OPEN_SOCKET = 1,
    E_SEND_OBEXFTP_CLIENT = 2,
    E_SEND_CONNECT_DEVICE = 3,
    E_SEND_PUT_FILE = 4,
    E_SEND_DISCONNECT_CLIENT = 5,
    E_SCAN_OPEN_SOCKET = 6,
    E_SCAN_SET_HCI_FILTER = 7,
    E_SCAN_SET_INQUIRY_MODE = 8,
    E_SCAN_START_INQUIRY = 9
  
};

typedef enum Error_code error_t;

struct _errordesc {
    int code;
    char *message;
}errordesc[] = {

    {E_OPEN_FILE, "Error with opening file"},
    {E_SEND_OPEN_SOCKET, "Error with opening socket"},
    {E_SEND_OBEXFTP_CLIENT, "Error opening obexftp client"},
    {E_SEND_CONNECT_DEVICE, "Error connecting to obexftp device"},
    {E_SEND_PUT_FILE, "Error with putting file"},
    {E_SEND_DISCONNECT_CLIENT, "Disconnecting the client"},
    {E_SCAN_OPEN_SOCKET, "Error with opening socket"},
    {E_SCAN_SET_HCI_FILTER, "Error with setting HCI filter"},
    {E_SCAN_SET_INQUIRY_MODE, "Error with settnig inquiry mode"},
    {E_SCAN_START_INQUIRY, "Error with starting inquiry"},

};



/*
 * EXTERN STRUCTS
 */

/*In sys/poll.h, the struct for controlling the events.*/
extern struct pollfd;

/*In hci_sock.h, the struct for callback event from the socket.*/
extern struct hci_filter;




/*
* GLOBAL VARIABLES
*/

/* The path of the object push file */
char *g_push_file_path;

/* The first timestamp of the output file used for tracking scanned
* devices */
unsigned g_initial_timestamp_of_tracking_file = 0;

/* The most recent time of the output file used for tracking scanned
* devices */
unsigned g_most_recent_timestamp_of_tracking_file = 0;

/* Number of lines in the output file used for tracking scanned devices */
int g_size_of_file = 0;

/* Struct for storing config information from the input file */
Config g_config;


/* An array of struct for storing information and status of each thread */
ThreadStatus *g_idle_handler;

/*Two list of struct for recording scanned devices */
List_Entry *scanned_list;
List_Entry *waiting_list;

/* Two global flags for threads */
bool ready_to_work = true;
bool send_message_cancelled = false;



/*
* FUNCTIONS
*/

Config get_config(char *file_name);
long long get_system_time();
bool check_is_in_list(List_Entry *list, char address[]);
void print_list(List_Entry *entry);
char *get_head_entry(List_Entry *entry);
void free_list(List_Entry *entry);
void send_to_push_dongle(bdaddr_t *bluetooth_device_address);
void *queue_to_array();
void *send_file(void *dongle_id);
void print_RSSI_value(bdaddr_t *bluetooth_device_address, bool has_rssi,
    int rssi);
void start_scanning();
void *cleanup_scanned_list(void);
void track_devices(bdaddr_t *bluetooth_device_address, char *file_name);
char *choose_file(char *message_to_send);
int enable_advertising(int advertising_interval, char *advertising_uuid,
    int rssi_value);
int disable_advertising();
void *ble_beacon(void *beacon_location);
void startThread(pthread_t threads, void * (*run)(void*), void *arg);
void cleanup_exit();


/*
* EXTERNAL FUNCTIONS
*/

/* The function calls the function of list_insert_ to add a new node at the
* first of the list.*/
extern void list_insert_head(List_Entry *new_node, List_Entry *head);

/* The function calls the function of remove_node__ to delete a node in the
* list.*/
extern void list_remove_node(List_Entry *removed_node_ptrs);

/* The function returns the length of the list. */
extern int get_list_length(List_Entry *entry);

/*In dirent.h, Open a directory stream corresponding */
extern DIR *opendir(const char *dirname);

/*In obexftp/client.h, Create an obexftp client.*/
extern obexftp_client_t * obexftp_open(int transport, obex_ctrans_t *ctrans,
    obexftp_info_cb_t infocb, void *infocb_data);

/*In strung.h, Fill block of memory*/
extern void * memset(void * ptr, int value, size_t num);

/* In stdlib.h, Allocate memory block.*/
extern void* malloc(size_t size);

/* In stdlib.h, Deallocate memory block.*/
extern void free(void* ptr);

/* In bluetooth.h,  Opens a Bluetooth socket with the specified resource
*  number*/
extern int hci_open_dev(int dev_id);

/*In bluetooth/hci_lib.h, Clear filter*/
extern void hci_filter_clear(struct hci_filter *    f);

/*In bluetooth/hci_lib.h, Filter set ptype */
extern void hci_filter_set_ptype(int t, struct hci_filter *f);

/*In bluetooth/hci_lib.h, Filter set event */
extern void hci_filter_set_event(int e, struct hci_filter *f);

/*In bluetooth/hci_lib.h, Configure inquiry mode */
extern int hci_write_inquiry_mode(int dd, uint8_t mode, int to);

/*In bluetooth/hci_lib.h, Send cmd */
extern int  hci_send_cmd(int dd, uint16_t ogf, uint16_t ocf, uint8_t plen,
    void *param);

/*In pthread.h, Initialize thread attributes object*/
extern int pthread_attr_init(pthread_attr_t *attr);

/*In pthread.h, Destroy thread attributes object*/
extern int pthread_attr_destroy(pthread_attr_t *attr);

/*In pthread.h, Detach a thread*/
extern int pthread_detach(pthread_t thread);

/*In pthread.h, Create a new thread*/
extern int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    void *(*start_routine) (void *), void *arg);



