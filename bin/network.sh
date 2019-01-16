#!/bin/bash

#check if we could ping to routers (Gateway or AP)
sudo cat /etc/dhcpcd.conf | grep "routers" | cut -d "=" -f 2 | xargs ping -nc 1
if [ "X$?" != "X0" ]; then
    sudo ifdown --force wlan0
    sudo ifup wlan0
fi
