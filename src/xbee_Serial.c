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
 *      BeDIPS
 *
 * File Description:
 *
 *   	This file contains the program to connect to xbee and send AT command
 *      to xbee.
 *
 * File Name:
 *
 *      xbee_Serial.c
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
 *      Gary Xiao		, garyh0205@hotmail.com
 */

#include "xbee_Serial.h"

int xbee_Serial_Power_Reset(int Wiring_Pi_Pin){

    //Check we have wiringPi
    if (wiringPiSetup () == -1 ){

        return Wiring_Pi_Setup_Fail;

    }

    pinMode(Wiring_Pi_Pin, OUTPUT);

    digitalWrite (Wiring_Pi_Pin, 0);

    usleep(xbee_usleep_time);

    digitalWrite (Wiring_Pi_Pin, 1);

    usleep(xbee_usleep_time);

    return 0;

}

int xbee_Serial_init(int *xbee_datastream, char *xbee_device){

    //Open in non blocking read/write mode  // | O_NOCTTY | O_NDELAY
    if ((*xbee_datastream = open(xbee_device, O_RDWR )) == -1)

        return Serial_init_Fail;

    // set new parameters to the serial device
    struct termios newtio;

    fcntl(*xbee_datastream, F_SETFL, 0);
    // set everything to 0
    bzero(&newtio, sizeof(newtio));

    // again set everything to 0
    bzero(&newtio, sizeof(newtio));

    newtio.c_cflag |= baudrate; // Set Baudrate first time
    newtio.c_cflag |= CLOCAL; // Local line - do not change "owner" of port
    newtio.c_cflag |= CREAD; // Enable receiver

    newtio.c_cflag &= ~ECHO; // Disable echoing of input characters
    newtio.c_cflag &= ~ECHOE;

    // set to 8N1
    newtio.c_cflag &= ~PARENB; // no parentybyte
    newtio.c_cflag &= ~CSTOPB; // 1 stop bit
    newtio.c_cflag &= ~CSIZE; // Mask the character size bits
    newtio.c_cflag |= databits; // 8 data bits

    // output mode to
    newtio.c_oflag = 0;

    // Set teh baudrate for sure
    cfsetispeed(&newtio, baudrate);
    cfsetospeed(&newtio, baudrate);

    newtio.c_cc[VTIME] = 10; /* inter-character timer  */
    newtio.c_cc[VMIN] = 0; /* blocking read until  */

    tcflush(*xbee_datastream, TCIFLUSH); // flush pending data

    // set the new defined settings
    if (tcsetattr(*xbee_datastream, TCSANOW, &newtio))

        return Serial_Setting_Fail;

    return 0;
}

int xbee_Serial_Tx(int* xbee_datastream, char* Data){

    //----- TX BYTES -----
    unsigned char tx_buffer[xbee_Serial_buffer + 1];

    unsigned char *p_tx_buffer;

    memset(tx_buffer, 0, sizeof(char) * (xbee_Serial_buffer + 1));

    p_tx_buffer = &tx_buffer[0];

    for(int i = 0; i < strlen(Data) ; i++)

        *p_tx_buffer++ = Data[i];

    if (*xbee_datastream != -1){

        int count = write(*xbee_datastream, &tx_buffer[0],
                            (p_tx_buffer - &tx_buffer[0]));

        //Datastream, bytes to write, number of bytes to write
        if (count < 0)

            return Serial_Tx_Error;

    }

    else

        return xbee_datastream_Error;

    return 0;
}

int xbee_Serial_Rx(int *xbee_datastream, char* Data){

    int Waiting;

    if(strlen(Data) > 0){

        Waiting = Remain;

    }

    else{

        Waiting = Ended;

    }

    int Received = 0;

    //----- CHECK FOR ANY RX BYTES -----
    if (*xbee_datastream != -1){

        // Read up to xbee_Serial_buffer characters from the port
        unsigned char rx_buffer[xbee_Serial_buffer + 1];

        memset(rx_buffer, 0, sizeof(char) * (xbee_Serial_buffer + 1));

        int rx_length = 0;

        int count = 0;

        do{

            memset(rx_buffer, 0, sizeof(char) * (xbee_Serial_buffer + 1));

            rx_length = read(*xbee_datastream, (void*)rx_buffer
                           , xbee_Serial_buffer);

            if(rx_length == 0){

                if(count == 5)

                    return Serial_Rx_Error;

                else

                    count ++;

            }
 	        else{

                count = 0;

                if((strlen(Data) - Received) > rx_length - 1){

                    return Data_Receive_OverFlow;

                }

                for(int i = 0 ; i < rx_length - 1; i++){

                    if(rx_buffer[i] != Data[i + Received]){

                        return Data_Receive_MisMatch;

                    }

                }

                Received += rx_length - 1;

                if(strlen(Data) == Received)

                    Waiting = Ended;

            }

        }while(Waiting != Ended);

    }

    else

        return xbee_datastream_Error;

    return 0;

}

char* xbee_Serial_Return(int *xbee_datastream){

    unsigned char rx_buffer[xbee_Serial_buffer + 1];

    int rx_length;

    memset(rx_buffer, 0, sizeof(char) * (xbee_Serial_buffer + 1));

    //----- CHECK FOR ANY RX BYTES -----

    if (*xbee_datastream != -1){

        // Read up to xbee_Serial_buffer characters from the port

        rx_length = 0;

        //Datastream, buffer to store in, number of bytes to read (max)

        rx_length = read(*xbee_datastream, (void*)rx_buffer
                       , xbee_Serial_buffer);

        //Bytes received

        rx_buffer[rx_length] = '\0';

    }

    else

        return "NULL";

    char* return_received = malloc(sizeof(char)*rx_length);

    for(int n = 0; n < rx_length; n++ )

        return_received[n] = rx_buffer[n];

    return return_received;

}

int  xbee_Send_Command(int *xbee_datastream, char *Command
                     , char *Command_Result){

    int ret, count = 0;

    while(ret = xbee_Serial_Tx(xbee_datastream, "+++") != 0){

        count ++;

        if(count == 5)

            return ret;

    }

    usleep(xbee_usleep_time);

    count = 0;

    while(ret = xbee_Serial_Rx(xbee_datastream, "OK") != 0){

        count ++;

        if(count == 5)

            return ret;

    }

    usleep(xbee_usleep_time);

    count = 0;

    while(ret = xbee_Serial_Tx(xbee_datastream, Command) != 0){

        count ++;

        if(count == 5)

            return ret;

    }

    usleep(xbee_usleep_time);

    count = 0;

    while(ret = xbee_Serial_Rx(xbee_datastream, Command_Result) != 0){

        count ++;

        if(count == 5)

            return ret;

    }

    return 0;

}

char* xbee_Send_Command_result(int *xbee_datastream, char *Command){

    int ret, count = 0;

    while(ret = xbee_Send_Command(xbee_datastream, "+++", "OK") != 0){

        count ++;

        if(count == 5)

            return "NULL";

    }

    usleep(xbee_usleep_time);

    count = 0;

    while(ret = xbee_Serial_Tx(xbee_datastream, Command) != 0){

        count ++;

        if(count == 5)

            return "NULL";

    }

    usleep(xbee_usleep_time);

    char* result =  xbee_Serial_Return(xbee_datastream);

    return result;
}
