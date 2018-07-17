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
*      This header file contains function declarations and
*      variables used in the LBeacon.c file.
*
* File Name:
*
*      LBeacon.h
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
#include "xbee_API.h"
#include "Mempool.h"
#include "thpool.h"



/*
* CONSTANTS
*/

/* Command opcode pack/unpack from HCI library. ogf and ocf stand for Opcode 
   group field and Opcofe command field, respectively. See Bluetooth 
   specification - core version 4.0, vol.2, part E Chapter 5.4 for details. */
#define cmd_opcode_pack(ogf, ocf) (uint16_t)((ocf & 0x03ff) | (ogf << 10))

/* File path of the config file */
#define CONFIG_FILE_NAME "../config/config.conf"

/* Maximum number of characters in each line of config file */
#define CONFIG_BUFFER_SIZE 64


/* Number of lines in the config file */
#define ConFIG_FILE_LENGTH 11

/* Parameter that marks the start of the config file */
#define DELIMITER "="

/* BlueZ bluetooth extended inquiry response protocol: flags */
#define EIR_FLAGS 0X01

/* BlueZ bluetooth extended inquiry response protocol: Manufacturer Specific
 v Data */
#define EIR_MANUFACTURE_SPECIFIC_DATA 0xFF

/* BlueZ bluetooth extended inquiry response protocol: complete local name */
#define EIR_NAME_COMPLETE 0x09

/* BlueZ bluetooth extended inquiry response protocol: short local name */
#define EIR_NAME_SHORT 0x08

/* Maximum number of characters in message file name */
#define FILE_NAME_BUFFER 64

/* Length of time in Epoch */
#define LENGTH_OF_TIME 10

/* Nominal transmission range limited. Only devices in this RSSI range are 
   allowed to be discovered and sent */
#define RSSI_RANGE -60

/* Maximum length of time in milliseconds, a bluetooth device
   stays in the push list */
#define TIMEOUT 30000

#define MAXIMUM_NUMBER_OF_DEVICES 10

/* Number of characters in a Bluetooth MAC address */
#define LENGTH_OF_MAC_ADDRESS 18

/* Timeout in millisecond of hci_send_req  */
#define HCI_SEND_REQUEST_TIMEOUT 1000

/* Time interval for the LBeacon to advertise*/
#define ADVERTISING_INTERVAL 300

/* RSSI value for the calibration */
#define RSSI_VALUE -50

/* The maximum number for  concureent zigbee transmission at one time */
#define MAX_NO_OBJECTS 10

/* The number of slots for the memory pool */
#define SLOTS_FOR_MEM_POOL 1024

/* Timeout interval in ms */
#define A_LONG_TIME 30000
#define A_SHORT_TIME 5000
#define A_VERY_SHORT_TIME 300
#define A_VERY_VERY_SHORT_TIME 30

/* The macro of comparing two integer for minimum */
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })



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

/* The configuration file structure */

typedef struct Config {
   
    /* String representation of the X coordinate of the beacon location */
    char coordinate_X[CONFIG_BUFFER_SIZE];

    /* String length needed to store coordinate_X */
    int coordinate_X_length;

    /* String representation of the Y coordinate of the beacon location */
    char coordinate_Y[CONFIG_BUFFER_SIZE];

    /* String length needed to store coordinate_Y */
    int coordinate_Y_length;

    /* String representation of the Z coordinate of the beacon location */
    char coordinate_Z[CONFIG_BUFFER_SIZE];

       /* String length needed to store coordinate_Z */
    int coordinate_Z_length;

    /* String representation of the message file name */
    char file_name[CONFIG_BUFFER_SIZE];

    /* String length needed to store file name */
    int file_name_length;

    /* String representation of the message file name's file path */
    char file_path[CONFIG_BUFFER_SIZE];

    /* String length needed to store file path */
    int file_path_length;

    /* String representation of the maximum number of devices to be
    handled by all push dongles */
    char maximum_number_of_devices[CONFIG_BUFFER_SIZE];

    /* String length needed to store maximum_number_of_devices */
    int maximum_number_of_devices_length;

    /* String representation of number of message groups */
    char number_of_groups[CONFIG_BUFFER_SIZE];

    /* String length needed to store number_of_groups */
    int number_of_groups_length;

    /* String representation of the number of messages */
    char number_of_messages[CONFIG_BUFFER_SIZE];

    /* String length needed to store number_of_messages */
    int number_of_messages_length;

    /* String representation of the number of push dongles */
    char number_of_push_dongles[CONFIG_BUFFER_SIZE];

    /* String length needed to store number_of_push_dongles */
    int number_of_push_dongles_length;

    /* String representation of the required signal strength */
    char rssi_coverage[CONFIG_BUFFER_SIZE];

    /* String length needed to store rssi_coverage */
    int rssi_coverage_length;

    /* String representation of the universally unique identifer */
    char uuid[CONFIG_BUFFER_SIZE];

    /* TString length needed to store uuid */
    int uuid_length;


} Config;

/* The structure for storing information and status of a thread */
typedef struct ThreadStatus {
    
    char scanned_mac_address[LENGTH_OF_MAC_ADDRESS];
    bool idle;
    bool is_waiting_to_send;

} ThreadStatus;


/* Struct for storing MAC address of the user's device and the time instant
 * at which the address is scanned */
typedef struct ScannedDevice {
    
    char scanned_mac_address[LENGTH_OF_MAC_ADDRESS];
    long long initial_scanned_time;
    long long final_scanned_time;
    struct List_Entry sc_list_entry;
    struct List_Entry tr_list_entry;

/* Pad added to make the struct size an integer multiple of 32 byte, size 
   of D -cache line.
   int pad[30];
*/    

} ScannedDevice;

/* Struct of parameters for Zigbee Initialization */
typedef struct Zigbee {

    /* Struct of xbee main part which is defined in "libxbee" library */
    struct xbee *xbee; 

    /* Struct of xbee connector which is defined in "libxbee" library */
    struct xbee_con *con;

    /* Struct of queue of packet which is defined in pkt_Queue.h */
    pkt_ptr pkt_Queue;
    
} Zigbee;



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

/* Path of the object push file */
char *g_push_file_path;

/* First timestamp of the output file used for tracking scanned
   devices */
unsigned g_initial_timestamp_of_tracking_file = 0;

/* The most recent timestamp in the output file used for tracking scanned
   devices */
unsigned g_most_recent_timestamp_of_tracking_file = 0;

/* Number of lines in the output file used for tracking scanned devices */
int g_size_of_file = 0;

/* Struct for storing config information from the input file */
Config g_config;

/* An array of struct for storing information and status of each thread */
ThreadStatus g_idle_handler[MAXIMUM_NUMBER_OF_DEVICES];

/* Struct for storing necessary objects for zigbee connection */
Zigbee zigbee;


/* Two lists of struct for recording scanned devices */

/* Head of scanned_list that holds the scanned device structs of devices found
   in recent scans. The MAC address elements of all the structs in this this 
   list are distinct. */
List_Entry scanned_list_head;

/* Head of tracking_object_list that holds the scanned device structs of 
   devices discovered in recent scans. The MAC address elements of some 
   structs in the list may ve identical but their associate timestamps 
   indicate disjoint time intervals. The contents of the list await to be 
   sent via the gateway to the server to be processed there. */
List_Entry tracked_object_list_head;


/* Global flags for communication among threads */

/* A global flag that is initially set to true by the main thread. It is set
   to false by any thread when the thread encounters a fatal error, 
   indicating that it is about to exit. */
bool ready_to_work;

/* A global falg to inform the track_object_thread to transmit all or part of 
   the contents of tracked object list to the gateway */
bool is_polled_by_gateway;

/* The memory pool for the allocation of all nodes in scanned_device_list and
   tracked_object_list */
Memory_Pool mempool;

Threadpool thpool;
 
/* A lock for the processing of writing data */ 
pthread_mutex_t lock;



/*
* ERROR CODE
*/

typedef enum ErrorCode {

    WORK_SCUCESSFULLY = 0,
    E_MALLOC = 1,
    E_OPEN_FILE = 2,
    E_OPEN_DEVICE = 3,
    E_OPEN_SOCKET = 4,
    E_SEND_OBEXFTP_CLIENT = 5,
    E_SEND_CONNECT_DEVICE = 6,
    E_SEND_PUT_FILE = 7,
    E_SEND_DISCONNECT_CLIENT = 8,
    E_SCAN_SET_HCI_FILTER = 9,
    E_SCAN_SET_INQUIRY_MODE = 10,
    E_SCAN_START_INQUIRY = 11,
    E_SEND_REQUEST_TIMEOUT = 12,
    E_ADVERTISE_STATUS = 13,
    E_ADVERTISE_MODE = 14,
    E_START_THREAD = 15,
    E_ZIGBEE_CONNECT = 16,
    MAX_ERROR_CODE = 17

} ErrorCode;

typedef enum ErrorCode error_t;

struct _errordesc {
    int code;
    char *message;
} errordesc[] = {

    {WORK_SCUCESSFULLY, "The code works successfullly"},
    {E_MALLOC, "Error allocating memory"},
    {E_OPEN_FILE, "Error opening file"},
    {E_OPEN_DEVICE, "Error opening the dvice"},
    {E_OPEN_SOCKET, "Error opening socket"},
    {E_SEND_OBEXFTP_CLIENT, "Error opening obexftp client"},
    {E_SEND_CONNECT_DEVICE, "Error connecting to obexftp device"},
    {E_SEND_PUT_FILE, "Error putting file"},
    {E_SEND_DISCONNECT_CLIENT, "Disconnecting the client"},
    {E_SCAN_SET_HCI_FILTER, "Error setting HCI filter"},
    {E_SCAN_SET_INQUIRY_MODE, "Error settnig inquiry mode"},
    {E_SCAN_START_INQUIRY, "Error starting inquiry"},
    {E_SEND_REQUEST_TIMEOUT, "Timeout for sending request"},
    {E_ADVERTISE_STATUS, "LE set advertise returned status"},
    {E_ADVERTISE_MODE, "Error setting advertise mode"},
    {E_START_THREAD, "Error creating thread"},
    {E_ZIGBEE_CONNECT, "Error zigbee connection"},
    {MAX_ERROR_CODE, "The element is invalid"},

};



/*
* EXTERNAL GLOBAL VARIABLES
*/
extern int errno;



/*
* FUNCTIONS
*/

/*
*  get_config:
*
*  This function reads the specified config file line by line until the
*  end of file, and stores the data in the lines into the global variable of a
*  Config struct.
*
*  Parameters:
*
*  file_name - the name of the config file that stores all the beacon data
*
*  Return value:
*
*  config - Config struct including file path, coordinates, etc.
*/

Config get_config(char *file_name);


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

long long get_system_time();


/*
  send_to_push_dongle:

  When called, this functions first checks whether there is a ScannedDevice 
  struct in the scanned list with MAC address matching the input bluetooth 
  device address. If there is no such struct, this function allocates from 
  memory pool space for a ScannedDeice struct, sets the MAC address of the 
  new struct to the input MAC address, the initial scanned time and final
  scanned time to the current time, and inserts the sruct at the head of of 
  the scanned list and tail of the tracked object list. If a struct with MAC
  address matching the input device address is found, this function sets the 
  final scanned time of the struct to current time.

  Parameters:

  bluetooth_device_address - bluetooth device address

  Return value:

  None
*/

void send_to_push_dongle(bdaddr_t *bluetooth_device_address);


/*
*  print_RSSI_value:
*
*  This function prints the RSSI value along with the MAC address of the
*  user's scanned bluetooth device. 
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
    int rssi);




/*
  check_is_in_scanned_list:

  This function checks whether the MAC address given as input is in the 
  scanned list. If a node with MAC address matching the input address is 
  found in the list, the function returns the pointer to the node with 
  maching address, otherwise it returns NULL.

  Parameters:

  address - MAC address of a bluetooth device

  Return value:

  match_node - A pointer to the node found with MAC address matched up with 
               the input address.
  or NULL
  

*/

struct ScannedDevice *check_is_in_scanned_list(char address[]);


/*
 *  print_list:
 *
 *  This function prints the data in the specified list in the order of head 
 *  to tail. fpitr is used to access the function to be used for printing 
 *  current node data.
 *
 *  Parameters:
 *
 *  entry - the head of the list for determining which list is goning to be 
 *  modified.
 *  ptrs_type - an indicator of the pointer type of the specific list
 *
 *  Return value:
 *
 *  None
 */

void print_list(List_Entry *entry, int ptrs_type);


/*
*  enable_advertising:
*
*  This function enables the LBeacon to start advertising, sets the time
*  interval for advertising, and calibrates the RSSI value.
*
*  Parameters:
*
*  advertising_interval - the time interval for which the LBeacon can
*  advertise 
*  advertising_uuid - universally unique identifier for advertising
*  rssi_value - RSSI value of the bluetooth device
*
*  Return value:
*
*  ErrorCode: The error code for the corresponding error
*
*/

ErrorCode enable_advertising(int advertising_interval, char *advertising_uuid,
                            int rssi_value);


/*
*  disable_advertising:
*
*  This function disables the advertising capabilities of the beacon.
*
*  Parameters:
*
*  None
*
*  Return value:
*
*  ErrorCode: The error code for the corresponding error
*
*/

ErrorCode disable_advertising();


/*
*  stop_ble_beacon:
*
*  This function allows avertising to be stopped with ctrl-c if a precious 
*  call to enable_advertising was a success.
*
*  Parameters:
*
*  beacon_location - advertising uuid
*
*  Return value:
*
*  None
*/

void *stop_ble_beacon(void *beacon_location);


/*
  cleanup_scanned_list:

  This function checks each ScannedDevice node in the scanned list to 
  determine whether the node has been in the list for over TIMEOUT, if yes, 
  the function removes the ScannedDevice struct from the list. If the struct 
  is no longer in the tracked_object_list, the function call the memory 
  pool to release the memory space used by the struct.

  Parameters:

  None

  Return value:

  None
*/

void *cleanup_scanned_list(void);



/*
*  track_devices:
*
*  This function tracks the MAC addresses of scanned (is discovered) bluetooth
*  devices under a location beacon. An output file will contain for each 
*  timestamp, the MAC addresses of the bluetooth devices discovered at the 
*  given timestamp.
*
*  Parameters:
*
*  file_name - name of the file where all the data will be stored
*
*  Return value:
*
*  None
*/

void *track_devices(char *file_name);


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

ErrorCode zigbee_connection(Zigbee zigbee, char *message);


/*
  start_scanning:

  This function scans continuously for bluetooth devices under the coverage
  of the  beacon until scanning is cancelled. When the RSSI value of the 
  device is within the threshold, this function calls send_to_push_dongle to
  either add a new ScannedDevice struct of the device to scanned list and 
  track_object_list or update the struct in the lists. 

  [N.B. This function is executed by the main thread. ]

  Parameters:

  None

  Return value:

  None
*/

void start_scanning();


/*
*  startThread:
*
*  This function initializes the threads.
*
*  Parameters:
*
*  threads - name of the thread
*  threadfunct - the function for thread to do
*  arg - the argument for thread's function
*
*  Return value:
*
*  ErrorCode: The error code for the corresponding error
*/

ErrorCode startThread(pthread_t threads, void * (*threadfunct)(void*), 
                        void *arg);


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

void cleanup_exit();





/*
* EXTERNAL FUNCTIONS
*/

//Node *add_node(struct List_Entry *entry);

/* This function is called to insert a node at the head of the list.*/
extern void list_insert_head(List_Entry *new_node, List_Entry *head);

/* This function is called to remove a node from the list.*/
extern void list_remove_node(List_Entry *removed_node_ptrs);

/* This function is called to get the length of the specified list. */
extern int get_list_length(List_Entry *entry);

/* This function is called to open a specified directory */
extern DIR *opendir(const char *dirname);

/* This function is called to create an obexftp client.*/
extern obexftp_client_t * obexftp_open(int transport, obex_ctrans_t *ctrans,
    obexftp_info_cb_t infocb, void *infocb_data);

/* This function is called to fill block of memory*/
extern void * memset(void * ptr, int value, size_t num);

/* This function is called to allocate memory block.*/
extern void* malloc(size_t size);

/* This function is called to deallocate memory block.*/
extern void free(void* ptr);

/* This function is called to open a Bluetooth socket with the specified resource
*  number*/
extern int hci_open_dev(int dev_id);

/* This function is called to clear filter */
extern void hci_filter_clear(struct hci_filter *    f);

/* This function is called to let filter set ptype */
extern void hci_filter_set_ptype(int t, struct hci_filter *f);

/* This function is called to let filter set event */
extern void hci_filter_set_event(int e, struct hci_filter *f);

/* This function is called to configure inquiry mode */
extern int hci_write_inquiry_mode(int dd, uint8_t mode, int to);

/* This function is called to send cmd */
extern int  hci_send_cmd(int dd, uint16_t ogf, uint16_t ocf, uint8_t plen,
    void *param);

/* This function is called to initialize thread attributes object*/
extern int pthread_attr_init(pthread_attr_t *attr);

/* This function is called to destroy thread attributes object*/
extern int pthread_attr_destroy(pthread_attr_t *attr);

/* This function is called to detach a thread*/
extern int pthread_detach(pthread_t thread);

/* This function is called to create a new thread*/
extern int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    void *(*start_routine) (void *), void *arg);

