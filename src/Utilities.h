/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and conditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIPS

  File Description:

     This is the header file containing the function declarations and variables
     used in the Utilities.c file.

  File Name:

     Utilities.h

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

     Jake Lee, jakelee@iis.sinica.edu.tw
     Johnson Su, johnsonsu@iis.sinica.edu.tw
     Shirley Huang, shirley.huang.93@gmail.com
     Howard Hsu, haohsu0823@gmail.com
     Gary Xiao, garyh0205@hotmail.com
     Holly Wang, hollywang@iis.sinica.edu.tw
 */


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

#ifndef BEDIS_H

#include "BeDIS.h"
#endif


#ifndef UTILITIES_H
#define UTILITIES_H


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
 ctrlc_handler:

     If the user presses CTRL-C, the global variable g_done will be set to true,
     and a signal will be thrown to stop running the LBeacon program.

 Parameters:

     s - @todo

 Return value:

     None

 */
extern void ctrlc_handler(int stop);


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
     clock in terms of the number of milliseconds since January 1, 1970.

  Parameters:

     None

  Return value:

     system_time - system time in milliseconds
*/

long long get_system_time();


#endif
