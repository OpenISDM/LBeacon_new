/*
 * Copyright (c) 2016 Academia Sinica, Institute of Information Science
 *
 * License:
 *
 *      GPL 3.0 : The content of this file is subject to the terms and
 *      cnditions defined in file 'COPYING.txt', which is part of this
 *      source code package.
 *
 * Project Name:
 *
 *      BeDIS
 *
 * File Description:
 *
 *   	This file contains the header of  function declarations and variable
 *      used in Serial.c
 *
 * File Name:
 *
 *      xbee_Serial.h
 *
 * Abstract:
 *
 *      BeDIS uses LBeacons to deliver 3D coordinates and textual
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
 *      Gary Xiao		, garyh0205@hotmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>         //Used for Serial
#include <fcntl.h>          //Used for Serial
#include <termios.h>        //Used for Serial
#include <string.h>
#include <wiringPi.h>

#ifndef xbee_Serial_H
#define xbee_Serial_H

#define baudrate B9600
#define databits  CS8
#define xbee_Serial_buffer 50
#define xbee_Serial_Power_Pin 1 //wiringPi pin 1 is BCM_GPIO 18.
#define xbee_usleep_time 500000

/*
 * A flag to record does the command returned value.
 * Remain : the command we send haven't return value.
 * Ended  : the command we send had return value.
 *
 */
enum{ Remain , Ended };

enum{ Wiring_Pi_Setup_Fail = -1, Serial_init_Fail = -2,
      Serial_Setting_Fail = -3, Data_Receive_MisMatch = -4,
      Data_Receive_OverFlow = -5, xbee_datastream_Error = -6,
      Serial_Tx_Error = -7, Serial_Rx_Error = -8};

/*
 * xbee_Serial_Power_Reset
 *
 *     Reset xbee Power.
 *
 * Parameter:
 *
 *     Wiring_Pi_Pin: the pin we decide to reset.(Using wiringPi)
 *
 * Return Value:
 *
 *     int: If return 0, everything work successfully.
 *          If return Wiring_Pi_Setup_Fail, wiringPi Setup fail.
 *
 */
int xbee_Serial_Power_Reset(int Wiring_Pi_Pin);

/*
 * xbee_Serial_init
 *
 *     Initialize Serial transmission.
 *
 * Parameter:
 *
 *     xbee_datastream: A pointer to catch zigbee datastream.
 *     xbee_device: Defin it's device path.
 *
 * Return Value:
 *
 *     int: If return 0, everything work successfully.
 *          If not 0, somthing wrong.
 *
 */
 int xbee_Serial_init(int *xbee_datastream, char *xbee_device);

/*
 * xbee_Serial_Tx
 *
 *     Send data to Serial.
 *
 * Parameter:
 *
 *     xbee_datastream: A pointer to catch zigbee datastream.
 *     Data: data we want to send.
 *
 * Return Value:
 *
 *     int: If return 0, everything work successfully.
 *          If not 0, somthing wrong.
 *
 */
int xbee_Serial_Tx(int *xbee_datastream, char* Data);

/*
 * xbee_Serial_Rx
 *
 *     Reading data from Serial.
 *
 * Parameter:
 *
 *     xbee_datastream: A pointer to catch zigbee datastream.
 *     Data: for storec data we received.
 *
 * Return Value:
 *
 *     int: If return 0, everything work successfully.
 *               If not 0, somthing wrong.
 *
 */
 int xbee_Serial_Rx(int *xbee_datastream, char* Data);

/*
 * xbee_Serial_Return
 *
 *     Reading data from Serial and return what it is read.
 *
 * Parameter:
 *
 *     xbee_datastream: A pointer to catch zigbee datastream.
 *
 * Return Value:
 *
 *     char: return the data read from Serial.
 *
 */
char* xbee_Serial_Return(int *xbee_datastream);

/*
 * xbee_Send_Command
 *
 *     Send command to zigbee and check it's result.
 *
 * Parameter:
 *
 *     xbee_datastream: A pointer to catch zigbee datastream.
 *     Command: The command we want to send.
 *     Command_Result: The expected result.
 *
 * Return Value:
 *
 *     int: If return 0, everything work successfully.
 *          If not 0, somthing wrong.
 *
 */
int xbee_Send_Command(int *xbee_datastream, char* Command
                    , char* Command_Result);

/*
 * xbee_Send_Command
 *
 *     Send command to zigbee and return it's result.
 *
 * Parameter:
 *
 *     xbee_datastream: A pointer to catch zigbee datastream.
 *     Command: The command we want to send.
 *
 * Return Value:
 *
 *     char: If return result, everything work successfully.
 *           If return NULL, somthing wrong.
 *
 */
 char* xbee_Send_Command_result(int *xbee_datastream, char* Command);

#endif
