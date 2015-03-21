import jarray
import time
from data_generator import image2data

def receive_config(socket):
    max_height = jarray.zeros(1, "b")
    max_width = jarray.zeros(1, "b")
    max_framerate = jarray.zeros(1, "b")
    while True:
        available_bytes = socket.available()
        if available_bytes > 0:
            break
        time.sleep(1)
    socket.readBytes(max_height)
    socket.readBytes(max_width)
    socket.readBytes(max_framerate)
    return max_height[0], max_width[0], max_framerate[0]

def send_config(socket, height, width, framerate):
    socket.write(height)
    socket.write(width)
    socket.write(framerate)

def send_TCP(socket, data, long_line, gammatable):
    image2data(data, long_line, gammatable)
    socket.write(data)
