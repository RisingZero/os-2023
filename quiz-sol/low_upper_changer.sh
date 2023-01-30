#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Usage: $0 <filename"
	exit 2
fi

if [ ! -f $1 ]
then
	echo "ERROR: $1 is not a file"
	exit 1
fi

while read -r line
do
	words=$(echo -n $line | wc -w)
	let mod=$words%2

	if [ $mod -eq 0 ]
	then
		# Even
		echo $(echo -n $line | tr [:lower:] [:upper:])
	else
		# Odd
		echo $(echo -n $line | tr [:upper:] [:lower:])
	fi

done < $1

