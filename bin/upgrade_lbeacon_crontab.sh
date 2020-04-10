#!/bin/bash

echo "check newer package"
is_lbeacon_package=`sudo ls -t /tmp/LBeacon-*.tar.gz | head -1 | wc -l`

if [ "_$is_lbeacon_package" = "_1" ]
then
    echo "has pakcage, checking version"

    version_info=`sudo ls -tr /home/bedis/LBeacon/*.txt | head -1 | cut -d "/" -f 5 | cut -d "." -f 1-2`
    build_info=`sudo ls -tr /home/bedis/LBeacon/*.txt | head -1 | cut -d "/" -f 5 | cut -d "." -f 3`
    
    new_version_info=`sudo ls -t /tmp/LBeacon-*.tar.gz | head -1 | cut -d "-" -f 3 | cut -d "." -f 1-2`
    new_build_info=`sudo ls -t /tmp/LBeacon-*.tar.gz | head -1 | cut -d "-" -f 3 | cut -d "." -f 3`
    
    echo "$version_info $build_info"
    echo "$new_version_info $new_build_info"
 
    if [ "_$version_info" = "_$new_version_info" ] && [ "$new_build_info" -gt "$build_info" ]
    then 
        echo "continue to apply newer package"
    else
        echo "no newer package"
        exit 0
    fi
else
    echo "no package"
    exit 0
fi

echo "stop running process"
cd /home/bedis/LBeacon/bin/
sudo chmod 755 /home/bedis/LBeacon/bin/kill_LBeacon.sh
sudo /home/bedis/LBeacon/bin/kill_LBeacon.sh

echo "backup existing configration file"
sudo cp /home/bedis/LBeacon/config/config.conf /home/bedis/upgrade-LBeacon/config_save.conf 

echo "remove existing version files"
sudo rm -f /home/bedis/LBeacon/*.txt

echo "upgrade package"
filename=`sudo ls -t /tmp/LBeacon-*.tar.gz | head -1 | cut -d "/" -f 3-`
sudo cp /tmp/$filename /home/bedis/$filename
cd /home/bedis
sudo tar zxvf $filename

echo "trigger upgrade program inside newer package"
sudo chmod 755 /home/bedis/LBeacon/bin/upgrade_lbeacon.sh
sudo /home/bedis/LBeacon/bin/upgrade_lbeacon.sh

echo "leave an upgrade record"
upgraded_info=`sudo ls -tr /home/bedis/LBeacon/*.txt | head -1`
now=`date`
echo "$now - $upgraded_info" >> /home/bedis/LBeacon/upgrade_history
