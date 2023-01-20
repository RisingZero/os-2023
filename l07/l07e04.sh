#!/bin/bash

if [[ $# -ne 1 || ! -f $1 ]]
then
    echo "Please give a file name:"
    read $file
else
    file=$1
fi

declare -A frequency
for str in $(cat $file)
do
    if [ ! ${frequency[$str]} ]
    then
        frequency[$str]=1
    else
        let "frequency[$str] = ${frequency[$str]} + 1"
    fi
done

for str in ${!frequency[*]}
do
    echo "$str ${frequency[$str]}"
done