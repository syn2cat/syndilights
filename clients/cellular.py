# Client program "cellular"

from socket import *
import sys, time
from math import *
from random import randint
from copy import deepcopy

# Set the socket parameters
local_port = 5001

# UDP webserver port to bypass frameserver for testing
remote_port = 4422
# port to frameserver
remote_port = 4321

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
hash = "s2l\n<8<18 " # 10 bytes

# plus 2 bytes below
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
sleeptime = 100 # milliseconds
frequency = 1000 / sleeptime # frames per second

# function resets map to zero
def set_wdata(r, g, b):
  global wdata
  wdata = []

  for x in xrange(width):
    wdata.append([0] * height)
    for y in xrange(height):
      # create an SRGBA array at each location
      # where S is the state, and RGBA is colour
      wdata[x][y] = [0, r, g, b, 1]

# function resets segment display
def set_sdata(r, g, b):
  global sdata
  sdata = []

  for s in xrange(segwindows): 
    sdata.append([0] * segments)
    for seg in xrange(segments):
      # create an SRGBA array at each location
      # where S is the state, and RGBA is colour
      sdata[s][seg] = [0, r, g, b, 1]

# convert [r,g,b,a] to chr string
def i2c(rgba):
  # ignore rgba[0] because it is the state
  return chr(int(rgba[1])) + chr(int(rgba[2])) + chr(int(rgba[3])) + chr(int(rgba[4])) 

# determine if a cells neighbour is active (any will do)
def has_active_neighb(x, y):
	# check the cell to the left
	if x > 0 and wdata[x - 1][y][0] > 0:
		return True
	# check the cell above
	if y > 0 and wdata[x][y - 1][0] > 0:
		return True
	# check the cell to the right
	if x < (width - 1) and wdata[x + 1][y][0] > 0:
		return True
	# check the cell below
	if y < (height - 1) and wdata[x][y + 1][0] > 0:
		return True

# function sends wdata and sdata state to server
def send_update():
  #zero out the data buffer
  data = hash
  data = data + z_buffer # 12 bytes

  # write frame data
  for y in xrange(height):
    for x in xrange(width):
      data = data + i2c( wdata[x][y] )
      #print str(wdata[x][y]) + " ",
    #print "\n"
    data = data + "\n"

  # write segment window displays
  for s in xrange(segwindows):
    for seg in xrange(segments):
	data = data + i2c( sdata[s][seg] )
    data = data + "\n"

  # print data

  # send the data packet to remote host
  UDPSock.sendto(data,(remote_host,remote_port))

# main loop
while True:
	# reset/initialize map
	set_wdata(67,67,67)
	set_sdata(255,255,255)

	# set start seed
	# get random location
	rx = randint(0, width - 1)
	ry = randint(0, height - 1)
	wdata[rx][ry] = [1,1,0,0,1]
	# print "origin(" + str(rx) + "," + str(ry) + ")"
				
	# main logic loop
	for t in range(23):
		
		# push update to clients
		send_update()

		# create CA buffer
		data_buffer = deepcopy(wdata)

		for y in xrange(height):
			for x in xrange(width):
				# if cell is active, increase its rating/value
				if wdata[x][y][0] > 0:
					# change the state of the cell
					data_buffer[x][y][0] += 16

					# assign stat to red colour
					data_buffer[x][y][1] = data_buffer[x][y][0]

					# check we don't go too high
					# if the state is over 127, reset red
					if data_buffer[x][y][0] > 127:
						# assign max stat to red colour
						data_buffer[x][y][1] = 0
						# assign stat to green colour
						data_buffer[x][y][2] = data_buffer[x][y][0] - 128

					# if the state is over 128*2, reset green 
					if data_buffer[x][y][0] > 255:
						# assign max stat to green colour
						data_buffer[x][y][2] = 0
						# assign stat to blue colour
						data_buffer[x][y][3] = data_buffer[x][y][0] - 255
						
					# if the state is over 128*3, reset blue 
					if data_buffer[x][y][0] > 383:
						data_buffer[x][y][3] = data_buffer[x][y][0]%128

				# if not active, check if adjacent cell is active
				if wdata[x][y][0] == 0 and has_active_neighb(x, y):
					# activate and colour
					data_buffer[x][y][0] = 1
					data_buffer[x][y][1] = 1
					data_buffer[x][y][2] = 0
					data_buffer[x][y][3] = 0


		# set wdata to new buffer
		wdata = data_buffer

		# wait to paint process next frame
		time.sleep(sleeptime/1000.0)

		# end of iteration, restart

	# end of main animation loop

# close socket
UDPSock.close()
