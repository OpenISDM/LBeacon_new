/*
Copyright (c) 2016 Academia Sinica, Institute of Information Science

License:

    GPL 3.0 : The content of this file is subject to the terms and
    conditions defined in file 'COPYING.txt', which is part of this source
    code package.

Project Name:

    BeDIS

File Description:

    This header file contains declarations of variables, structs and
    functions and definitions of global variables used in the LBeacon.c file.

File Name:

    LBeacon.h

Version:

    2.0,  20190911

Abstract:

    BeDIS uses LBeacons to deliver 3D coordinates and textual
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
    Joey Zhou, joeyzhou@iis.sinica.edu.tw
    Kenneth Tang, kennethtang@iis.sinica.edu.tw
    Chun Yu Lai, chunyu1202@gmail.com

*/

#ifndef LBEACON_H
#define LBEACON_H

/*
* INCLUDES
*/

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <netinet/in.h>
#include <obexftp/client.h>
#include "BeDIS.h"
#include "Version.h"


/*
  CONSTANTS
*/

/* File path of the config file of the LBeacon */
#define CONFIG_FILE_NAME "../config/config.conf"

/* File path of the logging file*/
#define LOG_FILE_NAME "../config/zlog.conf"

/* The temporary file for uploading tracked BR data */
#define TRACKED_BR_TXT_FILE_NAME "tracked_br_txt.txt"

/* The temporary file for uploading tracked BLE data */
#define TRACKED_BLE_TXT_FILE_NAME "tracked_ble_txt.txt"

/* The log file for LBeacon health history */
#define HEALTH_REPORT_LOG_FILE_NAME "../log/Health_Report.log"

/* The term used by zlog library to indicate error category. We will report
LBeacon's health report as errors, if we find this term in the last line of
Health_Report.log. */
#define HEALTH_REPORT_ERROR_SIGN "ERROR"

/* The lock file for LBeacon  */
#define LBEACON_LOCK_FILE "../bin/LBeacon.pid"

/* For following EIR_ constants, refer to Bluetooth specifications for
the defined values.
https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile
*/
/* BlueZ bluetooth extended inquiry response protocol: flags */
#define EIR_FLAGS 0X01

/* BlueZ bluetooth extended inquiry response protocol: short local name */
#define EIR_NAME_SHORT 0x08

/* BlueZ bluetooth extended inquiry response protocol: complete local name */
#define EIR_NAME_COMPLETE 0x09

/* BlueZ bluetooth extended inquiry response protocol: Manufacturer Specific
   Data */
#define EIR_MANUFACTURE_SPECIFIC_DATA 0xFF


/* Maximum number of characters in message file name */
#define FILE_NAME_BUFFER 64

/* Timeout in milliseconds of hci_send_req funtion */
#define HCI_SEND_REQUEST_TIMEOUT_IN_MS 1000

/* Time interval in seconds for cleaning up scanned list. The decision is
made by check_is_in_list. When the function checks for duplicated devices
in the scanned list, it will remove the timed out devices as well.
*/
#define INTERVAL_FOR_CLEANUP_SCANNED_LIST_IN_SEC 600

/* Time interval in seconds for idle status of the Wifi connection between the
LBeacon and gateway. Usually, the Wifi connection being idle for longer than
the specified time interval is impossible in BeDIS Object tracker solution. So
we treat the condition as a network connection failure. When this happens,
LBeacon sends UDP join_request to the gateway again.
*/
#define INTERVAL_RECEIVE_MESSAGE_FROM_GATEWAY_IN_SEC 30

/* Time interval in seconds for reconnect to Gateway */
#define INTERVAL_FOR_RECONNECT_GATEWAY_IN_SEC 30

/* Number of times to retry opening socket, because socket openning operation
   may have transient failure. */
#define SOCKET_OPEN_RETRY 5

/* Number of times to retry open file, because file openning operation may have
   transient failure. */
#define FILE_OPEN_RETRY 5

/* Number of times to retry getting a dongle, because this operation may have
   transient failure. */
#define DONGLE_GET_RETRY 5

/* Mempool usage threshold for cleaning up all lists. This threshold is used
to determine whether to cleanup all lists. */
#define MEMPOOL_USAGE_THRESHOLD 0.70

/* Number of characters in the name of a Bluetooth device */
#define LENGTH_OF_DEVICE_NAME 30

/* Number of characters in the advertising payload of a Bluetooth device */
#define LENGTH_OF_ADVERTISEMENT 33

/* Number of digits of MAC address to compare */
#define NUMBER_DIGITS_TO_COMPARE 4

/* Maximum length in number of bytes of basic info of each response from
LBeacon to gateway.
*/
#define MAX_LENGTH_RESP_BASIC_INFO 128

/* Maximum length in number of bytes of device information of each response
to gateway via wifi network link.*/
#define MAX_LENGTH_RESP_DEVICE_INFO 60

/* The BeDITech button tag identifer (0x0000000000000000) */
#define BEDITECH_BUTTON_TAG_IDENTIFIER "0000000000000000" 

/* The BeDITech button tag with battery voltage identifer 1478 (0x05C6) */
#define BEDITECH_BUTTON_BATTERY_TAG_IDENTIFIER "05C6" 

/* The macro of comparing two integer for minimum */
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

/*
  TYPEDEF STRUCTS
*/

/* The configuration file structure */

typedef struct Config {

    /* String representation of the area id of the beacon location */
    char area_id[CONFIG_BUFFER_SIZE];

    /* String representation of the X coordinate of the beacon location */
    char coordinate_X[CONFIG_BUFFER_SIZE];

    /* String representation of the Y coordinate of the beacon location */
    char coordinate_Y[CONFIG_BUFFER_SIZE];

    /* String representation of the Z coordinate of the beacon location */
    char coordinate_Z[CONFIG_BUFFER_SIZE];

    /* The expected lowest basement under ground in the world. This constant
    will be added to Z-coordinate (level information) gotten from input
    configuration file. This adjustment helps us to have positive number in the
    config data structure and lets Z-coordinate occupy only 2 bytes in UUID.
    */
    int lowest_basement_level;

    /* String representation of the universally unique identifer */
    char uuid[CONFIG_BUFFER_SIZE];

    /* The dongle used to advertise */
    int advertise_dongle_id;

    /* Time interval in units of 0.625ms between advertising by a LBeacon */
    int advertise_interval_in_units_0625_ms;

    /* The rssi value used to advertise */
    int advertise_rssi_value;

    /* The dongle used to scan */
    int scan_dongle_id;
    
    /* Time interval in units of 0.625ms between scanning by a LBeacon */
    int scan_interval_in_units_0625_ms;

    /* The required signal strength */
    int scan_rssi_coverage;

    /* The list of all acceptable mac address prefixes */
    struct List_Entry mac_prefix_list_head;

    /* The IPv4 network address of the gateway */
    char gateway_addr[NETWORK_ADDR_LENGTH];

    /* The UDP port of gateway connection*/
    int gateway_port;

    /* The IPv4 network address of LBeacon */
    char local_addr[NETWORK_ADDR_LENGTH];

    /* The UDP port for LBeacon to listen and receive UDP from gateway*/
    int local_client_port;

#ifdef Bluetooth_classic
    /* String representation of the message file name */
    char file_name[CONFIG_BUFFER_SIZE];

    /* String representation of the path  name of message file */
    char file_path[CONFIG_BUFFER_SIZE];

    /* String representation of the maximum number of devices to be
    handled by all push dongles
    */
    char maximum_number_of_devices[CONFIG_BUFFER_SIZE];

    /* String representation of number of message groups */
    char number_of_groups[CONFIG_BUFFER_SIZE];

    /* String representation of the number of messages */
    char number_of_messages[CONFIG_BUFFER_SIZE];

    /* String representation of the number of push dongles */
    char number_of_push_dongles[CONFIG_BUFFER_SIZE];

#endif

} Config;

/* The structure for storing information and status of a thread */
typedef struct ThreadStatus {

    char scanned_mac_address[LENGTH_OF_MAC_ADDRESS];
    bool idle;
    bool is_waiting_to_send;

} ThreadStatus;

/* Struct for storing MAC address of a Bluetooth device and the time instants
   at which the address is scanned
*/
typedef struct ScannedDevice {

    char scanned_mac_address[LENGTH_OF_MAC_ADDRESS];
    int initial_scanned_time;
    int final_scanned_time;
    int rssi;
    int is_button_pressed;
    int battery_voltage;
    /* List entries for linking the struct to scanned_list and
       tracked_BR_object_list or to tracked_BLE_object_list, depending
       whether the device type is BR_EDR or BLE. */
    struct List_Entry sc_list_entry;
    struct List_Entry tr_list_entry;

} ScannedDevice;

/* Struct for storing MAC address of a Bluetooth device and the advertising 
   payload and rssi signal strength
*/
typedef struct TempBleDevice {

    char scanned_mac_address[LENGTH_OF_MAC_ADDRESS];
    uint8_t payload[LENGTH_OF_ADVERTISEMENT];
    size_t payload_length;
    int rssi;
    
    struct List_Entry list_entry;

} TempBleDevice;

/* struct for device list head. */
typedef struct object_list_head{

    struct List_Entry list_entry;
    DeviceType device_type;

} ObjectListHead;

typedef struct PrefixString{

    char prefix[LENGTH_OF_MAC_ADDRESS];
    struct List_Entry list_entry;

} PrefixString;
/*
  EXTERN STRUCTS
*/

/* In sys/poll.h, the struct for controlling the events. */
//extern struct pollfd;

/* In hci_sock.h, the struct for callback event from the socket. */
//extern struct hci_filter;

/*
  EXTERNAL GLOBAL VARIABLES
*/

extern int errno;

/*
  GLOBAL VARIABLES
*/

/* Struct for storing config information from the input file */
Config g_config;

/* The struct of UDP configuration */
sudp_config udp_config;

/* Heads of three lists of structs for recording scanned devices */

/* Head of scanned_list that holds the scanned device structs of
   BR/EDR devices found in recent scans. The MAC address elements of all the
   structs in this list are distinct.
*/
ObjectListHead scanned_list_head;

/* Head of tracking_BR_object_list that holds the scanned device structs of
   Bluetooth devices discovered in recent scans. The MAC address elements of
   some structs in the list may be identical but their associated timestamps
   indicate disjoint time intervals. The contents of the list await to be
   sent via the gateway to the server to be processed there.
*/
ObjectListHead BR_object_list_head;

/* Head of tracking_BLE_object_list that holds the scanned device structs
   of BLE devices discovered in recent scans. The MAC address elements of
   some structs in the list may be identical but their associated timestamps
   indicate disjoint time intervals. The contents of the list await to be
   sent via the gateway to the server to be processed there.
*/
ObjectListHead BLE_object_list_head;

/* The pthread lock that controls access to lists */
pthread_mutex_t  list_lock;

/* Head of temp_ble_device_list that holds the scanned device information 
   structsof BLE devices discovered in recent scans. The contents of the list 
   await to be examined later and added into BLE_object_list if it meets BLE
   scanning criteria.
*/
struct List_Entry temp_ble_device_list_head;

/* The pthread lock that controls access to temp_ble_device_list */
pthread_mutex_t temp_ble_device_list_lock;

/* The memory pool for the allocation of all nodes in scanned device list and
   tracked object lists. */
Memory_Pool mempool;


/* The memory pool for the allocation of all scanned BLE devices */
Memory_Pool temp_ble_device_mempool;


/* Variables for storing the last polling times in second*/\
int gateway_latest_polling_time;

#ifdef Bluetooth_classic

/* Path of the object push file */
char *g_push_file_path;

/* An array of struct for storing information and status of threads */
ThreadStatus g_idle_handler[MAX_NUM_OBJECTS];

#endif

/*
  FUNCTIONS
*/

/*
  uuid_str_to_data:

     Convert uuid from string to unsigned integer.

  Parameters:

     uuid - The uuid in string type.

  Return value:

     unsigned int - The converted uuid in unsigned int type.
 */
unsigned int *uuid_str_to_data(char *uuid);

/*
  single_running_instance:

      This function write a file lock to ensure that system has only one
      instance of running LBeacon.

  Parameters:
      file_name - the name of the lock file that specifies PID of running
                  LBeacon

  Return value:
      ErrorCode - indicate the result of execution, the expected return code
                  is WORK_SUCCESSFULLY

*/

ErrorCode single_running_instance(char *file_name);

/*
  generate_uuid:

      This function generates the UUID of this LBeacon according to the 3D
      coordinates read from configuration file.

  Parameters:
      config - Config struct including file path, coordinates, etc.

  Return value:
      ErrorCode - indicate the result of execution, the expected return code
                  is WORK_SUCCESSFULLY

*/

ErrorCode generate_uuid(Config *config);

/*
  get_config:

      This function reads the specified config file line by line until the
      end of file and copies the data in the lines into the Config struct
      global variable.

  Parameters:
      config - Pointer to config struct including file path, coordinates, etc.
      file_name - the name of the config file that stores all the beacon data

  Return value:

      ErrorCode - indicate the result of execution, the expected return code
                  is WORK_SUCCESSFULLY
*/

ErrorCode get_config(Config *config, char *file_name);

/*
  send_to_push_dongle:

      When called, this functions first checks whether there is a
      ScannedDevice struct in the scanned list or ble_object_list with MAC
      address matching the input bluetooth device address depending on
      whether the device is a BR/EDR type or BLE type. If there is no such
      struct, this function allocates from memory pool space for a
      ScannedDeivce struct, sets the MAC address of the new struct to the
      input MAC address, the initial scanned time and final scanned time to
      the current time, and inserts the sruct at the head of the scanned_list
      if the device is of BR/EDR type, and tail of the tracked object list
      for the device type. If a struct with MAC address matching the input
      device address is found, this function sets the final scanned time of
      the struct to the current time.

  Parameters:

      mac_address - MAC address of a bluetooth device discovered during inquiry
      device_type - the indicator to show the device type of the input address
      rssi - the RSSI value of this device
      is_button_pressed - the push_button is pressed
      battery_voltage - the remaining battery voltage

  Return value:

      None
*/

void send_to_push_dongle(char * mac_address,
                         DeviceType device_type,
                         int rssi,
                         int is_button_pressed,
                         int battery_voltage);

/*
  compare_mac_address:

     This function compares two input MAC addresses specified by input
     parameters

  Parameters:

    address - the MAC address of a bluetooth device
    node - the node containing the other MAC address to compare
    number_digits_compared - number of digits in the addresses to be compared

  Return value:
    0: if the MAC addresses exactly match

*/

int compare_mac_address(char address[],
                        ScannedDevice *node,
                        int number_digits_compared);

/*
  check_is_in_list:

      This function checks whether the MAC address given as input is in the
      specified list. If a node with MAC address matching the input address
      is found in the list, the function returns the pointer to the node
      with matching address; otherwise it returns NULL.

  Parameters:

      address - MAC address of a bluetooth device
      list_head - the head of a specified list

  Return value:

  match_node - A pointer to the node found with MAC address identical to
               the input address.
               or NULL when no such node is found.
*/

struct ScannedDevice *check_is_in_list(char address[],
                                       ObjectListHead *list);

/*
  enable_advertising:

      This function enables the LBeacon to start advertising, sets the time
      interval for advertising, and calibrates the RSSI value.

  Parameters:

      dongle_device_id - the bluetooth dongle device which the LBeacon uses
                         to advertise
      advertising_interval_in_units_0625_ms - the time interval in units of 0.625ms 
                                              during which the LBeacon can advertise
      advertising_uuid - universally unique identifier of the advertiser
      major_number - major version number of LBeacon
      minor_number - minor version number of LBeacon
      rssi_value - RSSI value of the bluetooth device

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode enable_advertising(int dongle_device_id,
                             int advertising_interval_in_units_0625_ms,
                             char *advertising_uuid,
                             int major_number,
                             int minor_number,
                             int rssi_value);

/*
  disable_advertising:

      This function disables advertising of the beacon.

  Parameters:

      dongle_device_id - the bluetooth dongle device which the LBeacon needs to
                         disable advertising function

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode disable_advertising(int dongle_device_id);

/*
  beacon_basic_info

      This function prepares the basic information about the LBeacon which aims
      to help BeDIS server identify packets received from various gateways.
      [Note: The resulted message is in the format of
      "Packet type(one byte):<LBeacon UUID>:<Gateway IP address>"]
      Once the basic information is produced, the caller of this function can
      append more response content at the end of message.

  Parameters:

      message - the message buffer to contain the resulted basic information
      message_size - the size of message parameter
      poll_type - one of the packet types (also called communication
                  protocols between LBeacon and gateway). This function
                  needs this information to prepare the first byte of the
                  resulted message which will be parsed and utilized while
                  gateway receives the packet.

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode beacon_basic_info(char *message, size_t message_size, int poll_type);

/*
  send_join_request:

      This function sends join_request to gateway when there is no packets
      from the gateway for a specified long time.

  Parameters:

      None

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode send_join_request();

/*
  handle_join_response:

      This function parses the payload of join_request_ack response returned
      from the gateway to get public network address of this LBeacon. The
      network address is saved into g_config struct for further uses in
      communicating with the gateway.

  Parameters:

      resp_payload - message buffer to contain the payload of response data
      join_status - pointer to an enumerate variable to store the join result 
                    from Gateway

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode handle_join_response(char *resp_payload, JoinStatus *join_status);

/*
  handle_tracked_object_data:

      This function consolidates all the BLE and BR_EDR devices information
      and sends the information to the gateway.

  Parameters:

      None

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode handle_tracked_object_data();

/*
  handle_health_report:

      This function reads the Health_Report.log and send its content to the
      gateway.

  Parameters:

      None

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode handle_health_report();

/*
  manage_communication:

      This function waits for polling message from the gateway and process
      the corresponding polling types. When there is no polling from
      gateway for long time, this function submits join_request to
      gateway again to re-establish the connection between the gateway.

  Parameters:

      param - not used. This parameter is defined to meet the definition of
              pthread_create() function

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode *manage_communication(void *param);

/*
  copy_object_data_to_file:

      This function copies the data on tracked objects captured in the
      specifed tracked object list to file to be transferred to gateway. The
      output file contains for each ScannedDevice struct found in the list,
      the MAC address and the initial and final timestamps.

  Parameters:

      file_name - name of the file containing data stored in all
                  ScannedDevice struct found in specified tracked object
                  list.
      list - head of the tracked object list from which data is to be
             copied.
      max_num_objects - the maximum number of objects to be consolidated at
                        one time
      used_objects - used for return value to let caller know how many
                     objects were consolidated by this function.

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode copy_object_data_to_file(char *file_name,
                                   ObjectListHead *list,
                                   const int max_num_objects,
                                   int *used_objects);

/*
  consolidate_tracked_data:

      This function places the data on tracked objects captured in the
      specifed tracked object list into a message buffer. The output message
      buffer contains for each ScannedDevice struct found in the list, the MAC
      address and the initial and final timestamps.

  Parameters:

      list - head of the tracked object list from which data is to be
             copied.

      msg_buf - message buffer to contain the consolidated data

      msg_size - size of msg_buf in number of bytes

      max_num_objects - the maximum number of objects whose data are to be
                        consolidated at one time
      used_objects - the actual number of objects whose data were moved into
                     the message buffer
  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode consolidate_tracked_data(ObjectListHead *list,
                                   char *msg_buf, size_t msg_size,
                                   const int max_num_objects,
                                   int *used_objects);

/*
  ble_hci_request:

      This function prepares the bluetooh BLE request specified by the input
      parameters

  Parameters:

      ocf - an argument used by bluetooth BLE request
      clen - an argument used by bluetooth BLE request
      status - an argument used by bluetooth BLE request
      cparam - an argument used by bluetooth BLE request

  Return value:

      struct hci_request - the bluetooth BLE request specified by the input
                           parameters
*/

const struct hci_request ble_hci_request(uint16_t ocf,
                                         int clen,
                                         void * status,
                                         void * cparam);

/*
  eir_parse_specific_data:

      This function parses the sepcific data from bluetooth BLE device

  Parameters:

      eir - the data member of the advertising information result
            from bluetooth BLE scan result
      eir_len - the length in number of bytes of the eir argument
      buf - the output buffer to receive the parsed result
      buf_len - the length in number of bytes of the buf argument

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

static ErrorCode eir_parse_specific_data(uint8_t *eir,
                                         size_t eir_len,
                                         char *buf,
                                         size_t buf_len);


/*
  examine_scanned_ble_device:

      This function extracted scanned BLE devices information from the 
      temp_ble_device_list and compares the scanned attributes with BLE
      scanning criteria. To reduce the traffic within BeDIS system, this 
      function only tracks the tags with the specific prefix MAX address. 
      When a tag with specific prefix MAC address is found, this function 
      calls send_to_push_dongle to either add a new ScannedDevice struct 
      of the device to ble_object_list or update the final scan time of a 
      struct in the list.
      [N.B. This function is executed by the main thread. ]

  Parameters:

      param - not used. This parameter is defined to meet the definition of
              pthread_create() function

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode *examine_scanned_ble_device(void *param);

/*
  start_ble_scanning:

      This function scans continuously for BLE bluetooth devices under the
      coverage of the beacon until scanning is cancelled. 
      [N.B. This function is executed by the main thread. ]

  Parameters:

      param - not used. This parameter is defined to meet the definition of
              pthread_create() function

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode *start_ble_scanning(void *param);

ErrorCode *start_br_scanning(void *param);

/*
  cleanup_lists:

      This function first removes every node from the specified list. If the
      node is also linked by other lists, this function removes the node from
      the lists as well. Once the node is isolated, this function calls memory
      pool to release memory used by the node.

  Parameters:

      list_head - the head of a specified list.
      is_scanned_list - a flag indicating whether the input list is the
                        scanned list.

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode cleanup_lists(ObjectListHead *list_head, bool is_scanned_list_head);

/*
  timeout_cleanup:

      This function is event driven. When it is time to clean up all lists,
      this function cleans up all scanned list, BR object list, and BLE object
      list.

  Parameters:

      param - not used. This parameter is defined to meet the definition of
              pthread_create() function

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode *timeout_cleanup(void *param);

/*
  cleanup_exit:

      This function releases all the resources.

  Parameters:

      None

  Return value:

      ErrorCode - The error code for the corresponding error if the function
                  fails or WORK SUCCESSFULLY otherwise
*/

ErrorCode cleanup_exit();


/*
  Wifi_init:

     This function initializes the Wifi's objects.

  Parameters:

     None

  Return value:

     ErrorCode - The error code for the corresponding error or successful

 */
ErrorCode Wifi_init();


/*
  Wifi_free:

     When called, this function frees the queue of the Wi-Fi pkts and sockets.

  Parameters:

     None

  Return value:

     None

*/
void Wifi_free();

/*
  EXTERNAL FUNCTIONS
*/

/*
  opendir:

      This function is called to open a specified directory.

  Parameters:

      dirname - the name of the directory to be opened.

  Return value:

      dirp - a pointer to the directory stream.
*/

extern DIR *opendir(const char *dirname);

/*
  memset:

      This function is called to fill a block of memory.

  Parameters:

      ptr - the pointer points to the memory area
      value - the int value passed to the function which fills the blocks of
              memory using unsinged char convension of this value
      number - number of bytes in the memory area starting from ptr to be
               filled

  Return value:

      dst - a pointer to the memory area
*/

extern void * memset(void * ptr, int value, size_t number);

/*
  hci_open_dev:

      This function is called to open a Bluetooth socket with the specified
      resource number.

  Parameters:

      dev_id - the id of the Bluetooth socket device

  Return value:

      dd - device descriptor of the Bluetooth socket
*/

extern int hci_open_dev(int dev_id);

/*
  hci_filter_clear:

      This function is called to clear a specified filter.

  Parameters:

      f - the filter to be cleared

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
      to -

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

/* Functions for communication via BR/EDR path to Bluetooth
   classic devices */
#ifdef Bluetooth_classic

/*
  obexftp_open:

      This function is called to create an obexftp client.

  Parameters:

      transport - the transport protocol that will be used
      ctrans - optional custom transport protocol
      infocb - optional info callback
      infocb_data - optional info callback data

  Return value:

      cli - a new allocated ObexFTP client instance, or NULL on error.
*/

extern obexftp_client_t * obexftp_open(int transport,
                                       obex_ctrans_t *ctrans,
                                       obexftp_info_cb_t infocb,
                                       void *infocb_data);

/*
  xbee_send_data:

      When called, this function sends a packet containing the specified
      message to the gateway via xbee module.

  Parameters:

    message - the message to be sent via xbee module

  Return value:

      None

*/

extern void *xbee_send_data(char *message);

/*
  choose_file:

    This function receives the name of a message file and returns the file
    path where the message is located. It goes through each directory in the
    messages folder and in each category, reads each file name to find
    the designated message to be broadcast to the users under the beacon.

  Parameters:

    message_to_send - name of the message file we want to retrieve

  Return value:

    return_value - message file path
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

/*
  start_classic_pushing:

    This function creates threads per devices to push the data or file to
    the scanned classic Bluetooth devices via BR/EDR path.

    [N.B. The code in this function was orignally put in the main function]

  Parameters:

    None

  Return value:

    None
*/

void start_classic_pushing(void);

#endif // Bluetooth_classic

#endif
