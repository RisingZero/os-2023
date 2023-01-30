#!/bin/bash

if [ ! $# -ge 1 ]
then
	echo "Usage: $0 <list of files>"
	exit 1
fi

objects=$*

for obj in $objects
do
	if [ -f $obj ]
	then
		# If file
		ls_output=$(ls -l | egrep -E "$obj" | head -n 1)
		size=$(echo -n $ls_output | cut -d " " -f 5)
		echo -n "$obj: FILE, size $size "
		if [ -r $obj ]
		then
			echo -n "r"
		else
			echo -n "-"
		fi
		if [ -w $obj ]
		then
			echo -n "w"
		else
			echo -n "-"
		fi
		if [ -x $obj ]
		then
			echo -n "x"
		else
			echo -n "-"
		fi
		echo ""
	elif [ -d $obj ]
	then
		# If dir
		subdirs=$(ls -l $obj | egrep -E "^d" | wc -l)
		echo "$obj: DIR, subdirs $subdirs"
	fi
done
