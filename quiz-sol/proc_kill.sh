#!/bin/bash

ps -aux | egrep -v "^root" | tail -n+2 | while IFS= read -r line
do
	usage=$(echo $line | cut -d " " -f 4 | cut -d "." -f 1)	
	if [ $usage -gt 25 ]
	then
		pid=$(echo $line | cut -d " " -f 2)
		kill -9 $pid
	fi
done
