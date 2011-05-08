#!/bin/bash

processes=30
i=0

while [ "$i" -lt "$processes" ]
do
	python servertest.py "$i" &
	pids=( ${pids[@]} $! )
	echo ${pids["$i"]}
	i=$((i+1))
	#sleep 0.1
done

#wait for input to exit
read -n1 -s

#kill our background pythons
i=0
while [ "$i" -lt "$processes" ]
do
	echo "${pids["$i"]}"
	kill "${pids["$i"]}"
	i=$((i+1))
done	