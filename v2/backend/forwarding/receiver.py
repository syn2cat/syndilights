#!/usr/bin/env python

import socketserver
import redis


class MyTCPHandler(socketserver.BaseRequestHandler):
    """
    The RequestHandler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def get_config(self):
        self.imgsize = int(self.r.hget('config', 'imgsize'))

    def handle(self):
        print('Start receiving from {}...'.format(self.client_address[0]))
        self.r = redis.Redis()
        self.get_config()
        while True:
            data = self.request.recv(self.imgsize)
            print(len(data))
            self.r.lpush('new', data)
            if len(data) == 0:
                break
        print('... Done with {}.'.format(self.client_address[0]))


if __name__ == "__main__":
    HOST, PORT = "localhost", 9999

    # Create the server, binding to localhost on port 9999
    server = socketserver.TCPServer((HOST, PORT), MyTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    server.serve_forever()
