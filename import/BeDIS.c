/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and cnditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIS

  File Name:

     BeDIS.c

  File Description:

     This file contains code of functions used in both gateway and Lbeacon.

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

#include "BeDIS.h"


unsigned int twoc(int in, int t) 
{
    return (in < 0) ? (in + (2 << (t - 1))) : in;
}

char decimal_to_hex(int number)
{
    char c;
    
    if(number <= 9)
        c = number +'0';
    else
        c = (number - 10) + 'A';
    
    return c;
}

void init_buffer(BufferListHead *buffer_list_head, void (*function_p)(void *),
                 int priority_nice)
{
    init_entry( &(buffer_list_head -> list_head));

    init_entry( &(buffer_list_head -> priority_list_entry));

    pthread_mutex_init( &buffer_list_head -> list_lock, 0);

    buffer_list_head -> function = function_p;

    buffer_list_head -> arg = (void *) buffer_list_head;

    buffer_list_head -> priority_nice = priority_nice;
}


void init_Address_Map(AddressMapArray *address_map)
{
    int n;

    pthread_mutex_init( &address_map -> list_lock, 0);

    memset(address_map -> address_map_list, 0,
           sizeof(address_map -> address_map_list));

    for(n = 0; n < MAX_NUMBER_NODES; n ++)
        address_map -> in_use[n] = false;
}


int is_in_Address_Map(AddressMapArray *address_map, 
                      AddressMapType type, 
                      char *identifer)
{
    int n;
    if (type == ADDRESS_MAP_TYPE_GATEWAY)
    {
 
        for(n = 0;n < MAX_NUMBER_NODES;n ++)
        {
            if (address_map -> in_use[n] == true && 
                strncmp(address_map -> address_map_list[n].net_address, 
                identifer, NETWORK_ADDR_LENGTH)== 0)
            {
                    return n;
            }
        }
    }
    else if(type == ADDRESS_MAP_TYPE_LBEACON)
    {
        for(n = 0;n < MAX_NUMBER_NODES; n ++){
            if (address_map -> in_use[n] == true && 
                strncmp(address_map -> address_map_list[n].uuid, 
                identifer, LENGTH_OF_UUID) == 0)
            {
                zlog_debug(category_debug,
                            "uuid matached n=%d [%s] [%s] [%d]\n", 
                            n, 
                            address_map->address_map_list[n].uuid, 
                            identifer, 
                            LENGTH_OF_UUID);
                    return n;
                   
            }
        }
    }
    return -1;
}

ErrorCode update_entry_in_Address_Map(AddressMapArray *address_map,
                                      int index,
                                      AddressMapType type,
                                      char *address,
                                      char *uuid,
                                      char *API_version)
{
    int current_time = get_system_time();

    address_map -> in_use[index] = true;
    address_map -> last_reported_timestamp[index] = current_time;
    memset(address_map->address_map_list[index].API_version, 0,
           LENGTH_OF_API_VERSION);
    strncpy(address_map->address_map_list[index].API_version, 
            API_version, strlen(API_version));

    if(type == ADDRESS_MAP_TYPE_GATEWAY){
        
        memset(address_map->address_map_list[index].net_address, 0, 
               NETWORK_ADDR_LENGTH);

        strncpy(address_map->address_map_list[index].net_address, 
                address, strlen(address));

    }else if(type == ADDRESS_MAP_TYPE_LBEACON){
        
        memset(address_map->address_map_list[index].uuid, 0, 
               LENGTH_OF_UUID);
        strncpy(address_map->address_map_list[index].uuid, 
                uuid, strlen(uuid));         

        memset(address_map->address_map_list[index].net_address, 0, 
               NETWORK_ADDR_LENGTH);

        strncpy(address_map->address_map_list[index].net_address, 
                address, strlen(address));
    }

    return WORK_SUCCESSFULLY;
}

ErrorCode update_report_timestamp_in_Address_Map(AddressMapArray *address_map,
                                                 AddressMapType type, 
                                                 char *identifer)
{
    int index = -1;
    int current_time = get_system_time();

    index = is_in_Address_Map(address_map, type, identifer);

    if(index != -1){
        
        address_map -> last_reported_timestamp[index] = current_time;

    }

    return WORK_SUCCESSFULLY;

}

ErrorCode release_not_used_entry_from_Address_Map(AddressMapArray *address_map,
                                                  int tolerance_duration)
{
    int i;
    int current_time = get_system_time();

    for(i = 0;i < MAX_NUMBER_NODES;i ++)
    {
        if (address_map -> in_use[i] == true && 
            (current_time - address_map ->last_reported_timestamp[i] > 
             tolerance_duration)){

            address_map -> in_use[i] = false;
            printf("release index [%d], net_address [%s], uuid [%s]\n",
                   i, 
                   address_map->address_map_list[i].net_address, 
                   address_map->address_map_list[i].uuid);
        }
    }

    return WORK_SUCCESSFULLY;
}

ErrorCode dump_ip_of_active_entry_from_Address_Map(char *filename,
                                                   AddressMapArray *address_map,
                                                   int tolerance_duration){
                                                       
    int i;
    int current_time = get_system_time();
    int retry_times = 0;
    FILE *active_file = NULL;
    
    retry_times = FILE_OPEN_RETRY;
    while(retry_times--){
        active_file =
        fopen(filename, "w");

        if(NULL != active_file){
            break;
        }
    }
    
    if(NULL == active_file)
        return E_OPEN_FILE;
    
    for(i = 0;i < MAX_NUMBER_NODES;i ++)
    {
        if (address_map -> in_use[i] == true && 
            (current_time - address_map ->last_reported_timestamp[i] < 
             tolerance_duration)){

            fputs(address_map->address_map_list[i].net_address,
                  active_file);
        }
    }
    
    fclose(active_file);
    
    return WORK_SUCCESSFULLY;                                                      
}

void *sort_priority_list(CommonConfig *common_config, BufferListHead *list_head){

    List_Entry *list_pointer,
               *next_list_pointer;

    List_Entry critical_priority_head, high_priority_head,
               normal_priority_head, low_priority_head;

    BufferListHead *current_head, *next_head;

    init_entry( &critical_priority_head);
    init_entry( &high_priority_head);
    init_entry( &normal_priority_head);
    init_entry( &low_priority_head);

    pthread_mutex_lock( &list_head -> list_lock);

    list_for_each_safe(list_pointer, next_list_pointer,
                       &list_head -> priority_list_entry){

        remove_list_node(list_pointer);

        current_head = ListEntry(list_pointer, BufferListHead,
                                 priority_list_entry);

        if(current_head -> priority_nice == common_config->time_critical_priority)

            insert_list_tail( list_pointer, &critical_priority_head);

        else if(current_head -> priority_nice == common_config->high_priority)

            insert_list_tail( list_pointer, &high_priority_head);

        else if(current_head -> priority_nice == common_config->normal_priority)

            insert_list_tail( list_pointer, &normal_priority_head);

        else if(current_head -> priority_nice == common_config->low_priority)

            insert_list_tail( list_pointer, &low_priority_head); 
    }

    if(is_entry_list_empty(&critical_priority_head) == false){
        list_pointer = critical_priority_head.next;
        remove_list_node(list_pointer -> prev);
        concat_list( &list_head -> priority_list_entry, list_pointer);
    }

    if(is_entry_list_empty(&high_priority_head) == false){
        list_pointer = high_priority_head.next;
        remove_list_node(list_pointer -> prev);
        concat_list( &list_head -> priority_list_entry, list_pointer);
    }

    if(is_entry_list_empty(&normal_priority_head) == false){
        list_pointer = normal_priority_head.next;
        remove_list_node(list_pointer -> prev);
        concat_list( &list_head -> priority_list_entry, list_pointer);
    }

    if(is_entry_list_empty(&low_priority_head) == false){
        list_pointer = low_priority_head.next;
        remove_list_node(list_pointer -> prev);
        concat_list( &list_head -> priority_list_entry, list_pointer);
    }

    pthread_mutex_unlock( &list_head -> list_lock);

}

void *CommUnit_routine()
{
    /* The last reset time */
    int init_time;

    int uptime;

    Threadpool thpool;

    int return_error_value;

    /* A flag to indicate whether any buffer nodes were processed in 
    this iteration of the while loop */
    bool did_work;

    /* The pointer to the current priority buffer list entry */
    List_Entry *current_entry, *list_entry;

    /* The pointer to the current node in the list */
    BufferNode *current_node;

    /* The pointer to the current buffer list head */
    BufferListHead *current_head;

    /* wait for NSI get ready */
    while(NSI_initialization_complete == false)
    {
        sleep_t(BUSY_WAITING_TIME_IN_MS);
        if(initialization_failed == true)
        {
            return (void *)NULL;
        }
    }
#ifdef debugging
    zlog_info(category_debug,"[CommUnit] thread pool Initializing");
#endif
    /* Initialize the threadpool with specified number of worker threads
       according to the data stored in the configuration file. */
    thpool = thpool_init(common_config.number_worker_threads);

#ifdef debugging
    zlog_info(category_debug, "[CommUnit] thread pool Initialized");
#endif

    uptime = get_clock_time();

    /* Set the initial time. */
    init_time = uptime;

    /* All the buffers lists have been initialized and the thread pool
       initialized. Set the flag to true. */
    CommUnit_initialization_complete = true;

    /* When there is no dead thead, continue to work. */
    while(ready_to_work == true)
    {
        uptime = get_clock_time();

        /* In the normal situation, the scanning starts from the high priority
           to lower priority. When the timer expired for MAX_STARVATION_TIME,
           reverse the scanning order */
        while(ready_to_work == true && 
              uptime - init_time < MAX_STARVATION_TIME)
        {
            /* Scan the priority_list to get the buffer list with the highest
               priority among all lists that are not empty. */
            
            did_work = false;
            pthread_mutex_lock( &priority_list_head.list_lock);

            list_for_each(current_entry,
                          &priority_list_head.priority_list_entry)
            {
                current_head = ListEntry(current_entry, BufferListHead,
                                         priority_list_entry);

                pthread_mutex_lock( &current_head -> list_lock);

                if (is_entry_list_empty( &current_head->list_head) == true)
                {
                    pthread_mutex_unlock( &current_head -> list_lock);
                    /* Go to check the next buffer list in the priority list */

                    continue;
                }
                else 
                {
                    list_entry = current_head -> list_head.next;

                    remove_list_node(list_entry);

                    pthread_mutex_unlock( &current_head -> list_lock);

                    current_node = ListEntry(list_entry, BufferNode,
                                             buffer_entry);

                    if(uptime - current_node->uptime_at_receive > 
                       common_config.min_age_out_of_date_packet_in_sec){

                       mp_free(&node_mempool, current_node);
                       continue;
                    } 
                    /* Have a worker thread execute the function specified by the 
                    function pointer to do the work */
                    return_error_value = thpool_add_work(thpool,
                                                         current_head -> function,
                                                         current_node,
                                                         current_head ->
                                                         priority_nice);
                    did_work = true;
                    break;
                }
            }
            uptime = get_clock_time();
            pthread_mutex_unlock( &priority_list_head.list_lock);
            
            if(did_work == false){
                sleep_t(BUSY_WAITING_TIME_IN_PRIORITY_LIST_IN_MS);
            }
        }

        /* Scan the priority list in reverse order to prevent starving the
           lowest priority buffer list. */

        pthread_mutex_lock( &priority_list_head.list_lock);

        
        /*  In starvation scenario, we still need to process time-critial buffer 
            lists first. We first process Geo_fence_alert_buffer_list and 
            then Geo_fence_receive_buffer_list, and finally reversely 
            traverse the priority list.
        */
        list_for_each(current_entry, &priority_list_head.priority_list_entry)
        {
            current_head = ListEntry(current_entry, BufferListHead,
                                     priority_list_entry);

            if(current_head -> priority_nice 
               != common_config.time_critical_priority){

                break;
            }
            
            pthread_mutex_lock( &current_head -> list_lock);

            if (is_entry_list_empty( &current_head->list_head) == true)
            {
                pthread_mutex_unlock( &current_head -> list_lock);

                continue;
            }
            else 
            {
                list_entry = current_head -> list_head.next;

                remove_list_node(list_entry);

                pthread_mutex_unlock( &current_head -> list_lock);

                current_node = ListEntry(list_entry, BufferNode,
                                             buffer_entry);

                return_error_value = thpool_add_work(thpool,
                                                     current_head -> function,
                                                     current_node,
                                                     current_head ->
                                                     priority_nice);
                did_work = true;
                break;
            }
        }

        // reversly traverse the priority list
        list_for_each_reverse(current_entry,
                              &priority_list_head.priority_list_entry)
        {
            current_head = ListEntry(current_entry, BufferListHead,
                                     priority_list_entry);

            pthread_mutex_lock( &current_head -> list_lock);

            if (is_entry_list_empty( &current_head->list_head) == true)
            {
                pthread_mutex_unlock( &current_head -> list_lock);

                continue;
            }
            else 
            {
                list_entry = current_head -> list_head.next;

                remove_list_node(list_entry);

                pthread_mutex_unlock( &current_head -> list_lock);

                current_node = ListEntry(list_entry, BufferNode,
                                         buffer_entry);

                /* Call the function pointed to by the function pointer to do 
                   the work */
                return_error_value = thpool_add_work(thpool,
                                                     current_head -> function,
                                                     current_node,
                                                     current_head ->
                                                     priority_nice);
                did_work = true;
            }
        }

        /* Update the init_time */
        init_time = get_clock_time();

        pthread_mutex_unlock( &priority_list_head.list_lock);

        /* If during this iteration of while loop no work were done, 
           sleep before starting the next iteration */
        if(did_work == false)
        {
            sleep_t(BUSY_WAITING_TIME_IN_PRIORITY_LIST_IN_MS);
        }

    } /* End while(ready_to_work == true) */

    
    /* Destroy the thread pool */
    thpool_destroy(thpool);

    return (void *)NULL;
}


void trim_string_tail(char *message) 
{
    int idx = 0;

    /* discard the whitespace, newline, carry-return characters at the end */
    if(strlen(message) > 0){

        idx = strlen(message) - 1;
        while(10 == message[idx] ||
                13 == message[idx] ||
                32 == message[idx]){

           message[idx] = '\0';
           idx--;
        }
    }
}

void fetch_next_string(FILE *file, char *message, size_t message_size)
{
    char  config_setting[CONFIG_BUFFER_SIZE];
    char  *config_message = NULL;
        
    fgets(config_setting, sizeof(config_setting), file);
    config_message = strstr((char *)config_setting, DELIMITER);
    config_message = config_message + strlen(DELIMITER);
    trim_string_tail(config_message);
    
    memset(message, 0, message_size);
    strcpy(message, config_message);
}

void ctrlc_handler(int stop) { ready_to_work = false; }

int strncmp_caseinsensitive(char const *str_a, char const *str_b, size_t len)
{
    int index = 0;
    int diff = 0;

    for(index = 0; index < len; index++)
    {
        diff = tolower((unsigned char)str_a[index]) - 
               tolower((unsigned char)str_b[index]);
        if(0 != diff)
        {
            return -1;
        }
    }
    return 0;
}

ErrorCode strtolowercase(char const * source_str, char * buf, size_t buf_len){
    
    int i = 0;
    int str_len = strlen(source_str);

    if(buf_len < str_len)
        return E_BUFFER_SIZE;

    for(i = 0; i < str_len; i++){
        buf[i] = tolower((unsigned char)source_str[i]);
    }

    return WORK_SUCCESSFULLY;
}

ErrorCode startThread(pthread_t *thread, void *( *start_routine)(void *),
                      void *arg)
{
    pthread_attr_t attr;

    if ( pthread_attr_init( &attr) != 0 ||
         pthread_create(thread, &attr, start_routine, arg) != 0 ||
         pthread_detach( *thread))
    {
          zlog_info(category_debug, "Start Thread Error.");
          return E_START_THREAD;
    }

    return WORK_SUCCESSFULLY;
}

int get_system_time()
{
    /* Return value as a long long type */
    int system_time;

    time_t now = time(NULL);

    /* second ver. */
    system_time = (int)now;

    return system_time;
}

int get_clock_time()
{
#ifdef _WIN32
    return GetTickCount() / 1000;
#elif __unix__
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    return current_time.tv_sec;
#endif
}


char *strtok_save(char *str, char *delim, char **saveptr)
{
    char *tmp;

    if(str == NULL)
    {
        tmp = *saveptr;
    }
    else
    {
        tmp = str;
    }

    if(strncmp(tmp, delim, strlen(delim)) == 0)
    {
        *saveptr += strlen(delim) * sizeof(char);
        return NULL;
    }
    
#ifdef _WIN32
    return strtok_s(str, delim, saveptr);
#elif __unix__
    return strtok_r(str, delim, saveptr);
#endif
    
}


int display_time(void)
{
    // variables to store date and time components
    //int hours, minutes, seconds, day, month, year;

    // time_t is arithmetic time type
    time_t now;

    // Obtain current time
    // time() returns the current time of the system as a time_t value
    time(&now);

    // Convert to local time format and print to stdout
    zlog_debug(category_debug, "%s", ctime(&now));
    
    return 0;
}

void sleep_t(int wait_time)
{
#ifdef _WIN32
    Sleep(wait_time);
#elif __unix__
    wait_time*=1000;
    usleep(wait_time);
#endif
}


