#!/bin/bash

if [ $# -ne 2 ]
then
    echo "ERROR: usage $0 <processname> <interval (s)>"
    exit 1
fi

process=$1
interval=$2

counter=0
ok=1

while [ $ok -ne 0 ]
do
    output=$(ps -le | tr -s " " | cut -f 2,14 -d " " | egrep -E "$process")
    if [ $? -eq 1 ]
    then
        ok=0
        echo "Process not running"
    else
        echo $output | cut -f 1 | egrep -E "Z"
        if [ $? -eq 0 ]
        then
            let counter=${counter}+1
            echo "Process found in state ZOMBIE $counter times"
            if [ $counter -eq 5 ]
            then
                ok=0
                echo "Limit reached, closing"
            fi
        else
            counter=0
            echo "Process alive"
        fi
        sleep $interval
    fi
done
