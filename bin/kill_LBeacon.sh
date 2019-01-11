#!/bin/bash

ps -ef | grep LBeacon | grep -v grep | awk '{print $2}' | xargs sudo kill -SIGINT
