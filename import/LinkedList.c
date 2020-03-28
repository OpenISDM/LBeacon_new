/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and conditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIS

  File Name:

     LinkedList.c

  File Description:

     This file contains the generic implementation of a double-linked-list
     data structure.It has functions for inserting a node to the front of the
     list and deleting a specific node. It can also check the length of the
     list. Any datatype of data could be stored in this list.

  Version:

     2.0, 20190826

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

     Han Wang, hollywang@iis.sinica.edu.tw
     Jake Lee, jakelee@iis.sinica.edu.tw
     Johnson Su, johnsonsu@iis.sinica.edu.tw
     Shirley Huang, shirley.huang.93@gmail.com
     Han Hu, hhu14@illinois.edu
     Jeffrey Lin, lin.jeff03@gmail.com
     Howard Hsu, haohsu0823@gmail.com
     Chun-Yu Lai, chunyu1202@gmail.com
 */


#include "LinkedList.h"


void concat_list(List_Entry *first_list_head, List_Entry *second_list_head){

    List_Entry *first_list_tail, *second_list_tail;

    first_list_tail = first_list_head -> prev;
    second_list_tail = second_list_head -> prev;

    first_list_tail -> next = second_list_head;
    first_list_head -> prev = second_list_tail;

    second_list_tail -> next = first_list_head;
    second_list_head -> prev = first_list_tail;

}

int get_list_length(List_Entry *entry) {

    struct List_Entry *listptrs;
    int list_length = 0;

    for (listptrs = (entry)->next; listptrs != (entry);
         listptrs = listptrs->next) {
        list_length ++;
    }

    return list_length;
}
