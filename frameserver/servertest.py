# Client program

from socket import *
import sys
import time

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
z_buffer = "1" + "\n"

data = z_buffer + display.read()

# Send messages
while (1):
  if not data:
    break
  else:
    UDPSock.sendto(data,(remote_host,remote_port))
  #send ~100 packets per second  
  time.sleep(0.01)

UDPSock.close()
