#!/bin/bash

#Check network status and Change .conf
sudo touch /home/bedis/LBeacon/log/test_crontab
gatewayAddr="$( sudo ip r| sed -n 1p| cut -d " " -f 3 )"
confGatewayAddr="$( cat /home/bedis/LBeacon/config/config.conf | grep "gateway_addr" |cut -d "=" -f 2 | tr -d '\r\n')"
#check if we could ping to routers (Gateway or AP)
ping $gatewayAddr -c 1
if [ "X$?" != "X0" ]; then
    sudo ifdown --force wlan0
    sudo ifup wlan0
else
    if [ $gatewayAddr != "$( cat /home/bedis/LBeacon/config/config.conf | grep "gateway_addr" |cut -d "=" -f 2 | tr -d '\r\n')" ]; then
	#Change NTP settings and restart
	sudo sed -i '/prefer iburst/cserver '"$gatewayAddr"' prefer iburst' /etc/ntp.conf
	sudo sed -i '/prefer iburst/cserver '"$gatewayAddr"' prefer iburst' /var/lib/ntp/ntp.conf.dhcp
	sudo /etc/init.d/ntp restart
	#Change config and restart LBeacon
	sudo sed -i 's/gateway_addr='"$confGatewayAddr"'/gateway_addr='"$gatewayAddr"'/' /home/bedis/LBeacon/config/config.conf
	sudo /home/bedis/LBeacon/bin/kill_LBeacon.sh
	cd /home/bedis/LBeacon/bin/
	sudo /home/bedis/LBeacon/bin/auto_LBeacon.sh
	fi
fi
