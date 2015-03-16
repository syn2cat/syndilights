#!/usr/bin/env python

import redis
import time
from serial import Serial, SerialException
import sys

height = 5
width = 8


def send(r, s):
    if not r.exists('new'):
        return None

    data = r.rpop('new')
    if data is not None and len(data) > 0:
        s.write(data.encode())
        # size = s.write(data)
        # print('Data sent ({} bytes)'.format(size))


def serialConfigure(port_name, baudrate=9600):
    '''
        We use a very low baudrate by default because the USB port on the teensy
        enforce this value: http://www.pjrc.com/teensy/td_serial.html
    '''
    ser = Serial()
    ser.port = port_name
    ser.baudrate = baudrate
    ser.timeout = 5
    try:
        ser.open()
    except SerialException as e:
        sys.stderr.write("Could not open serial port %s: %s\n" % (ser.portstr, e))
        return

    ser.write(height.to_bytes(4, byteorder='little'))
    print(int.from_bytes(ser.read(4), byteorder='little'))

    ser.write(width.to_bytes(4, byteorder='little'))
    print(int.from_bytes(ser.read(4), byteorder='little'))

    ser.timeout = 1
    return ser

if __name__ == "__main__":
    r = redis.Redis()
    r.hset('config', 'imgsize', height * width * 24 + 1)
    s = serialConfigure('/dev/ttyACM0')
    while True:
        while r.llen('new') > 0:
            send(r, s)
        time.sleep(10)
