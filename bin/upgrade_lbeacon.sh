#!/bin/bash

echo "upgarde configuration file"
sudo chmod 755 /home/bedis/LBeacon/bin/upgrade_lbeacon_config.sh
sudo /home/bedis/LBeacon/bin/upgrade_lbeacon_config.sh

echo "upgrate other things"

echo "start process"
cd /home/bedis/LBeacon/bin/
sudo chmod 755 /home/bedis/LBeacon/bin/auto_LBeacon.sh
sudo /home/bedis/LBeacon/bin/auto_LBeacon.sh
