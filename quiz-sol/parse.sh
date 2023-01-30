#!/bin/bash

if [ ! $# -eq 6 ]
then
	echo "Usage: $0 fileIn str1 str2 N str3 fileOut"
	exit 2
fi

if [ ! -f $1 ]
then
	echo "ERROR: $1 is a file!"
	exit 1
fi

fileIn=$1
str1=$2 
str2=$3
N=$4
str3=$5
fileOut=$6

while read -r line
do
	replaced=$(echo -n $line | sed s/$str1/$str2/)
	i=0
	while [ $i -lt $N ]
	do
		echo -n "$str3 "
		let i=$i+1
	done
	echo -n $replaced
	i=0
	while [ $i -lt $N ]
	do
		echo -n " $str3"
		let i=$i+1
	done
	echo ""
done < $fileIn > $fileOut
