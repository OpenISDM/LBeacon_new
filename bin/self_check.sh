#!/bin/bash
# Cusomization settings
IS_LBEACON_WITHOUT_GATEWAY=0
IS_LBEACON_WITH_GATEWAY=1
IS_GATEWAY_WITHOUT_AP=0
IS_GATEWAY_WITH_AP=0

# Please do not modify following code
lbeacon_output="/home/bedis/LBeacon/log/self_check_result"
lbeacon_version_output="/home/bedis/LBeacon/log/version"
gateway_output="/home/bedis/Lbeacon-Gateway/log/self_check_result"
gateway_version_output="/home/bedis/Lbeacon-Gateway/log/version"

WORK_SUCCESSFULLY=0

ERR_HCI_COUNT=1001
ERR_WLAN_COUNT=1002
ERR_WLAN_SPECIFY_WLAN0=1003
ERR_WLAN_SPECIFY_WLAN1=1004

ERR_CRONTAB_NETWORK=2001
ERR_CRONTAB_SELF_CHECK=2002
ERR_CRONTAB_PING_IP=2003

ERR_USERNAME=3001
ERR_RC_LOCAL=3002
ERR_PROCESS_HOSTAPD=3003
ERR_WLAN0_RUNNING=3004
ERR_WLAN1_RUNNING=3005
ERR_HCI_RUNNING=3006

ERR_PROCESS_LBEACON=4001
ERR_PROCESS_GATEWAY=4002

ERR_ZLOG_LBEACON=5001
ERR_ZLOG_GATEWAY=5002
ERR_DEBUG_LBEACON=5003
ERR_DEBUG_GATEWAY=5004

# Display expected BOT component
if [ "_$IS_LBEACON_WITHOUT_GATEWAY" = "_1" ]
then 
    echo "This is Lbeacon without gateway on the same box"
    HCI_COUNT=2
elif [ "_$IS_LBEACON_WITH_GATEWAY" = "_1" ]
then 
    echo "This is Lbeacon with gateway on the same box"
    HCI_COUNT=2
elif [ "_$IS_GATEWAY_WITHOUT_AP" = "_1" ]
then
    echo "This is Gateway without AP on the same box"
elif [ "_$IS_GATEWAY_WITH_AP" = "_1" ]
then 
    echo "This is Gateway with AP on the same box"
else
    echo "Unknown component"
    exit 0 
fi

# Check hardwares
echo "checking [CPU] ....."
echo `sudo cat /proc/cpuinfo | grep "Hardware" `

if [ "_$IS_LBEACON_WITHOUT_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY" = "_1" ]
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
        sudo echo "$ERR_HCI_COUNT" > $lbeacon_output
        exit 0
    fi
fi

if [ "_$IS_GATEWAY_WITH_AP" = "_1" ]
then
    echo "checking [wlan] ....."
    echo "checking number of installed WLAN devices ....."
    wlan_count=`sudo ifconfig | grep "wlan" | wc -l`
    if [ "_$wlan_count" = "_2" ]
    then
        echo "ok"
    else 
        echo "not ok"
        sudo echo "$ERR_WLAN_COUNT" > $gateway_output
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
        sudo echo "$ERR_WLAN_SPECIFY_WLAN0" > $gateway_output
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
        sudo echo "$ERR_WLAN_SPECIFY_WLAN1" > $gateway_output
        exit 0
    fi
fi

# Check system software
echo "checking [kernel] ....."
sudo echo `sudo uname -a`

# Check system configuration
echo "checking [username = bedis] ....."
if [ "_$IS_LBEACON_WITHOUT_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY" = "_1" ]
then
    pwd_count=`sudo ls -al /home/ | grep "bedis" | wc -l`
    if [ "_$pwd_count" = "_1" ]
    then 
        echo "ok"
    else
        echo "not ok"
        sudo echo "$ERR_USERNAME" > $lbeacon_output
        exit 0
    fi
elif [ "_$IS_GATEWAY_WITHOUT_AP" = "_1" ] || [ "_$IS_GATEWAY_WITH_AP" = "_1" ]
then
    pwd_count=`sudo ls -al /home/ | grep "bedis" | wc -l`
    if [ "_$pwd_count" = "_1" ]
    then 
        echo "ok"
    else
        echo "not ok"
        sudo echo "$ERR_USERNAME" > $gateway_output
        exit 0
    fi
fi

echo "checking [rc.local] ....."
if [ "_$IS_LBEACON_WITHOUT_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY" = "_1" ]
then 
    echo "checking [LBeacon] ....."
    beacon_count=`sudo cat /etc/rc.local | grep "/home/bedis/LBeacon/bin/auto_LBeacon.sh" | wc -l`
    if [ "_$beacon_count" = "_2" ]
    then 
        echo "ok"
    else
        echo "not ok"
        sudo echo "$ERR_RC_LOCAL" > $lbeacon_output
        exit 0 
    fi
elif [ "_$IS_GATEWAY_WITHOUT_AP" = "_1" ] || [ "_$IS_GATEWAY_WITH_AP" = "_1" ]
then 
    echo "checking [Gateway] ....."
    gateway_count=`sudo cat /etc/rc.local | grep "/home/bedis/Lbeacon-Gateway/bin/auto_Gateway.sh" | wc -l`
    if [ "_$gateway_count" = "_2" ]
    then 
        echo "ok"
    else
        echo "not ok"
        sudo echo "$ERR_RC_LOCAL" > $gateway_output
        exit 0 
    fi
fi

echo "checking [crontab] ....."
if [ "_$IS_LBEACON_WITHOUT_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY" = "_1" ]
then
    echo "checking have self_check.sh ....."
    crontab_count=`sudo crontab -l | grep "LBeacon.*self_check.sh" | grep -v "#" | wc -l`
    if [ "_$crontab_count" = "_1" ]
    then
        echo "ok"
    else
        echo "not ok"
        sudo echo "$ERR_CRONTAB_SELF_CHECK" > $lbeacon_output
        exit 0
    fi
elif [ "_$IS_GATEWAY_WITHOUT_AP" = "_1" ] || [ "_$IS_GATEWAY_WITH_AP" = "_1" ]
then
    echo "checking have self_check.sh ....."
    crontab_count=`sudo crontab -l | grep "Lbeacon-Gateway.*self_check.sh" | grep -v "#" | wc -l`
    if [ "_$crontab_count" = "_1" ]
    then
        echo "ok"
    else
        echo "not ok"
        sudo echo "$ERR_CRONTAB_SELF_CHECK" > $gateway_output
        exit 0
    fi
fi
    
if [ "_$IS_LBEACON_WITHOUT_GATEWAY" = "_1" ]
then 
    echo "checking have network.sh ....."
    crontab_count=`crontab -l -u bedis | grep "network.sh" | grep -v "#" | wc -l`
    if [ "_$crontab_count" = "_1" ]
    then
        echo "ok"
    else
        echo "not ok"
        sudo echo "$ERR_CRONTAB_NETWORK" > $lbeacon_output
        exit 0
    fi
fi

if [ "_$IS_GATEWAY_WITHOUT_AP" = "_1" ] || [ "_$IS_GATEWAY_WITH_AP" = "_1" ]
then
    echo "checking have ping_ip.sh ....."
    crontab_count=`sudo crontab -l | grep "ping_ip.sh" | grep -v "#" | wc -l`
    if [ "_$crontab_count" = "_1" ]
    then
        echo "ok"
    else
        echo "not ok"
        sudo echo "$ERR_CRONTAB_PING_IP" > $gateway_output
        exit 0
    fi
fi

if [ "_$IS_GATEWAY_WITH_AP" = "_1" ]
then 
    echo "checking [hostapd] ....."
    hostapd_process=`sudo ps aux | grep -i "hostapd" | grep -v "color" | grep -v "grep" | wc -l`
    if [ "_$hostapd_process" = "_1" ]
    then 
        echo "ok"
    else
        echo "not ok"
        sudo echo "$ERR_PROCESS_HOSTAPD" > $gateway_output
        exit 0 
    fi
fi


echo "checking [BOT component version] ....."
if [ "_$IS_LBEACON_WITHOUT_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY" = "_1" ]
then 
    echo "checking [LBeacon] ....."
    beacon_version=`sudo ls -al /home/bedis/LBeacon/*.txt | cut -d "/" -f 5`
    echo "$beacon_version"
    beacon_version_number=`sudo ls -al /home/bedis/LBeacon/*.txt | cut -d "/" -f 5 | cut -d "." -f 1-3`
    sudo echo "$beacon_version_number" > $lbeacon_version_output
elif [ "_$IS_GATEWAY_WITHOUT_AP" = "_1" ] || [ "_$IS_GATEWAY_WITH_AP" = "_1" ]
then 
    echo "checking [Gateway] ....."
    gateway_version=`sudo ls -al /home/bedis/Lbeacon-Gateway/*.txt | cut -d "/" -f 5`
    echo "$gateway_version"
    gateway_version_number=`sudo ls -al /home/bedis/Lbeacon-Gateway/*.txt | cut -d "/" -f 5 | cut -d "." -f 1-3`
    sudo echo "$gateway_version_number" > $gateway_version_output
fi

echo "checking [WLAN running status] ....."
if [ "_$IS_GATEWAY_WITH_AP" = "_1" ]
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
        sudo echo "$ERR_WLAN1_RUNNING" > $gateway_output
        exit 0 
    fi
fi

if [ "_$IS_LBEACON_WITHOUT_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY" = "_1" ] 
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
            sudo echo "$ERR_HCI_RUNNING" > $lbeacon_output
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
            sudo echo "$ERR_HCI_RUNNING" > $lbeacon_output
            exit 0 
        fi 
    fi
fi

# Check BOT component installation
echo "checking [BOT running processes] ....."
if [ "_$IS_LBEACON_WITHOUT_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY" = "_1" ] 
then 
    echo "checking [LBeacon] ....."
    beacon_process=`sudo ps aux | grep "LBeacon" | grep -v "color" | grep -v "self_check.sh" | grep -v "grep" | wc -l`
    if [ "_$beacon_process" = "_2" ]
    then 
        echo "ok"
    else
        echo "not ok"
        sudo echo "$ERR_PROCESS_LBEACON" > $lbeacon_output
        exit 0 
    fi
elif [ "_$IS_GATEWAY_WITHOUT_AP" = "_1" ] || [ "_$IS_GATEWAY_WITH_AP" = "_1" ]
then 
    echo "checking [Gateway] ....."
    gateway_process=`sudo ps aux | grep "Gateway.out" | grep -v "color" | grep -v "self_check.sh" | grep -v "grep" | wc -l`
    if [ "_$gateway_process" = "_2" ]
    then 
        echo "ok"
    else
        echo "not ok"
        sudo echo "$ERR_PROCESS_GATEWAY" > $gateway_output
        exit 0 
    fi
fi

# Check BOT component configuration
echo "checking [zlog.conf] ....."
if [ "_$IS_LBEACON_WITHOUT_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY" = "_1" ]
then 
    echo "checking [LBeacon] ....."
    beacon_zlog=`sudo cat /home/bedis/LBeacon/config/zlog.conf | grep "LBeacon_Debug.DEBUG.*bedis" | wc -l`
    if [ "_$beacon_zlog" = "_1" ]
    then 
        echo "ok"
    else
        echo "not ok"
        sudo echo "$ERR_ZLOG_LBEACON" > $lbeacon_output
        exit 0 
    fi
elif [ "_$IS_GATEWAY_WITHOUT_AP" = "_1" ] || [ "_$IS_GATEWAY_WITH_AP" = "_1" ] 
then 
    echo "checking [Gateway] ....."
    gateway_zlog=`sudo cat /home/bedis/Lbeacon-Gateway/config/zlog.conf | grep "LBeacon_Debug.DEBUG.*bedis" | wc -l`
    if [ "_$gateway_zlog" = "_1" ]
    then 
        echo "ok"
    else
        echo "not ok"
        sudo echo "$ERR_ZLOG_GATEWAY" > $gateway_output
        exit 0 
    fi
fi

echo "checking [DEBUG log] ....."
if [ "_$IS_LBEACON_WITHOUT_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY" = "_1" ]
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
        sudo echo "$ERR_DEBUG_LBEACON" > $lbeacon_output
        exit 0 
    fi
elif [ "_$IS_GATEWAY_WITHOUT_AP" = "_1" ] || [ "_$IS_GATEWAY_WITH_AP" = "_1" ]
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
        sudo echo "$ERR_DEBUG_GATEWAY" > $gateway_output
        exit 0 
    fi
fi

if [ "_$IS_LBEACON_WITHOUT_GATEWAY" = "_1" ] || [ "_$IS_LBEACON_WITH_GATEWAY" = "_1" ]
then
    sudo echo "$WORK_SUCCESSFULLY" > $lbeacon_output
elif [ "_$IS_GATEWAY_WITHOUT_AP" = "_1" ] || [ "_$IS_GATEWAY_WITH_AP" = "_1" ]
then
    sudo echo "$WORK_SUCCESSFULLY" > $gateway_output
fi
exit 0
