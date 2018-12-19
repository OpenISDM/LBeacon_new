#include "BeDIS.h"

errordesc ErrorDesc [] = {

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
    {E_ZIGBEE_SHUT_DOWN,  "Error shutting down xbee."}

};

unsigned int *uuid_str_to_data(char *uuid) {
    char conversion[] = "0123456789ABCDEF";
    int uuid_length = strlen(uuid);
    unsigned int *data =
        (unsigned int *)malloc(sizeof(unsigned int) * uuid_length);

    if (data == NULL) {
        /* Error handling */
        perror("Failed to allocate memory");
        return NULL;
    }

    unsigned int *data_pointer = data;
    char *uuid_counter = uuid;

    for (; uuid_counter < uuid + uuid_length;

         data_pointer++, uuid_counter += 2) {
        *data_pointer =
            ((strchr(conversion, toupper(*uuid_counter)) - conversion) * 16) +
            (strchr(conversion, toupper(*(uuid_counter + 1))) - conversion);

    }

    return data;
}


unsigned int twoc(int in, int t) {

    return (in < 0) ? (in + (2 << (t - 1))) : in;
}


void ctrlc_handler(int stop) { ready_to_work = false; }


ErrorCode startThread(pthread_t *threads ,void *( *thfunct)(void *), void *arg){

    pthread_attr_t attr;

    if ( pthread_attr_init( &attr) != 0
      || pthread_create(threads, &attr, thfunct, arg) != 0){

          printf("Start Thread Error.\n");
          return E_START_THREAD;
    }

    printf("Start Thread Success.\n");
    return WORK_SUCCESSFULLY;

}


long long get_system_time() {
    /* A struct that stores the time */
    struct timeb t;

    /* Return value as a long long type */
    long long system_time;

    /* Convert time from Epoch to time in milliseconds of a long long type */
    ftime(&t);
    //system_time = 1000 * t.time + t.millitm;  //millisecond ver.
    system_time = t.time;

    return system_time;
}
