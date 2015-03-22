#!/usr/bin/env python3

from PIL import Image

import socket
import random
import time

HOST, PORT = "localhost", 9999

nb_images = 10

for i in range(nb_images):
    r = random.randint(0, 255)
    g = random.randint(0, 255)
    b = random.randint(0, 255)
    img1 = Image.new('RGBA', (100, 100), (r, g, b, 128))
    data = img1.tobytes()

    try:
        # Create a socket (SOCK_STREAM means a TCP socket)
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((HOST, PORT))

        # Connect to server and send data
        sock.sendall(bytes(data))
        time.sleep(2)

    finally:
        sock.close()
