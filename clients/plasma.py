# Client program

from socket import *
import sys
import time
import random
from math import *

# Set the socket parameters
local_port = 5000
remote_port = 4321

# TODO: autodetect interface address for remote application
outgoing_if = "127.0.0.1"
remote_host = "127.0.0.1"

# udp is the default for DGRAM
UDPSock = socket(AF_INET, SOCK_DGRAM)
UDPSock.bind((outgoing_if, local_port))

# we will not use connections so we can keep working even if the server
# goes down or refuses connection
#UDPSock.connect((remote_host, remote_port))

segments = open('segments','r')

alpha = 1

z_buffer = "1" + "\n"
width = 7
height = 12

# Send messages
sleeptime = 0.04
t = 0
frequency = 2*pi/40
while (1):
  #zero out the data buffer
  data = z_buffer
  for i in range(0,width):
    for j in range(0,height):
      pixel = sin(1.5*pi*(float(i)/width)+t*frequency)*sin(1.5*pi*(float(j)/height)+t*frequency)
      if pixel < 0.25:
        pixel = '.'
      elif pixel < 0.5:
        pixel = '-'
      elif pixel < 0.75:
        pixel = '+'
      elif pixel <= 1.0:
        pixel = '#'
      else:
        pixel = '*'
      data = data + pixel + str(alpha)
    data = data + "\n"
  t+=1
  #data += segments.read()
  if not data:
    break
  else:
    UDPSock.sendto(data,(remote_host,remote_port))
  #send ~100 packets per second
  time.sleep(sleeptime)

UDPSock.close()
