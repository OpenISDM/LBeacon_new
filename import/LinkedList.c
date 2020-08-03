/*
  Copyright (c) BiDaE Technology Inc. All rights reserved.

  License:

    BiDaE SHAREWARE LICENSE
    Version 1.0, 31 July 2020

    Copyright (c) BiDaE Technology Inc. All rights reserved.
    The SOFTWARE implemented in the product is copyrighted and protected by 
    all applicable intellectual property laws worldwide. Any duplication of 
    the SOFTWARE other than for archiving or resetting purposes on the same 
    product without the written agreement from BiDaE Technology could be a 
    violation of law. For the avoidance of doubt, redistribution of this 
    SOFTWARE in source or binary form is prohibited. You agree to prevent 
    any unauthorized copying and redistribution of the SOFTWARE. 

    BiDaE Technology Inc. is the license steward. No one other than the 
    license steward has the right to modify or publish new versions of this 
    License. However, You may distribute the SOFTWARE under the terms of the 
    version of the License under which You originally received the Covered 
    Software, or under the terms of any subsequent version published by the 
    license steward.

    LIMITED WARRANTY:

    BiDaE Technology Inc. or its distributors, depending on which party sold 
    the SOFTWARE, warrants that the media on which the SOFTWARE is installed 
    will be free from defects in materials under normal and purposed use.

    BiDaE Technology Inc. or its distributor warrants, for your benefit alone, 
    that during the Warranty Period the SOFTWARE, shall operate substantially 
    in accordance with the functional specifications in the User's Manual. If, 
    during the Warranty Period, a defect in the SOFTWARE appears, You may 
    obtain a replacement of the SOFTWARE. Any replacement SOFTWARE will be 
    warranted for the remainder of the Warranty Period attached to the product.

    WHEN THE WARRANTY PERIOD HAS BEEN EXPIRED, THIS SOFTWARE IS PROVIDED 
    ''AS IS,'' AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
    PARTICULAR PURPOSE ARE DISCLAIMED. HENCEFORTH, IN NO EVENT SHALL BiDaE 
    TECHNOLOGY INC. OR ITS COLLABORATOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
