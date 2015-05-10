add_library('net')
from network import send_TCP
from prepare import prepare

# Config, will be checked upstream
height = 25
width = 15
framerate = 10
brightness = 0.4
#####################################
#receiver_IP = "10.2.113.151"
receiver_IP = "10.24.146.53"
#receiver_IP = "127.0.0.1"
receiver_port = 9999

# Do we have one single long line?
long_line = True
# Type of installation (see details in data_generator)
type = 2

# TODO: test with real serial
# https://www.pjrc.com/teensy/td_uart.html

ledTCP = None
data = None
current_px = 0

def setup():
    global ledTCP
    global data
    ledTCP, data = prepare(Client, receiver_IP, receiver_port, height, width, framerate, brightness)
    background(0)
    send_TCP(ledTCP, data, long_line, type)

def draw():
    global current_px
    pixels[current_px] = color(0, 255, 0)
    if current_px == 0:
        pixels[len(pixels) - 1] = color(255, 0, 0)
    else:
        pixels[current_px - 1] = color(255, 0, 0)
    updatePixels()
    if current_px == len(pixels) - 1:
        current_px = 0
    else:
        current_px += 1
    send_TCP(ledTCP, data, long_line, type)
