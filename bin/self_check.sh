#!/bin/bash
# Cusomization settings
IS_LBEACON=0
IS_GATEWAY=0
IS_LBEACON_WITH_GATEWAY_RPIZW=0
IS_LBEACON_WITH_GATEWAY_RPI3B=1

# Please do not modify following code
lbeacon_output="/home/bedis/LBeacon/log/self_check_result"
lbeacon_version_output="/home/bedis/LBeacon/log/version"
gateway_output="/home/bedis/Lbeacon-Gateway/log/self_check_result"
gateway_version_output="/home/bedis/Lbeacon-Gateway/log/version"

WORK_SUCCESSFULLY=0

ERR_HCI_COUNT=10001
ERR_WLAN_COUNT=10002
ERR_WLAN_SPECIFY_WLAN0=10003
ERR_WLAN_SPECIFY_WLAN1=10004

ERR_USERNAME=30001
ERR_RC_LOCAL=30002
ERR_CRONTAB=30003
ERR_PROCESS_HOSTAPD=30004
ERR_WLAN0_RUNNING=30005
ERR_WLAN1_RUNNING=30006
ERR_HCI_RUNNING=30007

ERR_PROCESS_LBEACON=40001
ERR_PROCESS_GATEWAY=40002

ERR_ZLOG_LBEACON=50001
ERR_ZLOG_GATEWAY=50002
ERR_DEBUG_LBEACON=50003
ERR_DEBUG_GATEWAY=50004

if [ "_$IS_LBEACON" = "_1" ]
then
    HCI_COUNT=2
    WLAN_COUNT=1
elif [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ]
then
    HCI_COUNT=2
    WLAN_COUNT=1
elif [ "_$IS_GATEWAY" = "_1" ]
then
    HCI_COUNT=0
    WLAN_COUNT=2
elif [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ] 
then 
    HCI_COUNT=2
    WLAN_COUNT=2   
fi

# Display expected BOT component
if [ "_$IS_LBEACON" = "_1" ]
then 
    echo "This is Lbeacon"
elif [ "_$IS_GATEWAY" = "_1" ]
then
    echo "This is Gateway"
elif [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ]
then 
    echo "This is Lbeacon (with Gateway) on RPi ZW"
elif [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then 
    echo "This is Lbeacon (with Gateway) on RPi 3B"
else
    echo "Unknown component"
    exit 0 
fi

# Check hardwares
echo "checking [CPU] ....."
echo `sudo cat /proc/cpuinfo | grep "Hardware" `

if [ "_$HCI_COUNT" != "_0" ]
then
    echo "checking [HCI] ....."
    echo "checking number of running HCI devices ....."
    detected_hci_count=`sudo hciconfig | grep "RUNNING" | wc -l`
    echo "detected hci count:" $detected_hci_count
    if [ "_$detected_hci_count" = "_$HCI_COUNT" ]
    then
        echo "ok"
    else 
        echo "not ok"
        echo "$ERR_HCI_COUNT" > $lbeacon_output
        exit 0
    fi
fi

if [ "_$WLAN_COUNT" = "_2" ]
then
    echo "checking [wlan] ....."
    echo "checking number of installed WLAN devices ....."
    wlan_count=`sudo ifconfig | grep "wlan" | wc -l`
    if [ "_$wlan_count" = "_$WLAN_COUNT" ]
    then
        echo "ok"
    else 
        echo "not ok"
        echo "$ERR_WLAN_COUNT" > $gateway_output
        exit 0
    fi

    echo "checking correction of specifying WLAN0 devices ....."
    wlan0_mac_address_ifconfig=`sudo ifconfig | grep "wlan0" | cut -d " " -f 10`
    wlan0_mac_address_udev=`sudo cat /etc/udev/rules.d/99-com.rules | grep "wlan0" | cut -d " " -f 3 | cut -d "\"" -f 2`
    if [ "_$wlan0_mac_address_ifconfig" = "_$wlan0_mac_address_udev" ]
    then
        echo "ok"
    else 
        echo "not ok"
        echo "$ERR_WLAN_SPECIFY_WLAN0" > $gateway_output
        exit 0
    fi

    echo "checking correction of specifying WLAN1 devices ....."
    wlan1_mac_address_ifconfig=`sudo ifconfig | grep "wlan1" | cut -d " " -f 10`
    wlan1_mac_address_udev=`sudo cat /etc/udev/rules.d/99-com.rules | grep "wlan1" | cut -d " " -f 3 | cut -d "\"" -f 2`
    if [ "_$wlan1_mac_address_ifconfig" = "_$wlan1_mac_address_udev" ]
    then
        echo "ok"
    else 
        echo "not ok"
        echo "$ERR_WLAN_SPECIFY_WLAN1" > $gateway_output
        exit 0
    fi
fi

# Check system software
echo "checking [kernel] ....."
echo `uname -a`

# Check system configuration
echo "checking [username = bedis] ....."
pwd_count=`pwd | grep "bedis" | wc -l`
if [ "_$pwd_count" = "_1" ]
then 
    echo "ok"
else
    echo "not ok"
    echo "$ERR_USERNAME" > $lbeacon_output
    echo "$ERR_USERNAME" > $gateway__output
    exit 0
fi

echo "checking [rc.local] ....."
if [ "_$IS_LBEACON" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then 
    echo "checking [LBeacon] ....."
    beacon_count=`sudo cat /etc/rc.local | grep "/home/bedis/LBeacon/bin/auto_LBeacon.sh" | wc -l`
    if [ "_$beacon_count" = "_2" ]
    then 
        echo "ok"
    else
        echo "not ok"
        echo "$ERR_RC_LOCAL" > $lbeacon_output
        exit 0 
    fi
fi
if [ "_$IS_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then 
    echo "checking [Gateway] ....."
    gateway_count=`sudo cat /etc/rc.local | grep "/home/bedis/Lbeacon-Gateway/bin/auto_Gateway.sh" | wc -l`
    if [ "_$gateway_count" = "_2" ]
    then 
        echo "ok"
    else
        echo "not ok"
        echo "$ERR_RC_LOCAL" > $gateway_output
        exit 0 
    fi
fi

echo "checking [crontab] ....."
if [ "_$IS_LBEACON" = "_1" ]
then
    echo "checking crontab ....."
    crontab_count=`crontab -l -u bedis | grep "network.sh" | grep -v "#" | wc -l`
    if [ "_$crontab_count" = "_1" ]
    then
        echo "ok"
    else
        echo "not ok"
        echo "$ERR_CRONTAB" > $lbeacon_output
        exit 0
    fi
elif [ "_$IS_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then
    echo "checking crontab ....."
    crontab_count=`crontab -l -u bedis | grep "network.sh" | grep -v "#" | wc -l`
    if [ "_$crontab_count" = "_0" ]
    then
        echo "ok"
    else
        echo "not ok"
        echo "$ERR_CRONTAB" > $lbeacon_output
        echo "$ERR_CRONTAB" > $gateway_output
        exit 0
    fi

fi

echo "checking [hostapd] ....."
if [ "_$IS_LBEACON" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ]
then 
    echo "checking [hostapd] ....."
    hostapd_process=`ps aux | grep -i "hostapd" | grep -v "color" | grep -v "grep " | wc -l`
    if [ "_$hostapd_process" = "_0" ]
    then 
        echo "ok"
    else
        echo "not ok"
        echo "$ERR_PROCESS_HOSTAPD" > $lbeacon_output
        exit 0 
    fi
elif [ "_$IS_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then 
    echo "checking [hostapd] ....."
    hostapd_process=`ps aux | grep -i "hostapd" | grep -v "color" | grep -v "grep" | wc -l`
    if [ "_$hostapd_process" = "_1" ]
    then 
        echo "ok"
    else
        echo "not ok"
        echo "$ERR_PROCESS_HOSTAPD" > $gateway_output
        exit 0 
    fi
fi

echo "checking [WLAN running status] ....."
echo "checking [wlan0] ....."
echo `sudo ifconfig | grep -A 1 "wlan0" | grep "inet" | cut -d ":" -f 2 | cut -d " " -f 1`
wlan0_current_status=`sudo ifconfig | grep -A 6 "wlan0" | grep "TX"`
sleep 5
wlan0_later_status=`sudo ifconfig | grep -A 6 "wlan0" | grep "TX"`
if [ "_$wlan0_current_status" != "_$wlan0_later_status" ]
then
    echo "ok"
else
    echo "not ok"
    echo "$ERR_WLAN0_RUNNING" > $lbeacon_output
    echo "$ERR_WLAN0_RUNNING" > $gateway_output
    exit 0 
fi

if [ "_$WLAN_COUNT" = "_2" ]
then
    echo "checking [wlan1] ....."
    echo `sudo ifconfig | grep -A 1 "wlan1" | grep "inet" | cut -d ":" -f 2 | cut -d " " -f 1`
    wlan1_current_status=`sudo ifconfig | grep -A 6 "wlan1" | grep "TX"`
    sleep 5
    wlan1_later_status=`sudo ifconfig | grep -A 6 "wlan1" | grep "TX"`
    if [ "_$wlan1_current_status" != "_$wlan1_later_status" ]
    then
        echo "ok"
    else
        echo "not ok"
        echo "$ERR_WLAN1_RUNNING" > $gateway_output
        exit 0 
    fi
fi

if [ "_$IS_LBEACON" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then
    echo "checking [HCI running status] ....."
    scan_dongle_id=`sudo cat /home/bedis/LBeacon/config/config.conf | grep "scan_dongle_id=0" | wc -l`
    if [ "_$scan_dongle_id" = "_1" ]
    then 
        echo "checking [hci0] ....."
        hci_current_status=`sudo hciconfig hci0 | grep "RX"`
        sleep 5
        hci_later_status=`sudo hciconfig hci0 | grep "RX"`
        if [ "_$hci_current_status" != "_$hci_later_status" ]
        then
            echo "ok"
        else
            echo "not ok"
            echo "$ERR_HCI_RUNNING" > $lbeacon_output
            exit 0 
        fi 
    elif [ "_$scan_dongle_id" = "_0" ]
    then
        echo "checking [hci1] ....."
        hci_current_status=`sudo hciconfig hci1 | grep "RX"`
        sleep 5
        hci_later_status=`sudo hciconfig hci1 | grep "RX"`
        if [ "_$hci_current_status" != "_$hci_later_status" ]
        then
            echo "ok"
        else
            echo "not ok"
            echo "$ERR_HCI_RUNNING" > $lbeacon_output
            exit 0 
        fi 
    fi
fi

# Check BOT component installation
echo "checking [BOT component version] ....."
if [ "_$IS_LBEACON" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then 
    echo "checking [LBeacon] ....."
    beacon_version=`ls -al /home/bedis/LBeacon/*.txt | cut -d "/" -f 5`
    echo "$beacon_version"
    beacon_version_number=`ls -al /home/bedis/LBeacon/*.txt | cut -d "/" -f 5 | cut -d "." -f 1-3`
    echo "$beacon_version_number" > $lbeacon_version_output
fi
if [ "_$IS_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then 
    echo "checking [Gateway] ....."
    gateway_version=`ls -al /home/bedis/Lbeacon-Gateway/*.txt | cut -d "/" -f 5`
    echo "$gateway_version"
    gateway_version_number=`ls -al /home/bedis/Lbeacon-Gateway/*.txt | cut -d "/" -f 5 | cut -d "." -f 1-3`
    echo "$gateway_version_number" > $gateway_version_output
fi

echo "checking [BOT running processes] ....."
if [ "_$IS_LBEACON" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then 
    echo "checking [LBeacon] ....."
    beacon_process=`ps aux | grep "LBeacon" | grep -v "color" | grep -v "grep" | wc -l`
    if [ "_$beacon_process" = "_2" ]
    then 
        echo "ok"
    else
        echo "not ok"
        echo "$ERR_PROCESS_LBEACON" > $lbeacon_output
        exit 0 
    fi
fi
if [ "_$IS_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then 
    echo "checking [Gateway] ....."
    gateway_process=`ps aux | grep "Gateway" | grep -v "color" | grep -v "grep" | wc -l`
    if [ "_$gateway_process" = "_2" ]
    then 
        echo "ok"
    else
        echo "not ok"
        echo "$ERR_PROCESS_GATEWAY" > $gateway_output
        exit 0 
    fi
fi

# Check BOT component configuration
echo "checking [zlog.conf] ....."
if [ "_$IS_LBEACON" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then 
    echo "checking [LBeacon] ....."
    beacon_zlog=`sudo cat /home/bedis/LBeacon/config/zlog.conf | grep "LBeacon_Debug.DEBUG.*bedis" | wc -l`
    if [ "_$beacon_zlog" = "_1" ]
    then 
        echo "ok"
    else
        echo "not ok"
        echo "$ERR_ZLOG_LBEACON" > $lbeacon_output
        exit 0 
    fi
fi
if [ "_$IS_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then 
    echo "checking [Gateway] ....."
    gateway_zlog=`sudo cat /home/bedis/Lbeacon-Gateway/config/zlog.conf | grep "LBeacon_Debug.DEBUG.*bedis" | wc -l`
    if [ "_$gateway_zlog" = "_1" ]
    then 
        echo "ok"
    else
        echo "not ok"
        echo "$ERR_ZLOG_GATEWAY" > $gateway_output
        exit 0 
    fi
fi

echo "checking [DEBUG log] ....."
if [ "_$IS_LBEACON" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then 
    echo "checking [LBeacon] ....."
    beacon_current_debug=`sudo wc -c /home/bedis/LBeacon/log/diagnostic.log | cut -d " " -f 1`
    sleep 5
    beacon_later_debug=`sudo wc -c /home/bedis/LBeacon/log/diagnostic.log | cut -d " " -f 1`
    if [ "_$beacon_current_debug" != "_$beacon_later_debug" ]
    then 
        echo "ok"
    else
        echo "not ok"
        echo "$ERR_DEBUG_LBEACON" > $lbeacon_output
        exit 0 
    fi
fi
if [ "_$IS_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then 
    echo "checking [Gateway] ....."
    gateway_current_debug=`sudo wc -c /home/bedis/Lbeacon-Gateway/log/diagnostic.log | cut -d " " -f 1`
    sleep 5
    gateway_later_debug=`sudo wc -c /home/bedis/Lbeacon-Gateway/log/diagnostic.log | cut -d " " -f 1`
    if [ "_$gateway_current_debug" != "_$gateway_later_debug" ]
    then 
        echo "ok"
    else
        echo "not ok"
        echo "$ERR_DEBUG_GATEWAY" > $gateway_output
        exit 0 
    fi
fi

if [ "_$IS_LBEACON" = "_1" ]
then
    echo "$WORK_SUCCESSFULLY" > $lbeacon_output
elif [ "_$IS_GATEWAY" = "_1" ]
then
    echo "$WORK_SUCCESSFULLY" > $gateway_output
elif [ "_$IS_LBEACON_WITH_GATEWAY_RPIZW" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY_RPI3B" = "_1" ]
then
    echo "$WORK_SUCCESSFULLY" > $lbeacon_output
    echo "$WORK_SUCCESSFULLY" > $gateway_output
fi
exit 0
