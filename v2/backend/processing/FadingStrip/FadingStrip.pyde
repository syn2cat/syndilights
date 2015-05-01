add_library('net')
from network import send_TCP
from prepare import prepare
import random

# Config, will be checked upstream
height = 25
width = 15
framerate = 10
brightness = 0.5
#####################################
#receiver_IP = "10.2.113.151"
#receiver_IP = "10.24.147.20"
receiver_IP = "dummy"
receiver_port = 9999

# Do we have one single long line?
long_line = True

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
    send_TCP(ledTCP, data, long_line)

def draw():
    global cur_len
    global current_px
    global cur_color
    global len_change
    if cur_len == 0:
        pixels[current_px] = cur_color
    else:
        px = current_px
        for i in range(cur_len):
            if px == 0:
                prec_col = pixels[len(pixels) - 1]
            else:
                prec_col = pixels[px - 1]
            col = color(red(prec_col) / 1.1, green(prec_col) / 1.1, blue(prec_col) / 1.1)
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
    send_TCP(ledTCP, data, long_line)
