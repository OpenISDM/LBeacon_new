/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and cnditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIPS

  File Description:

     In this file, we group all the definition and declarations used in Gateway
     and LBeacon, including.

  File Name:

     BeDIS.h

  Abstract:

     BeDIPS uses LBeacons to deliver 3D coordinates and textual descriptions of
     their locations to users' devices. Basically, a LBeacon is an inexpensive,
     Bluetooth Smart Ready device. The 3D coordinates and location description
     of every LBeacon are retrieved from BeDIS (Building/environment Data and
     Information System) and stored locally during deployment and maintenance
     times. Once initialized, each LBeacon broadcasts its coordinates and
     location description to Bluetooth enabled user devices within its coverage
     area.

  Authors:

     Gary Xiao     , garyh0205@hotmail.com

 */

#ifndef BEDIS_H
#define BEDIS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Parameter that marks the start of the config file */
#define DELIMITER "="

/* Maximum number of characters in each line of config file */
#define CONFIG_BUFFER_SIZE 64

/* Number of lines in the config file */
#define CONFIG_FILE_LENGTH 11

/* Times of retrying to open file, because file openning operation 
is possibily transient failed. */
#define FILE_OPEN_RETRY 5

/* Times of retrying to open socket, because socket openning operation 
is possibily transient failed.*/
#define SOCKET_OPEN_RETRY 5

/* The number of slots in the memory pool */
#define SLOTS_IN_MEM_POOL 1024

/* Length of address of the network */
#define NETWORK_ADDR_LENGTH 16

/* Length of address of the network in Hex */
#define NETWORK_ADDR_LENGTH_HEX 8

/* The maxinum length in bytes of the message to be sent over zigbee link */
#define ZIG_MESSAGE_LENGTH 104

/* Maximum length of message to be sent over WiFi in bytes */
#define WIFI_MESSAGE_LENGTH 4096

#define WIFI_SSID_LENGTH 10

#define WIFI_PASS_LENGTH 10

/* Length of the beacon's UUID in a number of charaters */
#define UUID_LENGTH 32

// Length of coordinates in number of bits
#define COORDINATE_LENGTH 64

//The port on which to listen for incoming data
#define UDP_LISTEN_PORT 8888

/* Timeout interval in seconds */
#define A_LONG_TIME 36000
#define A_SHORT_TIME 6000

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

typedef enum ErrorCode error_t;

/*
typedef struct _errordesc {
    int code;
    char *message;
} s_er_d;

s_er_d errordesc[]; = {

    {WORK_SUCCESSFULLY, "The code works successfullly"},
    {E_MALLOC, "Error allocating memory"},
    {E_OPEN_FILE, "Error opening file"},
    {E_OPEN_DEVICE, "Error opening the dvice"},
    {E_OPEN_SOCKET, "Error opening socket"},
    {E_SEND_OBEXFTP_CLIENT, "Error opening obexftp client"},
    {E_SEND_CONNECT_DEVICE, "Error connecting to obexftp device"},
    {E_SEND_PUSH_FILE, "Error pushing file to device"},
    {E_SEND_DISCONNECT_CLIENT, "Disconnecting the client"},
    {E_SCAN_SET_HCI_FILTER, "Error setting HCI filter"},
    {E_SCAN_SET_INQUIRY_MODE, "Error settnig inquiry mode"},
    {E_SCAN_START_INQUIRY, "Error starting inquiry"},
    {E_SEND_REQUEST_TIMEOUT, "Sending request timeout"},
    {E_ADVERTISE_STATUS, "LE set advertise returned status"},
    {E_ADVERTISE_MODE, "Error setting advertise mode"},
    {E_SET_BLE_PARAMETER, "Error setting parameters of BLE scanning "},
    {E_BLE_ENABLE, "Error enabling BLE scanning"},
    {E_GET_BLE_SOCKET, "Error getting BLE socket options"},
    {E_START_THREAD, "Error creating thread"},
    {E_INIT_THREAD_POOL, "Error initializing thread pool"},
    {E_INIT_ZIGBEE, "Error initializing the zigbee"},
    {E_ZIGBEE_CONNECT, "Error zigbee connection"},
    {E_LOG_INIT, "Error initializing log file"},
    {E_LOG_GET_CATEGORY, "Error getting log category"},
    {E_EMPTY_FILE, "Empty file"},
    {E_INPUT_PARAMETER , "Error of invalid input parameter"},
    {E_ADD_WORK_THREAD, "Error adding work to the work thread"},
    {MAX_ERROR_CODE, "The element is invalid"},
    {E_INITIALIZATION_FAIL, "The Network or Buffer initialization Fail."},
    {E_WIFI_INIT_FAIL, "Wi-Fi initialization Fail."},
    {E_ZIGBEE_INIT_FAIL, "Zigbee initialization Fail."},
    {E_XBEE_VALIDATE, "Zigbee Connection Fail."},
    {E_START_COMMUNICAT_ROUTINE_THREAD, "Start Communocation Thread Fail."},
    {E_START_BHM_ROUTINE_THREAD, "Start BHM THread Fail."},
    {E_START_TRACKING_THREAD, "Start Tracking Thread Fail."},
    {E_ZIGBEE_CALL_BACK, "Error enabling call back function for xbee"},
    {E_ZIGBEE_SHUT_DOWN,  "Error shutting down xbee."},
    {E_REG_SIG_HANDLER, "Error registering signal handler"},
    {E_JOIN_THREAD, "Error joining thread"}

};
*/

typedef struct{

    char X_coordinates[COORDINATE_LENGTH];
    char Y_coordinates[COORDINATE_LENGTH];

    char Z_coordinates[COORDINATE_LENGTH];

} Coordinates;

/* A flag that is used to check if CTRL-C is pressed */
bool g_done;

/* A global flag that is initially set to true by the main thread. It is set
   to false by any thread when the thread encounters a fatal error,
   indicating that it is about to exit. */
bool ready_to_work;


/*
  memset:

      This function is called to fill a block of memory.

  Parameters:

      ptr - the pointer points to the memory area
      value - the constant byte to replace the memory area
      number - number of bytes in the memory area starting from ptr to be
               filled

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

      This function is called to make the thread identified by thread as
      detached. When a detached thread returns, its resources are
      automatically released back to the system.

  Parameters:

      thread - a thread to be detached

  Return value:

      0 for success. error number for error.
*/
extern int pthread_detach(pthread_t thread);


#endif
