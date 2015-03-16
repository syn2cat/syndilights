add_library('net')
import math
import jarray

gamma = 1.7
brightness = 4
errorCount = 0
framerate = 30
dimension = 0

# TODO: test with real serial
# https://www.pjrc.com/teensy/td_uart.html

long_line = False

ledTCP = None
data = None

current_px = 0

def TCPConfigure(server, port):
    global ledTCP
    ledTCP = Client(this, server, port)

def image2data(data):
    offset = 0
    pixel_nb = 0
    for x in range(0, height):
        pixel_line = pixels[pixel_nb:pixel_nb+width]
        if long_line and pixel_nb/width%2 == 1:
            pixel_line = reversed(pixel_line)
        for px in pixel_line:
            pixel = [colorWiring(px) for i in range(0, 8)]
            imgmask = 0x800000
            while imgmask != 0:
                b = 0
                for i in range(0, 8):
                    if ((pixel[i] & imgmask) != 0):
                        b |= (1 << i)
                if b > 127:
                    # Convert to signed bytes (expected by jarray)
                    b -= 2**8
                    data[offset] = b
                else:
                    data[offset] = b
                offset += 1
                imgmask >>= 1
            pixel_nb +=1

def colorWiring(c):
    red = (c & 0xFF0000) >> 16
    green = (c & 0x00FF00) >> 8
    blue = (c & 0x0000FF)
    red = gammatable[red] >> 8
    green = gammatable[green] >> 8
    blue = gammatable[blue] >> 8
    return (green << 16) | (red << 8) | (blue)

def send_TCP():
    image2data(data)
    #println(data)
    ledTCP.write(data)

def prepare_data():
    global data
    data = jarray.zeros(dimension * 24, "b")

def setup():
    global gammatable
    global dimension
    size(50, 30)
    dimension = width * height
    frameRate(framerate)
    TCPConfigure("127.0.0.1", 9999)
    if (errorCount > 0):
        exit()
    gammatable = [int((math.pow(i / 255.0, gamma) * 255.0 + 0.5) * brightness) for i in range(0, 256)]
    prepare_data()
    loadPixels()
    for i in range(dimension):
        pixels[i] = color(0, 0, 0)
    updatePixels()
    send_TCP()

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
    send_TCP()
