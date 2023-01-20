#!/bin/bash

if [ $# -ne 1 ]
then
    echo "ERROR: usage $0 <dirname>"
    exit 1
fi

dir=$1

if [ ! -d $dir ]
then
    echo "ERROR: given dir doesn't exist"
    exit 1
fi

objects=$(find $dir)

for obj in $objects
do
    dirname=$(dirname $obj)
    filename=$(basename $obj)

    echo $filename | egrep -q -E "[A-Z]*"
    if [[ $filename != $dir && $? -ne 1 ]]
    then
        lowerc_filename=$(echo $filename | tr [:upper:] [:lower:])
        if [ $lowerc_filename != $filename ]
        then
            mv $obj ${dirname}/${lowerc_filename}
        fi
    fi
done