#!/bin/bash
n=0
while [ 1 ]
do
	while [ 1 ]
	do
	  echo $1
	  cat display 
	  #sleep 0.01
	  n=$((n+1))
	  printf "$n \r" >&2
	done  | 
	netcat -p500$1 -u 127.0.0.1 4321
sleep 2
done
	
