# Client program

from socket import *
import sys
import time
import random

# Set the socket parameters
local_port = 5000 + int( sys.argv[1] )
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

display = open('display', 'r')
z_buffer = chr(1) + "\n"

data = z_buffer + display.read()

random.seed()

# Send messages
sleeptime = 0.0001
frequency = 1/sleeptime
i = random.randint(0,frequency-1)
while (1):
  if not data:
    break
  else:
    UDPSock.sendto(data,(remote_host,remote_port))
  #send ~100 packets per second
  i+=1
  time.sleep(sleeptime)
  if i >= frequency:
    time.sleep(3)
    i = random.randint(0,frequency-1)

UDPSock.close()
