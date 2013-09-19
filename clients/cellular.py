# Client program "cellular"

from socket import *
import sys
import time
from math import *
from random import randint

# Set the socket parameters
local_port = 5001

# UDP webserver port to bypass frameserver for testing
remote_port = 4422
# port to frameserver
#remote_port = 4321

# TODO: autodetect interface address for remote application
outgoing_if = "127.0.0.1"
remote_host = "127.0.0.1"

# udp is the default for DGRAM
UDPSock = socket(AF_INET, SOCK_DGRAM)

# we MUST bind, otherwise python will choose a different port for each
# connection
UDPSock.bind((outgoing_if, local_port))

# we will not use connections so we can keep working even if the server
# goes down or refuses connection
#UDPSock.connect((remote_host, remote_port))

# this is our handshake, weak certification
hash = "s2l\n<8<18 "

alpha = chr(255)
z_buffer = chr(1) + "\n"

# frame of 2 dimensional array holding data [r,g,b,a]
wdata = []
# segment display, each segment holds 8 values of [r,g,b,a]
sdata = []

# setup window area, channels and segments
segwindows = 12
segments = 8 # number of segments in a window (7 + period/dot/point)
width = 12
height = 8
segchannels = 4 # RGBA

# timing elements
# timer will hold the elapsed time in seconds
timer = 0
sleeptime = 5000 # milliseconds
frequency = 1000 / sleeptime # frames per second

# function resets map to zero
def zero_wdata():
  for x in xrange(width):
    wdata.append([0] * height)
    for y in xrange(height):
      # create an RGBA array at each location
      wdata[x][y] = [0, 0, 0, 1]

# function resets segment display
def zero_sdata():
  for s in xrange(segwindows):
    sdata.append([0] * segments)
    for seg in xrange(segments):
      sdata[s][seg] = [0, 0, 1, 1]

# convert [r,g,b,a] to chr string
def i2c(rgba):
  return chr(int(255 * rgba[0])) + chr(int(255 * rgba[1])) + chr(int(255 * rgba[2])) + chr(int(255 * rgba[3])) 

# function sends wdata and sdata state to server
def send_update():
  #zero out the data buffer
  data = hash
  data = data + z_buffer

  # write frame data
  for y in xrange(height):
    for x in xrange(width):
      data = data + i2c( wdata[x][y] )
    data = data + "\n"

  # write segment window displays
  for s in xrange(segwindows):
    for seg in xrange(segments):
      # I don't think I understand what the 3 channels are...
      for sc in xrange(segchannels):
	data = data + i2c( sdata[s][seg] )
    data = data + "\n"

  print data

  # send the data packet to remote host
  UDPSock.sendto(data,(remote_host,remote_port))

# clear map
zero_wdata()
zero_sdata()

# set start seed
# get random location
rx = randint(0, width - 1)
ry = randint(0, height - 1)
wdata[rx][ry] = [1,0,0,0.5]

send_update()

exit()

# build loop here
time.sleep(sleeptime/1000)

# old code below ...
# keep to compare for now

t = 0
if True:
  #zero out the data buffer
  data = hash
  data += z_buffer
  for i in range(0,width):
    for j in range(0,height):
      pixel = 0.5+0.5*sin(2*pi*(float(i+1)/width)+t*frequency)*sin(2*pi*(float(j+1)/height)+t*frequency)
      data = data + chr(int(255*pixel)) + alpha
    data = data + "\n"
  for i in range(0,segwindows):
    for j in range(0,segments):
      for a in range(0,segchannels):
	val = chr( 127 + int(128*sin(2*pi*(1+i)*(1+j)*(1+a)*t*frequency/200)))
        data += val + val + val
    data += "\n"
  t+=1

  print data
  UDPSock.sendto(data,(remote_host,remote_port))

# close socket
UDPSock.close()
