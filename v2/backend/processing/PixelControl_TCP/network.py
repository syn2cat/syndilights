import jarray
import time
import struct
from data_generator import image2data

def receive_config(socket):
    max_height = jarray.zeros(4, "b")
    max_width = jarray.zeros(4, "b")
    max_framerate = jarray.zeros(4, "b")
    while True:
        available_bytes = socket.available()
        if available_bytes > 0:
            break
        time.sleep(1)
    socket.readBytes(max_height)
    socket.readBytes(max_width)
    socket.readBytes(max_framerate)
    max_height = struct.unpack("<i", max_height)[0]
    max_width = struct.unpack("<i", max_width)[0]
    max_framerate = struct.unpack("<i", max_framerate)[0]
    return max_height, max_width, max_framerate

def send_config(socket, height, width, framerate):
    socket.write(struct.pack('<i', height))
    socket.write(struct.pack('<i', width))
    socket.write(struct.pack('<i', framerate))

def send_TCP(socket, data, long_line):
    image2data(data, long_line)
    # print data
    socket.write(data)
