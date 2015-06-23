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
framerate = 10
brightness = 0.05
#####################################
receiver_IP = "10.2.113.211"
#receiver_IP = "dummy"
receiver_port = 9999

# Do we have one single long line?
long_line = True
# Type of installation (see details in data_generator)
type = 0

ledTCP = None
data = None

barWidth = 1


def setup():
    global ledTCP
    global data
    # for the text example
    global f
    size(width, height)
    colorMode(HSB, width, height, 100)
    frameRate(framerate)

    # For the text examples
    f = createFont("Comic", 5, True)
    ledTCP, data = prepare(Client, receiver_IP, receiver_port, height, width, framerate, brightness)
    send_TCP(ledTCP, data, long_line, type)

def line_mouse():
    whichBar = mouseX / barWidth
    barX = whichBar * barWidth
    fill(barX, mouseY, random.randint(0, 255))
    rect(barX, 0, barWidth, height)

def ellipse_mouse():
  if mousePressed:
    fill(150)
  else:
    fill(0)
  ellipse(mouseX, mouseY, 2, 2)

def draw_text():
    textFont(f,8)
    textAlign(CENTER)
    text("Level 2", width/2, 7)

def advanced_text():
    noStroke();
    smooth();
    x = 1
    background(100)
    message = "Level2"
    for i in range(len(message)):
        textSize(random.randint(6, 8))
        text(message[i], x, height - 1)
        x += textWidth(message[i]) + 1

def draw():
    advanced_text()
    send_TCP(ledTCP, data, long_line, type)
