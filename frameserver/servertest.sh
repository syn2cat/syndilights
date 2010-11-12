#!/bin/bash

while [ 1 ]
do
	echo "$1" | cat - display | netcat -q 0.1 -u 127.0.0.1 4321
done
