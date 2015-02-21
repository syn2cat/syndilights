# Client program

from socket import *
import sys
import time
from math import *

# Set the socket parameters
local_port = 5004
# direct html udp port
remote_port = 4422
# production port
remote_port = 8888

zlevel=15
# TODO: autodetect interface address for remote application
outgoing_if = "10.2.113.175"
remote_host = "10.2.113.161"

# udp is the default for DGRAM
UDPSock = socket(AF_INET, SOCK_DGRAM)

# we MUST bind, otherwise python will choose a different port for each
# connection
UDPSock.bind((outgoing_if, local_port))

# we will not use connections so we can keep working even if the server
# goes down or refuses connection
#UDPSock.connect((remote_host, remote_port))

hash = "s2l\n55<18 " # 10 bytes

alpha = chr(55)

z_buffer = chr(zlevel) + "\n"

width = 12
height = 8

segments = 8
segwidth = 12
segchannels = 3

sleeptime = 0.1
t = 0

#timer will hold the elapsed time in seconds
frequency = 2*pi/20

while (1):
  #zero out the data buffer
  data = hash
  data += z_buffer

  # windows
  for j in range(0,height):
    for i in range(0,width):
      pixel = 0.5 + 0.5*sin(2*pi*(float(i+1)/width)+t*frequency)*sin(2*pi*(float(j+1)/height)+t*frequency)
      char = chr(int(127 * pixel))
      data += char + char + char + alpha
    data += "\n"
  
  # segment/text display
  for i in range(0,segwidth):
    for j in range(0,segments):
      for a in range(0,segchannels):
        val = 63 + int(63*sin(2*pi*(1+i)*(1+j)*t*frequency/200))
        data += chr(val)
      data += alpha
    data += "\n"
  t+=1
  if not data:
    break
  else:
    UDPSock.sendto(data,(remote_host,remote_port))
  time.sleep(sleeptime)

UDPSock.close()
