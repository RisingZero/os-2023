#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Usage: $0 <filename>"
	exit 1
fi

if [ ! -f $1 ]
then
	echo "ERROR: $1 is not a file"
	exit 2
fi

N=$(head -n 1 $1 | wc -w)
cnt=0
i=1
while [ $i -le $N ]
do
	line=$(head -n $i $1 | tail -n 1)
	j=1
	for word in $line
	do
		if [ $i -eq $j ]
		then
			len=$(echo -n $word | wc -c)
			let cnt=$cnt+$len
		fi
		let j=$j+1
	done

	let i=$i+1
done


echo $cnt
