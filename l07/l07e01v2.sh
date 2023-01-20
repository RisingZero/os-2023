#!/bin/bash

dir=$1
func=$2
outfile=$3

if [ $# -ne 3 ]
then
    echo "ERROR: usage $0 <dirname> <funcname> <outfile>"
    exit 1
fi

if [ ! -d $dir ]
then
    echo "ERROR: the given directory doesn't exist"
    exit 1
fi

files=$(find $dir -type f)

for file in $files
do
    egrep -n -H -E "${func}\([a-zA-Z0-9 _,]*\)" $file >> $outfile
done

cat $outfile | tr ":" " " | sort -n -k 2 | sort -k 1 > $outfile