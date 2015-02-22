#!/usr/bon/env python

import socketserver
import redis


class MyTCPHandler(socketserver.BaseRequestHandler):
    """
    The RequestHandler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def handle(self):
        r = redis.Redis()
        data = None
        while True:
            temp = self.request.recv(1024).strip()
            if data is None:
                data = temp
            else:
                data += temp
            if len(temp) == 0:
                break
        print("{} sent a packet".format(self.client_address[0]))
        r.sadd('new', data)


if __name__ == "__main__":
    HOST, PORT = "localhost", 9999

    # Create the server, binding to localhost on port 9999
    server = socketserver.TCPServer((HOST, PORT), MyTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    server.serve_forever()
