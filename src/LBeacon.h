/*
Copyright (c) 2016 Academia Sinica, Institute of Information Science

License:

    GPL 3.0 : The content of this file is subject to the terms and
    conditions defined in file 'COPYING.txt', which is part of this source
    code package.

Project Name:

    BeDIPS

File Description:

    This header file contains function declarations of variables, struct and
    functions and definitions pf global variables used in the LBeacon.c file.

File Name:

    LBeacon.h

Version:
 
    1.2

Abstract:

    BeDIPS uses LBeacons to deliver 3D coordinates and textual
    descriptions of their locations to users' devices. Basically, a
    LBeacon is an inexpensive, Bluetooth Smart Ready device. The 3D
    coordinates and location description of every LBeacon are retrieved
    from BeDIS (Building/environment Data and Information System) and
    stored locally during deployment and maintenance times. Once
    initialized, each LBeacon broadcasts its coordinates and location
    description to Bluetooth enabled user devices within its coverage
    area.

Authors:

    Han Wang, hollywang@iis.sinica.edu.tw
    Jake Lee, jakelee@iis.sinica.edu.tw
    Johnson Su, johnsonsu@iis.sinica.edu.tw
    Joey Zhou, joeyzhou@iis.sinica.edu.tw
    Kenneth Tang, kennethtang@iis.sinica.edu.tw
    James Huamg, 
    Shirley Huang, shirley.huang.93@gmail.com
    Jeffrey Lin, lin.jeff03@gmail.com
    Howard Hsu, haohsu0823@gmail.com
      
*/



/*
* INCLUDES
*/

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <ctype.h>
#include <dirent.h>
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
#include "zlog.h"
#include "LinkedList.h"
#include "Utilities.h"
#include "Mempool.h"
#include "Communication.h"
#include "thpool.h"


/*
  CONSTANTS
*/

/* Command opcode pack/unpack from HCI library. ogf and ocf stand for Opcode 
   group field and Opcofe command field, respectively. See Bluetooth 
   specification - core version 4.0, vol.2, part E Chapter 5.4 for details. 
*/
#define cmd_opcode_pack(ogf, ocf) (uint16_t)((ocf &amp; 0x03ff) | \
                                                        (ogf &lt;&lt; 10)) 
/* File path of the config file of the LBeacon */
#define CONFIG_FILE_NAME "../config/config.conf"

/* File path of the config file of the logging file*/
#define LOG_FILE_NAME "../config/zlog.conf" 

#define LOG_CATEGORY_HEALTH_REPORT "Health_Report"


/* Maximum number of characters in each line of config file */
#define CONFIG_BUFFER_SIZE 64

/* Number of lines in the config file */
#define CONFIG_FILE_LENGTH 11

/* Parameter that marks the start of the config file */
#define DELIMITER "="

/* BlueZ bluetooth extended inquiry response protocol: flags */
#define EIR_FLAGS 0X01

/* BlueZ bluetooth extended inquiry response protocol: Manufacturer Specific 
   Data */
#define EIR_MANUFACTURE_SPECIFIC_DATA 0xFF

/* BlueZ bluetooth extended inquiry response protocol: complete local name */
#define EIR_NAME_COMPLETE 0x09

/* BlueZ bluetooth extended inquiry response protocol: short local name */
#define EIR_NAME_SHORT 0x08

/* Maximum number of characters in message file name */
#define FILE_NAME_BUFFER 64

/* Length in number of chars used for basic information */
#define LENGTH_OF_INFO 128

/* Number of milliseconds in an epoch */
#define LENGTH_OF_TIME 10

/* Maximum length of time in milliseconds a bluetooth device
   stays in the scanned device list 
*/
#define TIMEOUT 30000

/* Timeout in milliseconds of hci_send_req  */
#define HCI_SEND_REQUEST_TIMEOUT 1000

/* Time interval in milliseconds for a LBeacon to advertise*/
#define ADVERTISING_INTERVAL 300

/* The timeout in milliseconds for waiting in threads */
#define TIMEOUT_WAITING 300

/* Timeout interval in seconds */
#define A_LONG_TIME 30000
#define A_SHORT_TIME 5000
#define A_VERY_SHORT_TIME 300

/* Nominal transmission range limited. Only devices in this RSSI range are 
   allowed to be discovered and sent. */
#define RSSI_RANGE -60

/* RSSI value for TX power of calibration and broadcast  */
#define RSSI_VALUE -50

/* Number of characters in a Bluetooth MAC address */
#define LENGTH_OF_MAC_ADDRESS 18

/* Number of digits of MAC address to compare */ 
#define NO_DIGITS_TO_COMPARE 2

/* Number of worker threads in the thread pool used by communication unit */
#define NO_WORK_THREADS 2

/* Location data in the maximum number of objects to be transmitted at
   one time */
#define MAX_NO_OBJECTS 32

/* The number of slots in the memory pool */
#define SLOTS_IN_MEM_POOL 1024




/* The macro of comparing two integer for minimum */
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })



/*
  UNION
*/

/* This union will convert floats into Hex code used for the beacon
   location 
*/
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
  TYPEDEF STRUCTS
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

    /* String representation of the path name of message file */
    char file_path[CONFIG_BUFFER_SIZE];

    /* String length needed to store path name of message file */
    int file_path_length;

    /* String representation of the maximum number of devices to be
       handled by all push dongles 
    */
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

    /* String length needed to store uuid */
    int uuid_length;


} Config;

/* The structure for storing information and status of a thread */
typedef struct ThreadStatus {
    
    char scanned_mac_address[LENGTH_OF_MAC_ADDRESS];
    bool idle;
    bool is_waiting_to_send;

} ThreadStatus;


/* Struct for storing MAC address of a user's device and the time instant
   at which the address is scanned 
*/
typedef struct ScannedDevice {
    
    char scanned_mac_address[LENGTH_OF_MAC_ADDRESS];
    long long initial_scanned_time;
    long long final_scanned_time;
    struct List_Entry sc_list_entry;
    struct List_Entry tr_list_entry;

/* Pad added to make the struct size an integer multiple of 32 byte, size of
   D-cache line.
   
   int pad[30];
*/    

} ScannedDevice;



/*
  EXTERN STRUCTS
*/

/* In sys/poll.h, the struct for controlling the events. */
extern struct pollfd;

/* In hci_sock.h, the struct for callback event from the socket. */
extern struct hci_filter;



/*
  GLOBAL VARIABLES
*/

/* Path of the object push file */
char *g_push_file_path;

/* First timestamp of the output file used for tracking scanned
   devices 
*/
unsigned g_initial_timestamp_of_tracking_file = 0;

/* The most recent timestamp in the output file used for tracking scanned
   devices 
*/
unsigned g_most_recent_timestamp_of_tracking_file = 0;

/* Number of lines in the output file used for tracking scanned devices */
int g_size_of_file = 0;

/* Struct for storing config information from the input file */
Config g_config;

/* An array of struct for storing information and status of threads */
ThreadStatus g_idle_handler[MAX_NO_OBJECTS];


/* Two lists of structs for recording scanned devices */

/* Head of scanned_list that holds the scanned device structs of devices 
   found in recent scans. The MAC address elements of all the structs in this
   list are distinct. 
*/
List_Entry scanned_list_head;

/* Head of tracking_object_list that holds the scanned device structs of 
   devices discovered in recent scans. The MAC address elements of some 
   structs in the list may be identical but their associate timestamps 
   indicate disjoint time intervals. The contents of the list await to be 
   sent via the gateway to the server to be processed there. 
*/
List_Entry tracked_object_list_head;


/* Global flags for communication among threads */

/* A global flag that is initially set to true by the main thread. It is set
   to false by any thread when the thread encounters a fatal error, 
   indicating that it is about to exit. 
*/
bool ready_to_work;


/* The memory pool for the allocation of all nodes in scanned_device_list and
   tracked_object_list 
*/
Memory_Pool mempool;

/* The lock that controls access to lists */
pthread_mutex_t  list_lock;    

zlog_category_t *category_health_report;


/*
  ERROR CODE
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
    E_INIT_THREAD_POOL = 16,
    E_INIT_ZIGBEE = 17,
    E_ZIGBEE_CONNECT = 18,
    E_EMPTY_FILE = 19,
    MAX_ERROR_CODE = 20

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
    {E_INIT_THREAD_POOL, "Error initializing thread pool"},
    {E_INIT_ZIGBEE, "Error initializing the zigbee"},
    {E_ZIGBEE_CONNECT, "Error zigbee connection"},
    {E_EMPTY_FILE, "Error of empty file"},
    {MAX_ERROR_CODE, "The element is invalid"}

};



/*
  EXTERNAL GLOBAL VARIABLES
*/
extern int errno;



/*
  FUNCTIONS
*/

/*
  get_config:

      This function reads the specified config file line by line until the
      end of file and copies the data in the lines into the Config struct 
      global variable.

  Parameters:

      file_name - the name of the config file that stores all the beacon data

  Return value: 

      config - Config struct including file path, coordinates, etc.
*/

Config get_config(char *file_name);


/*
  get_system_time:
 
      This helper function fetches the current time according to the system
      clock in terms of the number of milliseconds since January 1, 1970.
 
  Parameters:
 
      None
 
  Return value:
 
      system_time - system time in milliseconds
*/

long long get_system_time();




/*
  print_RSSI_value:

      This function prints the RSSI value along with the MAC address of the
      user's scanned bluetooth device. 

  Parameters:

      bluetooth_device_address - MAC address of a bluetooth device
      has_rssi - a flag indicating whether the bluetooth device has an RSSI 
                 value
      rssi - RSSI value of bluetooth device

  Return value:

      None
*/

void print_RSSI_value(bdaddr_t *bluetooth_device_address, bool has_rssi,
    int rssi);




/*
  send_to_push_dongle:

      When called, this functions first checks whether there is a 
      ScannedDevice struct in the scanned list with MAC address matching the 
      input bluetooth device address. If there is no such struct, this 
      function allocates from memory pool space for a ScannedDeice struct, 
      sets the MAC address of the new struct to the input MAC address, the 
      initial scanned time and final scanned time to the current time, and 
      inserts the sruct at the head of of the scanned list and tail of the 
      tracked object list. If a struct with MAC address matching the input 
      device address is found, this function sets the final scanned time of 
      the struct to current time.

  Parameters:

      bluetooth_device_address - MAC address of a bluetooth device discovered
                                 during inquiry

  Return value:

      None
*/

void send_to_push_dongle(bdaddr_t *bluetooth_device_address);




/*
  check_is_in_scanned_list:

      This function checks whether the MAC address given as input is in the 
      scanned list. If a node with MAC address matching the input address is 
      found in the list, the function returns the pointer to the node with 
      matching address; otherwise it returns NULL.

  Parameters:

      address - MAC address of a bluetooth device

  Return value:

  match_node - A pointer to the node found with MAC address identical to 
               the input address.
               or NULL
*/

struct ScannedDevice *check_is_in_scanned_list(char address[]);




/*
  enable_advertising:

      This function enables the LBeacon to start advertising, sets the time
      interval for advertising, and calibrates the RSSI value.

  Parameters:

      advertising_interval - the time interval during which the LBeacon can
                         advertise 
      advertising_uuid - universally unique identifier for advertising
      rssi_value - RSSI value of the bluetooth device

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise 

*/

ErrorCode enable_advertising(int advertising_interval, 
                             char *advertising_uuid,
                             int rssi_value);


/*
  disable_advertising:

      This function disables the advertising capabilities of the beacon.

  Parameters:

      None

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise 
*/

ErrorCode disable_advertising();


/*
  stop_ble_beacon:

      This function allows avertising to be stopped with ctrl-c if a 
      precious call to enable_advertising was a success.

  Parameters:

      beacon_location - advertising uuid

  Return value:

      None
*/

void *stop_ble_beacon(void *beacon_location);


/*
  cleanup_scanned_list:

      This function checks each ScannedDevice node in the scanned list to 
      determine whether the node has been in the list for over TIMEOUT unit 
      of time. If yes, the function removes the ScannedDevice struct from 
      the list. If the struct is no longer in the tracked_object_list, the 
      function calls the memory pool to release the memory space used by 
      the struct.

  Parameters:

      None

  Return value:

      None
*/

void *cleanup_scanned_list(void);



/*
  manage_communication:

      This is the start function of the main thread in the communication 
      unit of LBeacon. After initlizaiing the zigbee struct, it creates a 
      thread pool with NO_WORK_THREADS worker threads; then while beacon is 
      ready to work, the function waits for poll from the gateway, when 
      polled, the function creates appropriate work items to be executed by 
      a worker thread. 

  Parameters:

       None

  Return value:

      None
*/

void *manage_communication(void);



/*
  copy_object_data_to_file:

      This function copies the MAC addresses of scanned (i.e/ discovered)
      bluetooth devices under a location beacon to a file. The output file
      contains for each MAC address in a ScannedDevice struct found in the 
      track object list, the initial and final timestamps. 

  Parameters:

      file_name - name of the file where data is stored in all ScannedDevice 
                  struct found in tracked object list

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise 
*/

ErrorCode copy_object_data_to_file(char *file_name);


/*
  free_list:

      This function removes nodes from the specified list and if the removed 
      node is not in any list, call memory pool to release memoory used by 
      the node.

  Parameters:

      list_head - the head of a specified list.

  Return value:

      None
*/

void free_list(List_Entry *list_head);


/*
  start_scanning:

      This function scans continuously for bluetooth devices under the 
      coverage of the  beacon until scanning is cancelled. When the RSSI 
      value of the device is within the threshold, this function calls 
      send_to_push_dongle to either add a new ScannedDevice struct of the 
      device to scanned list and track_object_list or update the final scan 
      time of a struct in the lists. 

      [N.B. This function is executed by the main thread. ]

  Parameters:

      None

  Return value:

      None
*/

void start_scanning();


/*
  startThread:

      This function initializes a specified thread.

  Parameters:

      thread - name of a thread
      threadfunct - the function to be executed by the thread
      arg - the argument for thread function

  Return value:

      ErrorCode: The error code for the corresponding error if the function
                 fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode startThread(pthread_t thread, void * (*threadfunct)(void*), 
                                        void *arg);


/*
  cleanup_exit:

      This function releases all the resources.

  Parameters:

      None

  Return value:

      None
*/

void cleanup_exit();





/*
  EXTERNAL FUNCTIONS
*/



/* 
  opendir:

      This function is called to open a specified directory. 

  Parameters:

      dirname - the name of the directory which is goning to be opened.

  Return value:

      dirp - a pointer to the directory stream.
*/
extern DIR *opendir(const char *dirname);


/* 
  obexftp_open:

      This function is called to create an obexftp client.

  Parameters:

      transport - the transport type that will be used
      ctrans - optional custom transport
      infocb - optional info callback
      infocb_data - optional info callback data

  Return value:

      cli - a new allocated ObexFTP client instance, NULL on error.
*/
extern obexftp_client_t * obexftp_open(int transport, obex_ctrans_t *ctrans,
    obexftp_info_cb_t infocb, void *infocb_data);


/* 
  memset:

      This function is called to fill a block of memory.

  Parameters:

      ptr - the pointer points to the memory area
      value - the constant byte to replace the memory area
      number - number of bytes in the memory area to be filled

  Return value:

      dst - a pointer to the memory area
*/
extern void * memset(void * ptr, int value, size_t number);


/*
  hci_open_dev:
  
      This function is called to open a Bluetooth socket with the specified resource number.

  Parameters:

      dev_id - the id of the Bluetooth socket device

  Return value:

      dd - device descriptor of the Bluetooth socket
*/
extern int hci_open_dev(int dev_id);


/* 
  hci_filter_clear:

      This function is called to clear filter.

  Parameters:

      f - the filter to be cleaned

  Return value:

      None
*/
extern void hci_filter_clear(struct hci_filter *f);


/* 
  hci_filter_set_ptype:

      This function is called to let filter set ptype.

  Parameters:

      t - the type
      f - the filter to be set

  Return value:

      None
*/
extern void hci_filter_set_ptype(int t, struct hci_filter *f);


/* 
  hci_filter_set_event:

      This function is called to let filter set event

  Parameters:

      e - the event
      f - the filter to be set

  Return value:

      None
*/
extern void hci_filter_set_event(int e, struct hci_filter *f);


/* 
  hci_write_inquiry_mode:

      This function is called to configure inquiry mode

  Parameters:

      dd - device descriptor of the open HCI socket
      mode - new inquiry mode
      to - send request to this

  Return value:

      None
*/
extern int hci_write_inquiry_mode(int dd, uint8_t mode, int to);


/* 
  hci_send_cmd:

      This function is called to send cmd

  Parameters:

      dd - device descriptor of the open HCI socket
      ogf - opcode group field 
      ocf - opcode command field
      plen - the length of the command parameters 
      param - the parameters that function runs with

  Return value:

      0 for success. error number for error.
*/
extern int  hci_send_cmd(int dd, uint16_t ogf, uint16_t ocf, uint8_t plen,
    void *param);


/* 
  pthread_attr_init:

      This function is called to initialize thread attributes object

  Parameters:

      attr - the thread attributes object to be initiallized

  Return value:

      0 for success. error number for error.
*/
extern int pthread_attr_init(pthread_attr_t *attr);


/* 
  pthread_attr_destroy:

      This function is called to destroy thread attributes object

  Parameters:

      attr - the thread attributes object to be destroyed

  Return value:

      0 for success. error number for error.
*/
extern int pthread_attr_destroy(pthread_attr_t *attr);


/* 
  pthread_detach:

      This function is called to detach a thread

  Parameters:

      thread - a thread to be detached

  Return value:

      0 for success. error number for error.
*/
extern int pthread_detach(pthread_t thread);


/* 
  pthread_create:

      This function is called to create a new thread

  Parameters:

      thread - a pointer to the new thread
      attr - set thread properties
      arg - the parameters that function runs with

  Return value:

      0 for success. error number for error and the contents of *thread are 
      undefined.
*/
extern int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    void *(*start_routine) (void *), void *arg);


/*
  zigbee_send_file:

      When called, this function sends a containing the specified message 
      packet to the gateway via xbee module and and receives command or data 
      from the gateway. 

  Parameters:

      zigbee - the struct of necessary parameter and data

  Return value:

      None

*/
extern void zigbee_send_file(Zigbee *zigbee);


/* Follow are functions for communication via BR/EDR path to Bluetooth
   classic devices */
#ifdef Bluetooth_classic

/*
  choose_file:

    This function receives the name of the message file and returns the file 
    path where the message is located. It goes through each directory in the 
    messages folder and in each category, it reads each file name to find the 
    designated message we want to broadcast to the users under the beacon.

  Parameters:

    message_to_send - name of the message file we want to retrieve

  Return value:

    eturn_value - message file path
*/


char *choose_file(char *message_to_send);

/*
  send_file:

    This function pushes a message asynchronously to devices. It is the 
    thread function of the specified thread.

    [N.B. The beacon may still be scanning for other bluetooth devices.]

  Parameters:

    id - ID of the thread used to send the push message

  Return value:

    None
*/

void *send_file(void *id);

#endif // Bluetooth_classic