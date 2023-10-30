#!/bin/bash
to=""
if [ "" == "$1" ]
then
	to="$PWD"
else
	to="$1"
fi

for arg in $(realpath $to/*)
do
	echo $arg
done
