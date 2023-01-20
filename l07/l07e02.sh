#!/bin/bash

if [ $# -ne 1 ]
then
    echo "ERROR: usage $0 <file>"
    exit 1
fi

file=$1

if [ ! -f $file ]
then
    echo "ERROR: the argument received is not a file"
    exit 1
fi

line_no=$(wc -l $file | cut -f 1 -d " ")
max_line_c=0
for line in $(cat $file)
do
    x=$(echo $line | wc -c)
    if [ $x -gt $max_line_c ]
    then
        max_line_c=$x
    fi
done

echo "The longest line has ${max_line_c} characters, the file has a total of ${line_no} lines"