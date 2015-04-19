#!/bin/bash

port=''

for i in `seq 2 5`; 
do
    echo '1-1.'$i > /sys/bus/usb/drivers/usb/unbind
    if [ $? == 0 ]; then
        echo 'Found port: 1-1.'$i
        port='1-1.'$i
        break
    fi
done

if [ ${#port} == 0 ]; then
    echo Unable to find a device connected on USB, trying to reconnect on all ports...
    for i in `seq 2 5`;
    do
        echo '1-1.'$i > /sys/bus/usb/drivers/usb/bind
	if [ $? == 0 ]; then
	    echo 'Found port: 1-1.'$i
	    break
        fi
    done
else
    sleep 1
    echo -n Reconnect...
    echo $port > /sys/bus/usb/drivers/usb/bind
fi
        
if [ $? == 0 ]; then
    echo done
    echo -e 'Device reachable on port: \n\t' `dmesg | grep cdc_acm | tail -n 1 | cut -d: -f 3`
    echo You may have to change the port in forwarder.py
else
    echo Failed, you probably have to move your body to the teensy.
fi


