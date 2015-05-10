add_library('net')
from network import send_TCP
from prepare import prepare
import random

# Config, will be checked upstream
height = 15
width = 25
framerate = 30
brightness = 1
#####################################
#receiver_IP = "10.2.113.151"
#receiver_IP = "10.24.147.20"
receiver_IP = "dummy"
receiver_port = 9999

# Do we have one single long line?
long_line = False
# Type of installation (see details in data_generator)
type = 2

# TODO: test with real serial
# https://www.pjrc.com/teensy/td_uart.html

ledTCP = None
data = None
current_px = 0

cur_color = None
cur_len = 0
len_change_choice = [7, 11, 13, 17, 19]

def setup():
    global ledTCP
    global data
    global cur_color
    global len_change
    len_change = random.choice(len_change_choice)
    background(0)
    cur_color = color(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

    ledTCP, data = prepare(Client, receiver_IP, receiver_port, height, width, framerate, brightness)
    send_TCP(ledTCP, data, long_line, type)

def draw():
    global cur_len
    global current_px
    global cur_color
    global len_change
    pixels[current_px] = cur_color
    px = current_px - 1
    for i in range(cur_len + 1):
        if px == 0:
            col = pixels[len(pixels) -1]
        else:
            col = pixels[px]
        if i == cur_len:
            col = color(red(col) * 0.9, green(col) * 0.9, blue(col) * 0.9)
        pixels[px] = col
        px -=1
    updatePixels()

    if cur_len == len_change:
        cur_color = color(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        cur_len = 0
        len_change = random.choice(len_change_choice)
    else:
        cur_len += 1
    if current_px >= len(pixels) - 1:
        current_px = 0
    else:
        current_px += 1
    send_TCP(ledTCP, data, long_line, type)
