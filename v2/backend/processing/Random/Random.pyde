add_library('net')
from network import send_TCP
from prepare import prepare
import random

# Config, will be checked upstream
height = 25
width = 20
framerate = 30
brightness = 0.1
#####################################
receiver_IP = "10.2.113.151"
#receiver_IP = "127.0.0.1"
receiver_port = 9999

# Do we have one single long line?
long_line = False

# TODO: test with real serial
# https://www.pjrc.com/teensy/td_uart.html

ledTCP = None
data = None

def setup():
    global ledTCP
    global data
    ledTCP, data = prepare(Client, receiver_IP, receiver_port, height, width, framerate, brightness)
    background(0)
    send_TCP(ledTCP, data, long_line)

def draw():
    pixels[random.randint(0, len(pixels) - 1)] = color(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
    pixels[random.randint(0, len(pixels) - 1)] = color(0, 0, 0)
    updatePixels()
    send_TCP(ledTCP, data, long_line)
