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

/* Number of lines in the config file */
#define ConFIG_FILE_LENGTH 11

/* Parameter that marks the start of the config file */
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

/* Maximum length of time in milliseconds, a bluetooth device
* stays in the push list */
#define TIMEOUT 30000

/* Maximum number of characters in each line of output file used for tracking
 * scanned devices */
#define TRACKING_FILE_LINE_LENGTH 1024

/* The number of char has been checked */
#define NUMBER_CHAR_CHECKED 10

/* Number of characters in a Bluetooth MAC address */
#define LENGTH_OF_MAC_ADDRESS 18

/* Timeout of hci_send_req  */
#define HCI_SEND_REQUEST_TIMEOUT 1000

/* Maximum length of time interval in seconds for Send to gateway */
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
    /* String representation of the X coordinate of the beacon location */
    char coordinate_X[CONFIG_BUFFER_SIZE];

    /* String representation of the Y coordinate of the beacon location */
    char coordinate_Y[CONFIG_BUFFER_SIZE];

    /* String representation of the Z coordinate of the beacon location */
    char coordinate_Z[CONFIG_BUFFER_SIZE];

    /* String representation of the message file name */
    char file_name[CONFIG_BUFFER_SIZE];

    /* String representation of the message file name's file path */
    char file_path[CONFIG_BUFFER_SIZE];

    /* String representation of the maximum number of devices to be
    handled by all push dongles */
    char maximum_number_of_devices[CONFIG_BUFFER_SIZE];

    /* String representation of number of message groups */
    char number_of_groups[CONFIG_BUFFER_SIZE];

    /* String representation of the number of messages */
    char number_of_messages[CONFIG_BUFFER_SIZE];

    /* String representation of the number of push dongles */
    char number_of_push_dongles[CONFIG_BUFFER_SIZE];

    /* String representation of the required signal strength */
    char rssi_coverage[CONFIG_BUFFER_SIZE];

    /* String representation of the universally unique identifer */
    char uuid[CONFIG_BUFFER_SIZE];

    /* The indicator for checking the beacon whether is initialized */
    char beacon_init[CONFIG_BUFFER_SIZE];

    /* String length needed to store coordinate_X */
    int coordinate_X_length;

    /* String length needed to store coordinate_Y */
    int coordinate_Y_length;

    /* String length needed to store coordinate_Z */
    int coordinate_Z_length;

    /* String length needed to store file name */
    int file_name_length;

    /* String length needed to store file path */
    int file_path_length;

    /* String length needed to store maximum_number_of_devices */
    int maximum_number_of_devices_length;

    /* String length needed to store number_of_groups */
    int number_of_groups_length;

    /* String length needed to store number_of_messages */
    int number_of_messages_length;

    /* String length needed to store number_of_push_dongles */
    int number_of_push_dongles_length;

    /* String length needed to store rssi_coverage */
    int rssi_coverage_length;

    /* TString length needed to store uuid */
    int uuid_length;

    /* The length of the indicator for initialization */
    int beacon_initialized_length;

} Config;


typedef struct ThreadStatus {
    char scanned_mac_address[LENGTH_OF_MAC_ADDRESS];
    bool idle;
    bool is_waiting_to_send;
} ThreadStatus;


/* Struct for storing MAC address of the user's device and the time instant
 * at when the address is scanned */
typedef struct ScannedDevice {
    long long initial_scanned_time;
    char scanned_mac_address[LENGTH_OF_MAC_ADDRESS];
} ScannedDevice;



/*
* ERROR CODE
*/

typedef enum Error_code {

    WORK_SCUCESSFULLY = 0,
    E_MALLOC = 1,
    E_OPEN_FILE = 2,
    E_OPEN_DEVICE = 3,
    E_SEND_OPEN_SOCKET = 4,
    E_SEND_OBEXFTP_CLIENT = 5,
    E_SEND_CONNECT_DEVICE = 6,
    E_SEND_PUT_FILE = 7,
    E_SEND_DISCONNECT_CLIENT = 8,
    E_SCAN_OPEN_SOCKET = 9,
    E_SCAN_SET_HCI_FILTER = 10,
    E_SCAN_SET_INQUIRY_MODE = 11,
    E_SCAN_START_INQUIRY = 12,
    E_SEND_REQUEST_TIMEOUT = 13,
    E_ADVERTISE_STATUS = 14,
    E_ADVERTISE_MODE = 15,
    E_START_THREAD = 16

}Error_code;

typedef enum Error_code error_t;

struct _errordesc {
    int code;
    char *message;
}errordesc[] = {

    {WORK_SCUCESSFULLY, "The code works successfullly"},
    {E_MALLOC, "Error with allocating memory"},
    {E_OPEN_FILE, "Error with opening file"},
    {E_OPEN_DEVICE, "Error with opening the dvice"},
    {E_SEND_OPEN_SOCKET, "Error with opening socket"},
    {E_SEND_OBEXFTP_CLIENT, "Error opening obexftp client"},
    {E_SEND_CONNECT_DEVICE, "Error connecting to obexftp device"},
    {E_SEND_PUT_FILE, "Error with putting file"},
    {E_SEND_DISCONNECT_CLIENT, "Disconnecting the client"},
    {E_SCAN_OPEN_SOCKET, "Error with opening socket"},
    {E_SCAN_SET_HCI_FILTER, "Error with setting HCI filter"},
    {E_SCAN_SET_INQUIRY_MODE, "Error with settnig inquiry mode"},
    {E_SCAN_START_INQUIRY, "Error with starting inquiry"},
    {E_SEND_REQUEST_TIMEOUT, "Timeout for sending request"},
    {E_ADVERTISE_STATUS, "LE set advertise returned status"},
    {E_ADVERTISE_MODE, "Error with setting advertise mode"},
    {E_START_THREAD, "Error with creating thread"},

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

/* Path of the object push file */
char *g_push_file_path;

/* First timestamp of the output file used for tracking scanned
* devices */
unsigned g_initial_timestamp_of_tracking_file = 0;

/* The most recent timestamp in the output file used for tracking scanned
* devices */
unsigned g_most_recent_timestamp_of_tracking_file = 0;

/* Number of lines in the output file used for tracking scanned devices */
int g_size_of_file = 0;

/* Struct for storing config information from the input file */
Config g_config;


/* An array of struct for storing information and status of each thread */
ThreadStatus *g_idle_handler;

/* Three list of struct for recording scanned devices */

/* Head of scanned_list that holds the scanned device structs of devices found in recent scan.
* Some of the structs in this list may be duplicated.*/
List_Entry *scanned_list;

/* Head of waiting_list that holds the scanned device structs of devices
* waiting for an available thread to send messages to their address.*/
List_Entry *waiting_list;

/* Head of tracking_object_list that holds the scanned device structs of devices
* to be processed for each device in the list, a line contain of it's MAC
* address and time at which the address is found in placed to a tracked object
* buffer to be send the gateway and search.*/
List_Entry *tracked_object_list;

/* Global flags for communication among threads */

/* A global flag that in initially set to true by main thread. It is set to false
* by any thread when the thread encounters a fatal error, indicating that it is about to exit.*/
bool ready_to_work;

/* A global flag that will be set to true be the main thread to inform all of the thread that
* scanning operation have been canceled. The flag set by main thread.*/
bool send_message_cancelled;




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
*  send_to_push_dongle:
*
*  When called, this functions constructs a ScannedDevice struct using the
*  input bluetooth_device_address as MAC address and current time as timestamp.
*  It then checks whether there is a ScannedDevice struct in the scanned list
*  with MAC address matching the input MAC address. If there is no such
*  ScannedDevice struct, the function inserts the newly constructed struct at
*  the head of the waiting list. It inserts new struct at the head of the
*  scanned list regarded as the results of above mentioned test.
*
*  Parameters:
*
*  bluetooth_device_address - bluetooth device address
*
*  Return value:
*
*  None
*/
void send_to_push_dongle(bdaddr_t *bluetooth_device_address);

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
    int rssi);

/*
*  track_devices:
*
*  This function tracks the MAC addresses of scanned bluetooth devices under
*  the beacon. An output file will contain for each timestamp and the MAC
*  addresses of the scanned bluetooth devices at the given timestamp.
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
*  check_is_in_list:
*
*  This helper function checks whether the specified MAC address given as
*  input is in the specified list of ScannedDevice struct of bluetooth devices.
*  If it is, the function returns true, else the function returns false.
*
*  Parameters:
*
*  list - the list is going to be checked
*  address - scanned MAC address of a bluetooth device
*
*  Return value:
*
*  temp - the node which its MAC address matched with the input address
*  NULL - there is no matched address in the list
*
*/
struct Node *check_is_in_list(List_Entry *list, char address[]);

/*
*  print_MACaddress:
*
*  This helper function prints the MAC addresses which is used with the
*  function of print_list defined in LinkedList.h.
*
*  Parameters:
*
*  sc - anytype of data which will be printed
*
*  Return value:
*
*  None
*/
void print_MACaddress(void *sc);

/*
*  print_Timestamp:
*
*  This helper function prints the timestamp which is used with the
*  function of print_list defined in LinkedList.h.
*
*  Parameters:
*
*  sc - anytype of data which will be printed
*
*  Return value:
*
*  None
*/
void print_Timestamp(void *sc);

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
Error_code enable_advertising(int advertising_interval, char *advertising_uuid,
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
*  Error_code: The error code for the corresponding error
*
*/
Error_code disable_advertising();

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
*  Error_code: The error code for the corresponding error
*/
void *stop_ble_beacon(void *beacon_location);

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
void *cleanup_scanned_list(void);

/*
*  queue_to_array:
*
*  This function continuously looks through the ThreadStatus array that
*  contains the status of all the send_file thread. When the function finds the
*  ThreadStatus of available thread and the waiting list is not empty, the
*  function removes the last node from the waiting list and copies the  MAC
*  address in the removed node to the ThreadStatus.
*
*  Parameters:
*
*  None
*
*  Return value:
*
*  None
*/
void *queue_to_array();

/*
*  send_file:
*
*  This function pushes a message asynchronously to devices. It is the thread
*  function of the specified thread.
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
void *send_file(void *dongle_id);

/*
*  start_scanning:
*
*  This function scans continuously for bluetooth devices under the coverage
*  of the  beacon until scanning is cancelled. Each scanned device fall under
*  one of three cases: a bluetooth device with no RSSI value and a bluetooth
*  device with a RSSI value, When the RSSI value of the device is within the
*  threshold, the ScannedDevice struct of the device is be added to the linked
*  list of devices to which messages will be sent.
*
*  [N.B. This function is extented by the main thread. ]
*
*  Parameters:
*
*  None
*
*  Return value:
*
*  None
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
*  thfunct - the function for thread to do
*  arg - the argument for thread's function
*
*  Return value:
*
*  Error_code: The error code for the corresponding error
*/
Error_code startThread(pthread_t threads, void * (*thfunct)(void*), void *arg);

/*
*  cleanup_exit:
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
void cleanup_exit();
/*
*  setminprio:
*
*  This function is call to set specified thread's minimum priority.
*
*  Parameters:
*
*  None
*
*  Return value:
*
*  None
*/
void setminprio(pthread_t threads);

/*
* EXTERNAL FUNCTIONS
*/

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
