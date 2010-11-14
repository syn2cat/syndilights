# Client program

from socket import *

# Set the socket parameters
host = "localhost"
port = 4321
buf = 382
addr = (host,port)

# Create socket
UDPSock = socket(AF_INET,SOCK_DGRAM)

# Send messages
while (1):
  data = raw_input('>> ')
  if not data:
    break
  else:
    UDPSock.sendto(data,addr)

UDPSock.close()                    