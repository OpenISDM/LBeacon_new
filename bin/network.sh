#!/bin/bash

#check if we could ping to routers (Gateway or AP)
if ! ifconfig wlan0 | grep -q "inet addr:"; then
    sudo ifdown wlan0
    sleep 5
    sudo ifup wlan0
fi
