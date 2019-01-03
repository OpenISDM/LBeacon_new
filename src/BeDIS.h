/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and cnditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIS

  File Description:

     This file, contain the definitions and declarations of constant structure.
     and function used in both Gateway and LBeacon.

  File Name:

     BeDIS.h

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
     Johnson Su    , johnsonsu@iis.sinica.edu.tw
     Shirley Huang , shirley.huang.93@gmail.com


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
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <time.h>
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

/* Times of retrying to open file, because file openning operation
is possibily transient failed. */
#define FILE_OPEN_RETRY 5

/* Times of retrying to get dongle, because this operation
is possibily transient failed.*/
#define DONGLE_GET_RETRY 5

/* Times of retrying to open socket, because socket openning operation
is possibily transient failed.*/
#define SOCKET_OPEN_RETRY 5

/* The number of slots in the memory pool */
#define SLOTS_IN_MEM_POOL 1024

/* Length of address of the network */
#define NETWORK_ADDR_LENGTH 16

/* Length of address of the network in Hex */
#define NETWORK_ADDR_LENGTH_HEX 8

/* Maximum length of message to be sent over WiFi in bytes */
#define WIFI_MESSAGE_LENGTH 4096

/* define the size of array to store Wi-Fi SSID */
#define WIFI_SSID_LENGTH 10

/* define the size of array to store Wi-Fi Password */
#define WIFI_PASS_LENGTH 10

/* Length of the Lbeacon's UUID in a number of characters */
#define UUID_LENGTH 32

// Legnth of temporary buffer
#define BUFFER_LENGTH 1024

// Length of coordinates in number of bits
#define COORDINATE_LENGTH 64

//The port on which to listen for incoming data
#define UDP_LISTEN_PORT 8888

/* Number of bytes in the string format of epoch time */
#define LENGTH_OF_EPOCH_TIME 11

/* Time interval in seconds for busy-wait checking in threads */
#define INTERVAL_FOR_BUSY_WAITING_CHECK_IN_SEC 3

/* Timeout interval in seconds */
#define WAITING_TIME 3
#define A_SHORT_TIME 10

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
    E_SCAN_SET_HCI_FILTER = 9,
    E_SCAN_SET_INQUIRY_MODE = 10,
    E_SCAN_START_INQUIRY = 11,
    E_SEND_REQUEST_TIMEOUT = 12,
    E_ADVERTISE_STATUS = 13,
    E_ADVERTISE_MODE = 14,
    E_SET_BLE_PARAMETER = 15,
    E_BLE_ENABLE = 16,
    E_GET_BLE_SOCKET =17,
    E_START_THREAD = 18,
    E_INIT_THREAD_POOL = 19,
    E_INIT_ZIGBEE = 20,
    E_ZIGBEE_CONNECT = 21,
    E_LOG_INIT = 22,
    E_LOG_GET_CATEGORY = 23,
    E_EMPTY_FILE = 24,
    E_INPUT_PARAMETER = 25,
    E_ADD_WORK_THREAD = 26,
    MAX_ERROR_CODE = 27,
    E_INITIALIZATION_FAIL = 28,
    E_WIFI_INIT_FAIL = 29,
    E_ZIGBEE_INIT_FAIL = 30,
    E_XBEE_VALIDATE = 31,
    E_START_COMMUNICAT_ROUTINE_THREAD = 32,
    E_START_BHM_ROUTINE_THREAD = 33,
    E_START_TRACKING_THREAD = 34,
    E_ZIGBEE_CALL_BACK = 35,
    E_ZIGBEE_SHUT_DOWN = 36,
    E_REG_SIG_HANDLER = 37,
    E_JOIN_THREAD = 38,

} ErrorCode;


typedef struct _errordesc {
    ErrorCode code;
    char *message;
} errordesc;

typedef struct coordinates{

    char X_coordinates[COORDINATE_LENGTH];
    char Y_coordinates[COORDINATE_LENGTH];
    char Z_coordinates[COORDINATE_LENGTH];

} Coordinates;

/* A global flag that is initially set to true by the main thread. It is set
   to false by any thread when the thread encounters a fatal error,
   indicating that it is about to exit. In addition, if user presses Ctrl+C,
   the ready_to_work will be set as false to stop all threadts. */
bool ready_to_work;

/* Type of device to be tracked. */
typedef enum DeviceType {

  BR_EDR = 0,
  BLE = 1,
  max_type = 2

} DeviceType;

/* The pointer to the category of the log file */
zlog_category_t *category_health_report, *category_debug;

typedef enum pkt_types {

    undefined = 0,
    request_to_join = 1,
    join_request_ack = 2,
    join_request_deny = 3,
    tracked_object_data = 8,
    health_report = 9,
    data_for_LBeacon = 10,
    poll_for_tracked_object_data = 11,
    RFHR_to_Lbeacons = 12,
    poll_for_RFHR_from_sever = 13

} PktType;

typedef enum pkt_direction {

    from_gateway = 10,
    from_server = 8,
    from_beacon = 0

} PktDirection;

// FUNCTIONS

/*
  uuid_str_to_data:

     @todo

  Parameters:

     uuid - @todo

  Return value:

     data - @todo
 */
unsigned int *uuid_str_to_data(char *uuid);


/*
  twoc:

  @todo

  Parameters:

     in - @todo
     t -  @todo

  Return value:

     data - @todo
 */
unsigned int twoc(int in, int t);

/*
  trim_string_tail:

  Trim the whitespace, newline and carry-return at the end of string

  Parameters:

     message - the character array of input string 

  Return value:

     data - @todo
 */
void trim_string_tail(char *message);

/*
 ctrlc_handler:

     If the user presses CTRL-C, the global variable ready_to_work will be set
     to false, and a signal will be thrown to stop running the program.

 Parameters:

     s - @todo

 Return value:

     None

 */
void ctrlc_handler(int stop);


/*
  startThread:

     This function initializes the specified threads.

  Parameters:

     threads - name of the thread
     thfunct - the function for thread to execute.
     arg - the argument for thread's function

  Return value:

     Error_code: The error code for the corresponding error
 */
ErrorCode startThread(pthread_t *threads, void *( *thfunct)(void *), void *arg);


/*
  get_system_time:

     This helper function fetches the current time according to the system
     clock in terms of the number of seconds since January 1, 1970.

  Parameters:

     None

  Return value:

     system_time - system time in seconds
*/

long long unsigned get_system_time();

/*
  memset:

      This function is called to fill a block of memory.

  Parameters:

      ptr - the pointer to the block memory to fill
      value - The value as an int to be set: The function will fill
              the memory as if this value
      number - number of bytes in the memory area starting from ptr to be
               set to value

  Return value:

      dst - a pointer to the memory area
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
      The new thrad starts execution by invoking start_routine.

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
