add_library('serial')
import math
import struct
import jarray

gamma = 1.7

framerate = 0.0
brightness = 12
errorCount = 0
s = None
img = None
ledSerial = None

def setup():
    global img
    global gammatable
    listserials = Serial.list()
    delay(20)
    #print("Serial Ports List:")
    #print(listserials)
    serialConfigure("/dev/ttyACM0") 
    if (errorCount > 0):
        exit()
    gammatable = [math.pow(i / 255.0, gamma) * 255.0 + 0.5 for i in range(0, 256)]
    
    size(40, 1)
    img = loadImage("test3.png")

def image2data(data):
    offset = 3
    dimension = img.width * img.height
    for x in xrange(0, dimension, 1):
        pixel = [colorWiring(img.pixels[x]) for i in range(0, 8)]
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
            offset += 1
            imgmask >>= 1

def colorWiring(c):
    red = (c & 0xFF0000) >> 16
    green = (c & 0x00FF00) >> 8
    blue = (c & 0x0000FF)
    red = gammatable[red]
    green = gammatable[green]
    blue = gammatable[blue]
    red = int(red * brightness) >> 8
    green = int(green * brightness) >> 8
    blue = int(blue * brightness) >> 8
    return (green << 16) | (red << 8) | (blue)

def serialConfigure(portName):
    global ledSerial
    try:
        ledSerial = Serial(this, portName)
        if ledSerial is None:
            raise NullPointerException()
        ledSerial.write('?')
    except Exception as e:
        println("Serial port " + portName + " does not exist or is non-functional")
        errorCount += 1
        return
    delay(50);
    line_serial = ledSerial.readStringUntil(10)
    if (line is None):
        println("Serial port " + portName + " is not responding.")
        println("Is it really a Teensy 3.0 running VideoDisplay?")
        errorCount += 1
        return
    
    param = line_serial.split(",")
    if (len(param) != 12):
        println("Error: port " + portName + " did not respond to LED config query")
        errorCount += 1
        return

def draw():
    global img
    image(img, 0, 0)
    data = jarray.zeros(img.width * img.height * 24 + 3, "b")
    image2data(data)
    data[0] = ord('*')
    framerate = 30.0
    usec = int((1000000.0 / framerate) * 0.75)
    data[1] = (usec % 256) - 2**8
    data[2] = usec >> 8
    ledSerial.write(data)
    
