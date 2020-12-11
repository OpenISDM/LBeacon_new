#!/bin/bash

echo "Upgrade configuration file"
config_new="/home/bedis/LBeacon/config/config.conf"
config_save="/home/bedis/upgrade-LBeacon/config_save.conf"
config_result="/home/bedis/LBeacon/config/config_result.conf"

sudo cat /dev/null > $config_result

while IFS= read -r line
do
    config_key=`echo $line | cut -d "=" -f 1`
    echo "handle config key: $config_key"    
    
    is_exist=`sudo cat $config_save | grep "$config_key" | wc -l`
    if [ "_$is_exist" = "_1" ] && [ "_$config_key" != "_scan_device_name_prefix" ] 
    then
        config_value_save=`sudo cat $config_save | grep $config_key | cut -d "=" -f 2`
        echo "$config_key=$config_value_save" >> $config_result
    else
        echo "$line" >> $config_result
    fi
done < "$config_new"

sudo mv $config_result $config_new


echo "Upgrade self_check.sh"
self_check_new="/home/bedis/LBeacon/bin/self_check.sh"
self_check_bak="/home/bedis/upgrade-LBeacon/self_check.sh.bak"


value_is_lbeacon_without_gateway=`sudo cat $self_check_bak | grep "IS_LBEACON_WITHOUT_GATEWAY=" | cut -d "=" -f 2`
sudo sed -i 's/IS_LBEACON_WITHOUT_GATEWAY=.*/IS_LBEACON_WITHOUT_GATEWAY='$value_is_lbeacon_without_gateway'/' $self_check_new

value_is_lbeacon_with_gateway=`sudo cat $self_check_bak | grep "IS_LBEACON_WITH_GATEWAY=" | cut -d "=" -f 2`
sudo sed -i 's/IS_LBEACON_WITH_GATEWAY=.*/IS_LBEACON_WITH_GATEWAY='$value_is_lbeacon_with_gateway'/' $self_check_new

value_is_gateway_without_ap=`sudo cat $self_check_bak | grep "IS_GATEWAY_WITHOUT_AP=" | cut -d "=" -f 2`
sudo sed -i 's/IS_GATEWAY_WITHOUT_AP=.*/IS_GATEWAY_WITHOUT_AP='$value_is_gateway_without_ap'/' $self_check_new

value_is_gateway_with_ap=`sudo cat $self_check_bak | grep "IS_GATEWAY_WITH_AP=" | cut -d "=" -f 2`
sudo sed -i 's/IS_GATEWAY_WITH_AP=.*/IS_GATEWAY_WITH_AP='$value_is_gateway_with_ap'/' $self_check_new

