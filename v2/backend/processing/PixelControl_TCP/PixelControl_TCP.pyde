add_library('net')
import math
import time
import struct
from network import receive_config, send_config, send_TCP
from data_generator import prepare_data

# Config, will be checked upstream
height = 5
width = 8
framerate = 30
#####################################

gamma = 1.7
brightness = 4
dimension = 0

# TODO: test with real serial
# https://www.pjrc.com/teensy/td_uart.html

long_line = True

ledTCP = None
data = None

current_px = 0
gammatable = None

def TCPConfigure(server, port):
    return Client(this, server, port)

def check_config(max_height, max_width, max_framerate):
    if height > max_height:
        return False, "height cannot be higher than {}. Current: {}.".format(max_height, height)
    if width > max_width:
        return False, "width cannot be higher than {}. Current: {}.".format(max_width, width)
    if framerate > max_framerate:
        return False, "framerate cannot be higher than {}. Current: {}.".format(max_framerate, framerate)
    return True, None

def setup():
    global gammatable
    global dimension
    global data
    global ledTCP
    
    ledTCP = TCPConfigure("127.0.0.1", 9999)
    max_height, max_width, max_framerate = receive_config(ledTCP)
    print(max_height, max_width, max_framerate)
    good, reason = check_config(max_height, max_width, max_framerate)
    if not good:
        raise Exception(reason)
        send_config(ledTCP)
    size(width, height)
    dimension = width * height
    frameRate(framerate)
    data, gammatable = prepare_data(dimension, gamma, brightness)
    loadPixels()
    for i in range(dimension):
        pixels[i] = color(0, 0, 0)
    updatePixels()
    send_TCP(ledTCP, data, long_line, gammatable)

def draw():
    global current_px
    pixels[current_px] = color(0, 255, 0)
    if current_px == 0:
        pixels[len(pixels) - 1] = color(0, 0, 0)
    else:
        pixels[current_px - 1] = color(0, 0, 0)
    updatePixels()
    if current_px == len(pixels) - 1:
        current_px = 0
    else:
        current_px += 1
    send_TCP(ledTCP, data, long_line, gammatable)
