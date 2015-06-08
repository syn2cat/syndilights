"""
Saturation. 

Saturation is the strength or purity of the color and represents the 
amount of gray in proportion to the hue. A "saturated" color is pure 
and an "unsaturated" color has a large percentage of gray. 
Move the cursor vertically over each bar to alter its saturation. 
"""

add_library('net')
from network import send_TCP
from prepare import prepare
import random

# Config, will be checked upstream
height = 8
width = 32
framerate = 100
brightness = 0.05
#####################################
receiver_IP = "10.2.113.107"
#receiver_IP = "dummy"
receiver_port = 9999

# Do we have one single long line?
long_line = False
# Type of installation (see details in data_generator)
type = 1

ledTCP = None
data = None

barWidth = 1


def setup():
    global ledTCP
    global data
    size(width, height)
    colorMode(HSB, width, height, 100)
    noStroke()
    background(0)
    ledTCP, data = prepare(Client, receiver_IP, receiver_port, height, width, framerate, brightness)
    send_TCP(ledTCP, data, long_line, type)


def draw():
    whichBar = mouseX / barWidth
    barX = whichBar * barWidth
    fill(barX, mouseY, random.randint(0, 255))
    rect(barX, 0, barWidth, height)
    send_TCP(ledTCP, data, long_line, type)
