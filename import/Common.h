/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and cnditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIS

  File Name:

     Common.h

  File Description:

     This file contains the definitions and declarations of constants,
     structures, and functions used in server, gateway and Lbeacon.

  Version:

     2.0, 20191227

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

     Chun Yu Lai   , chunyu1202@gmail.com
 */

#ifndef COMMON_H
#define COMMON_H

/* Server API protocol version for communications between server and 
   gateway.*/

/* BOT_SERVER_API_VERSION_20 is compatible with BOT_GATEWAY_API_VERSION_10 */

/* BOT_SERVER_API_VERSION_LATEST=2.1 is compatible with both 
   BOT_GATEWAY_API_VERSION_10 and BOT_GATEWAY_API_VERSION_LATEST=1.1 */
   
#define BOT_SERVER_API_VERSION_20 "2.0"

#define BOT_SERVER_API_VERSION_21 "2.1"

#define BOT_SERVER_API_VERSION_LATEST "2.2"

/* The size of message to be sent over WiFi in bytes */
#define WIFI_MESSAGE_LENGTH 4096

/* Length of the IP address in byte */
#define NETWORK_ADDR_LENGTH 16

typedef enum pkt_types {
    /* Unknown type of pkt type */
    undefined = 0,

    /* Request join from LBeacon */
    request_to_join = 1,
    
    /* Join response */
    join_response = 2,

    /* A pkt containing time critical tracked object data */
    time_critical_tracked_object_data = 3,
    
    /* A pkt containing tracked object data */
    tracked_object_data = 4,
    
    /* A pkt containing health report */
    gateway_health_report = 5,
    
    /* A pkt containing health report */
    beacon_health_report = 6,
    
    /* A pkt containing notification alarm */
    notification_alarm = 7,

    /* A pkt containing IPC command */
    ipc_command = 8,
} PktType;

typedef enum pkt_direction {
    /* pkt from server */
    from_server = 2,

    /* pkt from GUI */
    from_gui = 3,
    
    /* pkt from gateway */
    from_gateway = 6,

    /* pkt from beacon */
    from_beacon = 8,

} PktDirection;

typedef enum IPCCommand {

    CMD_NONE = 0,
    CMD_RELOAD_GEO_FENCE_SETTING = 1,
    CMD_MAX,

} IPCCommand;

/* Type of aspects of geo-fence to be reloaded. */
typedef enum ReloadGeoFenceSetting {

    /* No need to reload. */
    GEO_FENCE_NONE = 0,

    /* Reload both geo-fence list and monitored objects. */
    GEO_FENCE_ALL = 1, 

    /* Reload only geo-fence list. */
    GEO_FENCE_LIST = 2,

    /* Reload only geo-fence monitored objects */
    GEO_FENCE_OBJECT = 3,

    GEO_FENCE_MAX, 

} ReloadGeoFenceSetting;

/* Type of coverage of areas. */
typedef enum AreaScope {

    AREA_NONE = 0,

    AREA_ALL = 1, 

    AREA_ONE = 2,

    AREA_MAX, 

} AreaScope;

#endif