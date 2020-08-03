/*
  Copyright (c) BiDaE Technology Inc. All rights reserved.

  License:

    BiDaE SHAREWARE LICENSE
    Version 1.0, 31 July 2020

    Copyright (c) BiDaE Technology Inc. All rights reserved.
    The SOFTWARE implemented in the product is copyrighted and protected by 
    all applicable intellectual property laws worldwide. Any duplication of 
    the SOFTWARE other than for archiving or resetting purposes on the same 
    product without the written agreement from BiDaE Technology could be a 
    violation of law. For the avoidance of doubt, redistribution of this 
    SOFTWARE in source or binary form is prohibited. You agree to prevent 
    any unauthorized copying and redistribution of the SOFTWARE. 

    BiDaE Technology Inc. is the license steward. No one other than the 
    license steward has the right to modify or publish new versions of this 
    License. However, You may distribute the SOFTWARE under the terms of the 
    version of the License under which You originally received the Covered 
    Software, or under the terms of any subsequent version published by the 
    license steward.

    LIMITED WARRANTY:

    BiDaE Technology Inc. or its distributors, depending on which party sold 
    the SOFTWARE, warrants that the media on which the SOFTWARE is installed 
    will be free from defects in materials under normal and purposed use.

    BiDaE Technology Inc. or its distributor warrants, for your benefit alone, 
    that during the Warranty Period the SOFTWARE, shall operate substantially 
    in accordance with the functional specifications in the User's Manual. If, 
    during the Warranty Period, a defect in the SOFTWARE appears, You may 
    obtain a replacement of the SOFTWARE. Any replacement SOFTWARE will be 
    warranted for the remainder of the Warranty Period attached to the product.

    WHEN THE WARRANTY PERIOD HAS BEEN EXPIRED, THIS SOFTWARE IS PROVIDED 
    ''AS IS,'' AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
    PARTICULAR PURPOSE ARE DISCLAIMED. HENCEFORTH, IN NO EVENT SHALL BiDaE 
    TECHNOLOGY INC. OR ITS COLLABORATOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  Project Name:

     BeDIS

  File Name:

     BeDIS.h

  File Description:

     This file contains the definitions and declarations of constants,
     structures, and functions used in server, gateway and Lbeacon.

  Version:

     2.0, 20190617

  Abstract:

     BeDIS uses LBeacons to deliver 3D coordinates and textual descriptions of
     their locations to users' devices. Basically, a LBeacon is an inexpensive,
     Bluetooth Smart Ready device. The 3D coordinates and location description
     of every LBeacon are retrieved from BeDIS (Building/environment Data and
     Information System) and stored locally during deployment and maintenance
     times. Once initialized, each LBeacon broadcasts its coordinates and
     location description to Bluetooth enabled user devices within its coverage
     area.

  Authors:

     Gary Xiao     , garyh0205@hotmail.com
     Joey Zhou     , joeyzhou5566@gmail.com
     Holly Wang    , hollywang@iis.sinica.edu.tw
     Jake Lee      , jakelee@iis.sinica.edu.tw
     Chun Yu Lai   , chunyu1202@gmail.com
     Jia Ying Shi  , littlestone1225@yahoo.com.tw

 */

#ifndef BEDIS_H
#define BEDIS_H

#ifdef _WIN32
 
   #include <winsock2.h>
   #pragma  comment(lib,"WS2_32.lib")
   #include <WS2tcpip.h>
   #include <windows.h>

#elif __unix__

   #include <netdb.h>
   #include <netinet/in.h>
   #include <dirent.h>
   #include <pthread.h>
   #include <arpa/inet.h>
   #include <sys/socket.h>
   #include <sys/poll.h>
   #include <sys/ioctl.h>
   #include <sys/types.h>
   #include <sys/time.h>
   #include <sys/timeb.h>
   #include <sys/file.h>
#else
#   error "Unknown compiler"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <time.h>
#include "Common.h"
#include "Mempool.h"
#include "UDP_API.h"
#include "LinkedList.h"
#include "thpool.h"
#include "zlog.h"


/* Gateway API protocol version for communicate between gateway and Lbeacon. */

#define BOT_GATEWAY_API_VERSION_10 "1.0"

#define BOT_GATEWAY_API_VERSION_11 "1.1"

#define BOT_GATEWAY_API_VERSION_LATEST "1.2"

/* Agent API protocol version for gateway to deploy commands to agent. */

#define BOT_AGENT_API_VERSION_LATEST "1.0"


/* zlog category name */
/* The category of log file used for health report */
#define LOG_CATEGORY_HEALTH_REPORT "Health_Report"

/* The category of log file used for debugging */
#define LOG_CATEGORY_DEBUG "LBeacon_Debug"

/* The category of log file used for dumping tracking data */
#define LOG_CATEGORY_DUMP_TRACKING "Dump_Tracking"

/* Parameter that marks the start of the config file */
#define DELIMITER "="

/* Parameter that marks the separation between records communicated with
   SQL wrapper API */
#define DELIMITER_SEMICOLON ";"

/* Parameter that marks the separation between records */
#define DELIMITER_COMMA ","

/* Parameter that marks the separation between records */
#define DELIMITER_COLON ":"

/* Parameter that marks the separation between records */
#define DELIMITER_DOT "."

/* Maximum number of characters in each line of config file */
#define CONFIG_BUFFER_SIZE 4096

/* Number of characters of API version */
#define LENGTH_OF_API_VERSION 16

/* Number of characters in the uuid of a Bluetooth device */
#define LENGTH_OF_UUID 33

/* Number of characters in a Bluetooth MAC address */
#define LENGTH_OF_MAC_ADDRESS 18

/* Number of bytes in the string format of epoch time */
#define LENGTH_OF_EPOCH_TIME 11

/* Length of the IP address in Hex */
#define NETWORK_ADDR_LENGTH_HEX 8

/* Length of coordinates in number of bits */
#define COORDINATE_LENGTH 64

/* Timeout interval in ms */
#define NORMAL_WAITING_TIME_IN_MS 1000

/* Timeout interval in ms */
#define BUSY_WAITING_TIME_IN_MS 300

/* Timeout interval in ms for busy waiting in processing priority list */
#define BUSY_WAITING_TIME_IN_PRIORITY_LIST_IN_MS 50

/* Timeout interval in ms for busy waiting in receiving wifi packet*/
#define BUSY_WAITING_TIME_IN_WIFI_REXEIVE_PACKET_IN_MS 50

/* Number of times to retry allocating memory, because memory allocating 
   operation may have transient failure. */
#define MEMORY_ALLOCATE_RETRIES 5

/* Maximum number of nodes per star network */
#define MAX_NUMBER_NODES 4096

/* Maximum length of time in seconds low priority message lists are allowed to 
   be starved of attention. */
#define MAX_STARVATION_TIME 600

/* The number of milliseconds of each hour */
#define MS_EACH_HOUR 3600000

/* The index of starting coordinate_x information in lbeacon uuid */
#define INDEX_OF_COORDINATE_X_IN_UUID  12

/* The index of starting coordinate_y information in lbeacon uuid */
#define INDEX_OF_COORDINATE_Y_IN_UUID  24

/* Number of characters for coordinate informatoin within lbeacon uuid */
#define LENGTH_OF_COORDINATE_IN_UUID 8

/* Number of characters for area id informatoin within lbeacon uuid */
#define LENGTH_OF_AREA_ID_IN_UUID 4

/* Number of bytes in the string format of coordinate */
#define LENGTH_OF_COORDINATE 9

/* Number of charactures in the time format of %Y-%m-%d %H:%M:%S */
#define LENGTH_OF_TIME_FORMAT 80

/* Number of objects been monitored in each covered area */
#define NUMBER_OBJECTS_PER_AREA 512 

/* Error code for failure of opening self-check and version files */
#define SELF_CHECK_ERROR_OPEN_FILE 9999

/* Number of times to retry open file, because file openning operation may have
   transient failure. */
#define FILE_OPEN_RETRY 5

typedef enum _ErrorCode{

    WORK_SUCCESSFULLY = 0,
    E_MALLOC = 1,
    E_OPEN_FILE = 2,
    E_OPEN_DEVICE = 3,
    E_OPEN_SOCKET = 4,
    E_SEND_OBEXFTP_CLIENT = 5,
    E_SEND_CONNECT_DEVICE = 6,
    E_SEND_PUSH_FILE = 7,
    E_SEND_DISCONNECT_CLIENT = 8,
    E_SCAN_SET_EVENT_MASK = 9,
    E_SCAN_SET_ENABLE = 10,
    E_SCAN_SET_HCI_FILTER = 11,
    E_SCAN_SET_INQUIRY_MODE = 12,
    E_SCAN_START_INQUIRY = 13,
    E_SEND_REQUEST_TIMEOUT = 14,
    E_BLE_ENABLE = 15,
    E_SET_BLE_PARAMETER = 16,
    E_GET_BLE_SOCKET = 17,
    E_ADVERTISE_STATUS = 18,
    E_ADVERTISE_MODE = 19,
    E_START_THREAD = 20,
    E_INIT_THREAD_POOL = 21,
    E_INIT_ZIGBEE = 22,
    E_LOG_INIT = 23,
    E_LOG_GET_CATEGORY = 24,
    E_EMPTY_FILE = 25,
    E_INPUT_PARAMETER = 26,
    E_ADD_WORK_THREAD = 27,
    E_INITIALIZATION_FAIL = 28,
    E_WIFI_INIT_FAIL = 29,
    E_START_COMMUNICAT_ROUTINE_THREAD = 30,
    E_START_BHM_ROUTINE_THREAD = 31,
    E_START_TRACKING_THREAD = 32,
    E_REG_SIG_HANDLER = 33,
    E_JOIN_THREAD = 34,
    E_BUFFER_SIZE = 35,
    E_PREPARE_RESPONSE_BASIC_INFO = 36,
    E_ADD_PACKET_TO_QUEUE = 37,
    E_SQL_OPEN_DATABASE = 38,
    E_SQL_PARSE = 39,
    E_SQL_RESULT_EXCEED = 40,
    E_SQL_EXECUTE = 41,
    E_API_INITIALLZATION =42,
    E_API_FREE = 43,
    E_MODULE_INITIALIZATION = 44,
    E_PARSE_UUID = 45,
    E_PARSE_JOIN_RESPONSE = 46,
    E_API_PROTOCOL_FORMAT = 47,

    MAX_ERROR_CODE = 48

} ErrorCode;
 
/* Type of join response. */
typedef enum _JoinStatus{
    JOIN_ACK = 0,
    JOIN_DENY = 1,
    JOIN_UNKNOWN = 2,
    MAX_JOINSTATUS = 3
} JoinStatus;

/* Type of health_status to be queried. */
typedef enum _HealthStatus {

    S_NORMAL_STATUS = 0,
    E_ERROR_STATUS = 1,
    MAX_STATUS = 2

} HealthStatus;

/* BitMap of different object monitor types in order to support one object 
   with multiple monitor types. */
typedef enum ObjectMonitorType {

    MONITOR_NORMAL = 0,
    MONITOR_GEO_FENCE = 1,
    MONITOR_PANIC = 2,
    MONITOR_MOVEMENT = 4,
    MONITOR_LOCATION = 8,

} ObjectMonitorType;

/* Type of notification alarms. */
typedef enum AlarmType {

    NO_ALARM = 0,
    ALARM_LIGHT = 1,
    ALARM_SOUND = 2,
    ALARM_LIGHT_SOUND = 3

} AlarmType;

typedef enum AddressMapType {

    ADDRESS_MAP_TYPE_GATEWAY = 0,
    ADDRESS_MAP_TYPE_LBEACON = 1

} AddressMapType;

/* A node of buffer to store received data and/or data to be send */
typedef struct {

    struct List_Entry buffer_entry;

    unsigned int pkt_direction;

    unsigned int pkt_type;
    
    float API_version;

    /* The network address of the packet received or the packet to be sent */
    char net_address[NETWORK_ADDR_LENGTH];

    /* The port from which the packet was received or to be sent */
    unsigned int port;

    /* The pointer points to the content. */
    char content[WIFI_MESSAGE_LENGTH];

    /* The size of the content */
    int content_size;

    /* The uptime at which this buffer is recevied */
    int uptime_at_receive;

} BufferNode;


/* A Head of a list of msg buffers */
typedef struct {

    /* A per list lock */
    pthread_mutex_t list_lock;

    struct List_Entry list_head;

    struct List_Entry priority_list_entry;

    /* The nice relative to the normal priority (i.e. nice = 0) */
    int priority_nice;

    /* The pointer to the function to be called to process buffer nodes in
       the list. */
    void (*function)(void *arg);

    /* The argument of the function */
    void *arg;

} BufferListHead;

/*  A struct for recording the network address and its last update time */
typedef struct {

    char uuid[LENGTH_OF_UUID];

    /* The network address */
    char net_address[NETWORK_ADDR_LENGTH];
    
    char API_version[LENGTH_OF_API_VERSION];
      
} AddressMap;


typedef struct {

    /* A per array lock for the AddressMapArray when read and update data */
    pthread_mutex_t list_lock;

    /* A Boolean array in which ith element records whether the ith address map
       is in use. */
    bool in_use[MAX_NUMBER_NODES];

    int last_reported_timestamp[MAX_NUMBER_NODES];
    
    AddressMap address_map_list[MAX_NUMBER_NODES];

} AddressMapArray;


typedef struct coordinates{

    char X_coordinates[COORDINATE_LENGTH];
    char Y_coordinates[COORDINATE_LENGTH];
    char Z_coordinates[COORDINATE_LENGTH];

} Coordinates;

typedef struct {

    /* The number of worker threads used by the communication unit for sending
      and receiving packets.*/
    int number_worker_threads;
    /* The number of seconds used by CommUnit_routine() to decide whether an 
    old packet is out-of-date 
       packets */
    int min_age_out_of_date_packet_in_sec;

    /* Priority levels at which buffer lists are processed by the worker threads
     */
    int time_critical_priority;
    int high_priority;
    int normal_priority;
    int low_priority;

} CommonConfig;

typedef struct {

    int start_area_index;
    int number_areas;

} AreaSet;


/* Global variables */

/* The struct for storing common information between gateway and server*/
CommonConfig common_config;

/* The struct for storing necessary objects for the Wifi connection */
sudp_config udp_config;

/* The mempool for the buffer node structure to allocate memory */
Memory_Pool node_mempool;

/* The head of a list of buffers of data for tracked object data and 
   health report */
BufferListHead data_receive_buffer_list_head;

/* The head of a list of the return message for the gateway join requests */
BufferListHead NSI_send_buffer_list_head;

/* The head of a list of buffers for return join request status */
BufferListHead NSI_receive_buffer_list_head;

/* The head of a list of buffers holding health reports to be processed and sent
   to the server */
BufferListHead BHM_send_buffer_list_head;

/* The head of a list of buffers holding health reports from LBeacons */
BufferListHead BHM_receive_buffer_list_head;

/* The head of a list of buffers for the buffer list head in the priority 
   order. */
BufferListHead priority_list_head;


/* Flags */

/*
  Initialization of the server components involves network activation that may
  take time. These flags enable each module to inform the main thread when its
  initialization completes.
 */
bool NSI_initialization_complete;
bool CommUnit_initialization_complete;

/* The flag is to identify whether any component fail to initialize */
bool initialization_failed;

/* A global flag that is initially set to true by the main thread. It is set
   to false by any thread when the thread encounters a fatal error,
   indicating that it is about to exit. In addition, if user presses Ctrl+C,
   the ready_to_work flag will be set as false to stop all threads. */
bool ready_to_work;

/* The pointer to the category of the log file */
zlog_category_t *category_health_report, *category_debug;

/* The pointer to the category of the log file */
zlog_category_t *category_dump;


/* FUNCTIONS */

/*
  twoc:

     @todo

  Parameters:

     in - @todo
     t  -  @todo

  Return value:

     data - @todo
 */
unsigned int twoc(int in, int t);


/*
  decimal_to_hex:

     This function translates the input number from demical to hex format 

  Parameters:

     number - the input decimal number

  Return value:

     the hex format of the input number
 */
char decimal_to_hex(int number);


/*
  init_buffer:

     The function fills the attributes of the head of a list of buffers to be 
     processed by another thread, including the function called to process 
     buffer contents, the argument of the function and the priority level at 
     which the function is to be executed.

  Parameters:

     buffer - A pointer to the buffer to be processed.
     buff_id - The index of the buffer for the priority array.
     function - The pointer to the function to process the buffer.
     priority - The priority nice of the thread when processing the buffer.

  Return value:

     None
 */
void init_buffer(BufferListHead *buffer_list_head, void (*function_p)(void *),
                 int priority_nice);


/*
  init_Address_Map:

     This function initialize the head of the AddressMap.

  Parameters:

     address_map - Pointer to head of the AddressMap.

  Return value:

     None
 */
void init_Address_Map(AddressMapArray *address_map);


/*
  is_in_Address_Map:

     This function check whether the input network address is in the AddressMap.

  Parameters:

     address_map - A pointer to the head of the AddressMap.
     type - The type of entries in the AddressMap.
     identifer - The pointer to the IP address or UUID identifer
            

  Return value:

     int: If not find, return -1, else return its array number.
 */
int is_in_Address_Map(AddressMapArray *address_map, 
                      AddressMapType type,
                      char *identifer);

/*
  update_entry_in_Address_Map:

     This function occupies one entry space in address map and copy the 
     input identifer to the newly occupied entry.

  Parameters:

     address_map - A pointer to the head of the AddressMap.
     index - The index of address_map to be occupied
     type - The type of entries in the AddressMap.
     address - The pointer to the IP address
     uuid - The pointer to the UUID 
     API_version - API version used by the device
     
  Return value:

     Error_code: The error code for the corresponding error
 */
ErrorCode update_entry_in_Address_Map(AddressMapArray *address_map,
                                      int index,
                                      AddressMapType type, 
                                      char *address,
                                      char *uuid,
                                      char *API_version);

/*
  update_report_timestamp_in_Address_Map:

     This function updates the last reported timestamp of the input identifer

  Parameters:

     address_map - A pointer to the head of the AddressMap.
     type - The type of entries in the AddressMap.
     identifer - The pointer to the IP address or UUID identifer
            
  Return value:

     Error_code: The error code for the corresponding error
 */
ErrorCode update_report_timestamp_in_Address_Map(AddressMapArray *address_map,
                                                 AddressMapType type, 
                                                 char *identifer);

/*
  release_not_used_entry_from_Address_Map:

     This function releases the out-of-date entries on which the 
     last_reported_timestamp is not updated for long time.

  Parameters:

     address_map - A pointer to the head of the AddressMap.
     tolerance_duration - The time period in which we expected the 
                          last_reported_timestamp to be updated.

  Return value:

     Error_code: The error code for the corresponding error
 */
ErrorCode release_not_used_entry_from_Address_Map(AddressMapArray *address_map,
                                                  int tolerance_duration);
/*
  dump_ip_of_active_entry_from_Address_Map:

     This function dumps the active entries on which the 
     last_reported_timestamp is updated within tolerant duration.

  Parameters:

     filename - file path to store dumped data
     address_map - A pointer to the head of the AddressMap.
     tolerance_duration - The time period in which we expected the 
                          last_reported_timestamp to be updated.

  Return value:

     Error_code: The error code for the corresponding error
 */
ErrorCode dump_ip_of_active_entry_from_Address_Map(char *filename,
                                                   AddressMapArray *address_map,
                                                   int tolerance_duration);
/*
  sort_priority_list:

     The function arranges entries in the priority list in nondecreasing
     order of priority.

  Parameters:

     config - The pointer points to the structure which stored config for
              gateway.
     list_head - The pointer points to the priority list head.

  Return value:

     None
 */
void *sort_priority_list(CommonConfig *common_config, BufferListHead *list_head);

/*
  CommUnit_routine:

     The function is executed by the main thread of the communication unit that
     is responsible for monitoring the prioritized buffer lists containing packet
     to be sent and received. After the NSI module has initialized WiFi 
     networks, It creates work item for each send or received packet and have 
     the work done by a thread from the thread pool.

  Parameters:

     None

  Return value:

     None

 */
void *CommUnit_routine();

/*
  trim_string_tail:

     This function trims whitespace, newline and carry-return at the end of
     the string when reading config messages.

  Parameters:

     message - A pointer to the character array containing the input
               string.

  Return value:

     None
 */
void trim_string_tail(char *message);

/*
  fetch_next_string:

     This function fetches next configuration string from openned configuration 
     file handler.

  Parameters:

     file - The FILE handler of openned configuration file.
     message - A pointer to the character array for storing the output string.
     message_size - The length in number of bytes of the input message buffer 

  Return value:

     None
 */
void fetch_next_string(FILE *file, char *message, size_t message_size);

/*
  ctrlc_handler:

     When the user presses CTRL-C, the function sets the global variable
     ready_to_work to false, and throw a signal to stop running the program.

  Parameters:

     stop - A integer signal triggered by ctrl-c.

  Return value:

     None

 */
void ctrlc_handler(int stop);

/*
  strncasecmp:

     This function compares two input strings specified by input parameters.

  Parameters:

     str_a - the first string to be compared
     str_b - the second string to be comapred
     len - number of characters in the strings to be compared

  Return value:
     0: if the two strings exactly match
 */

int strncasecmp(char const *str_a, char const *str_b, size_t len);

/*
  strtolowercase:

     This function translates string to lower case

  Parameters:

     source_str - the original string to be translate

     buf - the output buffer to store the result string with lower case
     
     buf_len - number of characters in the size of buf

  Return value:
     0: if the two strings exactly match
 */

ErrorCode strtolowercase(char const * source_str, char * buf, size_t buf_len);

/*
  startThread:

     This function initializes the specified thread. Threads initialized by
     this function will be create in detached mode.

  Parameters:

     thread        - A pointer of the thread.
     start_routine - Routine to be executed by the thread.
     arg           - The argument for the start_routine.

  Return value:

     Error_code: The error code for the corresponding error
 */
ErrorCode extern startThread(pthread_t *thread, void *( *start_routine)(void *),
                             void *arg);

/*
  strtok_save:
     
     This function breaks a specified string into a series of tokens using the 
     delimiter delim.
     
     Windows uses strtok_s()
     Linux uses strtok_r()

  Parameters:

     str     - A pointer to the string to be modified and broken into smaller    
               strings (tokens).
     delim   - The C string containing the delimiters.
     saveptr - A pointer to the next char of the searched character.

  Return value:

      Return a pointer to the next token, or NULL if there are no more tokens.

 */
char *strtok_save(char *str, char *delim, char **saveptr);


/*
  get_system_time:

     This helper function fetches the current time according to the system
     clock in terms of the number of seconds since January 1, 1970.

  Parameters:

     None

  Return value:

     system_time - system time in seconds
*/
int extern get_system_time();

/*
  get_clock_time:

     This helper function gets the monotonic time.

  Parameters:

     None

  Return value:

     int - uptime of MONOTONIC time in seconds
*/
int extern get_clock_time();

/*
  display_time:

     The function gets the current date time and displays the current date time
     in debug logs.

  Parameters:

     None

  Return value:

     None

 */
int display_time(void);

/*
  sleep_t:

     The function is used to sleep the program between different operating system. 

  Parameters:

     wait_time - sleep time in seconds

  Return value:

     None
*/
void sleep_t(int wait_time);


#endif