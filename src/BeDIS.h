/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and cnditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIS

  File Name:

     BeDIS.h

  File Description:

     This file contains the definitions and declarations of constants,
     structures, and functions used in both Gateway and LBeacon.

  Version:

     2.0, 20190201

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

 */

#ifndef BEDIS_H
#define BEDIS_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <dirent.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/file.h>
#include "Mempool.h"
#include "UDP_API.h"
#include "LinkedList.h"
#include "thpool.h"
#include "zlog.h"

/* Parameter that marks the start of the config file */
#define DELIMITER "="

/* Parameter that marks the start of fracton part of float number */
#define FRACTION_DOT "."

/* Maximum number of characters in each line of config file */
#define CONFIG_BUFFER_SIZE 64

/* Number of times to retry open file, because file openning operation may have
   transient failure. */
#define FILE_OPEN_RETRY 5

/* Number of times to retry getting a dongle, because this operation may have
   transient failure. */
#define DONGLE_GET_RETRY 5

/* Number of times to retry opening socket, because socket openning operation
   may have transient failure. */
#define SOCKET_OPEN_RETRY 5

/* The number of slots in the memory pool */
#define SLOTS_IN_MEM_POOL 1024

/* Length of the IP address in byte */
#define NETWORK_ADDR_LENGTH 16

/* Length of the IP address in Hex */
#define NETWORK_ADDR_LENGTH_HEX 8

/* Maximum length of message to be sent over WiFi in bytes */
#define WIFI_MESSAGE_LENGTH 4096

/* Minimum Wi-Fi message size (One byte for data type and one byte for a space) */
#define MINIMUM_WIFI_MESSAGE_LENGTH 2

/* The size of array to store Wi-Fi SSID */
#define WIFI_SSID_LENGTH 10

/* The size of array to store Wi-Fi Password */
#define WIFI_PASS_LENGTH 10

/* Length of the LBeacon's UUID in number of characters */
#define UUID_LENGTH 32

/* Length of coordinates in number of bits */
#define COORDINATE_LENGTH 64

/* The port on which to listen for incoming data */
#define UDP_LISTEN_PORT 8888

/* Number of bytes in the string format of epoch time */
#define LENGTH_OF_EPOCH_TIME 11

/* Time interval in micro seconds for busy-wait checking in threads */
#define INTERVAL_FOR_BUSY_WAITING_CHECK_IN_MICRO_SECONDS 500000

/* Time interval in seconds for reconnect to Gateway */
#define INTERVAL_FOR_RECONNECT_GATEWAY_IN_SEC 120

/* Timeout in seconds for UDP recevie socket */
#define TIMEOUT_UDP_RECEIVCE_IN_SEC 5


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
    E_ADVERTISE_STATUS = 15,
    E_ADVERTISE_MODE = 16,
    E_SET_BLE_PARAMETER = 17,
    E_BLE_ENABLE = 18,
    E_GET_BLE_SOCKET = 19,
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
    E_SQL_PARSE = 38,
    E_SQL_RESULT_EXCEED = 39,
    E_PARSE_UUID = 40,
    MAX_ERROR_CODE = 41

} ErrorCode;

typedef struct {
    ErrorCode code;
    char *message;
} errordesc;

typedef enum _HealthReportErrorCode{
    S_NORMAL = 0,
    E_ERROR = 1
} HealthReportErrorCode;

typedef struct coordinates{

    char X_coordinates[COORDINATE_LENGTH];
    char Y_coordinates[COORDINATE_LENGTH];
    char Z_coordinates[COORDINATE_LENGTH];

} Coordinates;


typedef enum pkt_types {
    /* Unknown type of pkt type */
    undefined = 0,

    /* For Join Request */

    /* Request join from LBeacon */
    request_to_join = 1,
    /* When Gateway accept LBeacon join request */
    join_request_ack = 2,
    /* When Gateway deny Beacon join request */
    join_request_deny = 3,

    /* For LBeacon send pkt type */

    /* A pkt containing tracked object data */
    tracked_object_data = 4,
    /* A pkt containing health report */
    health_report = 5,
    /* A pkt for LBeacon */
    data_for_LBeacon = 6,

    /* For Gayeway */

    /* For the Gateway polling health reports from LBeacons */
    RFHR_from_gateway = 8,

    /* For server */

    /* For the Gateway polling tracked object data from LBeacons */
    poll_for_tracked_object_data_from_server = 9,
    /* A polling request for health report from server */
    RFHR_from_server = 10

} PktType;


typedef enum pkt_direction {
    /* pkt from gateway */
    from_gateway = 10,
    /* pkt from server */
    from_server = 8,
    /* pkt from beacon */
    from_beacon = 0

} PktDirection;


/* Type of device to be tracked. */
typedef enum DeviceType {

    BR_EDR = 0,
    BLE = 1,
    max_type = 2

} DeviceType;


/* A global flag that is initially set to true by the main thread. It is set
   to false by any thread when the thread encounters a fatal error,
   indicating that it is about to exit. In addition, if user presses Ctrl+C,
   the ready_to_work will be set as false to stop all threadts. */
bool ready_to_work;

/* The pointer to the category of the log file */
zlog_category_t *category_health_report, *category_debug;


/* FUNCTIONS */

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
  trim_string_tail:

     Trim the whitespace, newline and carry-return at the end of string when
     reading config messages.

  Parameters:

     message - the character array of input string

  Return value:

     None
 */
void trim_string_tail(char *message);


/*
  ctrlc_handler:

     If the user presses CTRL-C, the global variable ready_to_work will be set
     to false, and a signal will be thrown to stop running the program.

  Parameters:

     stop - A interger signal triggered by ctrl-c.

  Return value:

     None

 */
void ctrlc_handler(int stop);


/*
  startThread:

     This function initializes the specified thread. Threads initialized by
     this function will be create in detached mode.

  Parameters:

     thread        - The pointer of the thread.
     start_routine - routine to be executed by the thread.
     arg           - the argument for the start_routine.

  Return value:

     Error_code: The error code for the corresponding error
 */
ErrorCode startThread(pthread_t *thread, void *( *start_routine)(void *),
                      void *arg);


/*
  get_system_time:

     This helper function fetches the current time according to the system
     clock in terms of the number of seconds since January 1, 1970.

  Parameters:

     None

  Return value:

     system_time - system time in seconds
*/
int get_system_time();


/*
  memset:

      This function is called to fill a block of memory.

  Parameters:

     ptr    - the pointer to the block memory to fill
     value  - The value in int type: The function will fill the memory
              as if this value
     number - number of bytes in the memory area starting from ptr to be
               set to value

  Return value:

     void * - a pointer points to the memory area
*/
extern void * memset(void * ptr, int value, size_t number);


/*
  pthread_attr_init:

      This function is called to initialize thread attributes object pointed
      to by attr with default attribute values

  Parameters:

      attr - pointer to the thread attributes object to be initialized

  Return value:

      0 for success. error number for error.
*/
extern int pthread_attr_init(pthread_attr_t *attr);


/*
  pthread_attr_destroy:

      This function is called to destroy the thread attributes object
      pointed to by attr

  Parameters:

      attr - the thread attributes object to be destroyed

  Return value:

      0 for success. error number for error.
*/
extern int pthread_attr_destroy(pthread_attr_t *attr);


/*
  pthread_create:

      This function is called to start a new thread in the calling process.
      The new thread starts execution by invoking start_routine.

  Parameters:

      thread - a pointer to the new thread
      attr - set thread properties
      start_routine - routine to be executed by the new thread
      arg - the parameters of the start_routine.

  Return value:

      0 for success. error number for error and the contents of *thread are
      undefined.
*/
extern int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg);


/*
  pthread_detach:

      This function is called to mark the thread identified by thread as
      detached. When a detached thread returns, its resources are
      automatically released back to the system.

  Parameters:

      thread - a thread to be detached

  Return value:

      0 for success. error number for error.
*/
extern int pthread_detach(pthread_t thread);


#endif
