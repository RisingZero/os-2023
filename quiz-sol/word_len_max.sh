#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Usage: $0 <filename>"
	exit 1
fi

if [ ! -f $1 ]
then
	echo "ERROR: $1 is a not a file"
	exit 2
fi

words=$(cat $1)
max_len=0
max_word=""
for word in $words
do
	n=$(echo $word | wc -c)
	if [ $max_len -lt $n ]
	then
		max_len=$n
		max_word=$word
	fi
done

echo $max_word

