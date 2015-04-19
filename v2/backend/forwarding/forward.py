#!/usr/bin/env python3

import redis
import time
from serial import Serial, SerialException
import sys
import os

max_height = 25
max_width = 20
max_framerate = 40

# Use dummy to test the Processing code without have access to actual hardware
serialDevice = 'dummy'

# OSX serial device for teensy (not the same everywhere but gives a good indications what it might look like)
#serialDevice = '/dev/tty.usbmodem807701'

# Linux Serial device
#serialDevice = '/dev/ttyACM0'

wait_time = None


def update_framerate():
    global wait_time
    new_framerate = int(r.hget('config', 'cur_framerate'))
    if new_framerate >= max_framerate or new_framerate == 0:
        wait_time = 1.0 / max_framerate
    else:
        wait_time = 1.0 / new_framerate


def send(r, s):
    os.system('clear')
    print(r.llen('new'))
    data = r.rpop('new')
    if data is not None and len(data) > 0:
        now = time.time()
        end = now + wait_time
        a = bytes([ord('*')]) + bytearray(data) + bytes([ord('#')])
        if s != 'dummyMode':
            s.write(a)
        time.sleep(end - now)


def serialConfigure(port_name, baudrate=9600):
    '''
        We use a very low baudrate by default because the USB port on the teensy
        enforce this value: http://www.pjrc.com/teensy/td_serial.html
    '''
    if port_name == 'dummy':
        print("Dummy mode detected, disabling Serial comms")
        time.sleep(1)
        ser = 'dummyMode'
        return ser

    ser = Serial()
    ser.port = port_name
    ser.baudrate = baudrate
    ser.timeout = 5
    try:
        ser.open()
    except SerialException as e:
        sys.stderr.write("Could not open serial port %s: %s\n" % (ser.portstr, e))
        return

    ledsPerStrip = max_height * max_width
    print(ledsPerStrip)
    ser.write(ledsPerStrip.to_bytes(4, byteorder='little'))

    data = ser.read(4)
    print(data, int.from_bytes(data, byteorder='little'))

    ser.timeout = 1
    return ser


def serialDataConfigure(port_name, baudrate=115200):
    ser = Serial()
    ser.port = port_name
    ser.baudrate = baudrate
    try:
        ser.open()
    except SerialException as e:
        sys.stderr.write("Could not open serial port %s: %s\n" % (ser.portstr, e))
        return

    ser.timeout = 1
    return ser

if __name__ == "__main__":
    r = redis.Redis()
    r.hset('config', 'height', max_height)
    r.hset('config', 'width', max_width)
    r.hset('config', 'imgsize', max_height * max_width * 24)
    r.hset('config', 'max_framerate', max_framerate)
    r.hset('config', 'cur_framerate', max_framerate)
    s = serialConfigure(serialDevice)
    # s_data = serialDataConfigure('/dev/ttyUSB0')
    wait_time = 1.0 / max_framerate
    while True:
        while r.llen('new') > 0:
            send(r, s)
        time.sleep(1)
        update_framerate()
