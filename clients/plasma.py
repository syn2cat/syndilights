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

alpha = chr(125)

z_buffer = chr(1) + "\n"

width = 7
height = 12

segments = 8
segwidth = 12
segchannels = 4

# Send messages
sleeptime = 0.03
t = 0

#timer will hold the elapsed time in seconds
timer = 0
frequency = 2*pi/200
max = 0.0

while (1):
  #zero out the data buffer
  data = z_buffer
  for i in range(0,width):
    for j in range(0,height):
      pixel = fabs(sin(2*pi*(float(i)/width)+t*frequency)*sin(2*pi*(float(j)/height)+t*frequency))
      data = data + chr(int(254*pixel)) + alpha
    data = data + "\n"
  for i in range(0,segwidth):
    for j in range(0,segments):
      for a in range(0,segchannels):
        data += chr(254)
    data += "\n"
  t+=1
  timer = t*sleeptime
  #print( data )
  #data += segments.read()
  if not data:
    break
  else:
    UDPSock.sendto(data,(remote_host,remote_port))
  time.sleep(sleeptime)

UDPSock.close()
