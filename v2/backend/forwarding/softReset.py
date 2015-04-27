#!/usr/bin/env python
# -*- coding: utf-8 -*-

from serial import Serial, SerialException
import sys

# OSX serial device for teensy (not the same everywhere but gives a good indications what it might look like)
# serialDevice = '/dev/tty.usbmodem807701'

# Linux Serial device
serialDevice = '/dev/ttyACM1'

if __name__ == "__main__":
    ser = Serial()
    ser.port = serialDevice
    try:
        ser.open()
        ser.timeout = 1
        ser.write(ord('!').to_bytes(4, byteorder='little'))
    except SerialException as e:
        sys.stderr.write("Could not open serial port %s: %s\n" % (ser.portstr, e))
