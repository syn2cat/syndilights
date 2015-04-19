#!/usr/bin/env python3

import socketserver
import redis


class MyTCPHandler(socketserver.StreamRequestHandler):
    """
    The RequestHandler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def _get_config(self):
        self.max_height = int(self.r.hget('config', 'height'))
        self.max_width = int(self.r.hget('config', 'width'))
        self.max_framerate = int(self.r.hget('config', 'max_framerate'))
        self.cur_framerate = int(self.r.hget('config', 'cur_framerate'))
        self.imgsize = int(self.r.hget('config', 'imgsize'))

    def _set_config(self, framerate, height, width):
        self.r.hset('config', 'cur_framerate', framerate)
        self.imgsize = height * width * 24

    def _send_config_to_client(self):
        self.request.sendall(self.max_height.to_bytes(4, byteorder='little'))
        self.request.sendall(self.max_width.to_bytes(4, byteorder='little'))
        self.request.sendall(self.max_framerate.to_bytes(4, byteorder='little'))

    def _receive_client_config(self):
        height = int.from_bytes(self.request.recv(4), byteorder='little')
        width = int.from_bytes(self.request.recv(4), byteorder='little')
        framerate = int.from_bytes(self.request.recv(4), byteorder='little')
        print(height, width, framerate)
        good, reason = self._check_config(height, width, framerate)
        if good:
            self._set_config(framerate, height, width)
        return good, reason

    def _check_config(self, height, width, framerate):
        if height > 0 and height > self.max_height:
            return False, "height has to be between 0 and {}. Current: {}".format(self.max_height, height)
        if width > 0 and width > self.max_width:
            return False, "width has to be between 0 and {}. Current: {}".format(self.max_width, width)
        if framerate > 0 and framerate > self.max_framerate:
            return False, "framerate has to be between 0 and {}. Current: {}".format(self.max_framerate, framerate)
        return True, None

    def handle(self):
        self.r = redis.Redis()
        self._get_config()
        self._send_config_to_client()
        good, reason = self._receive_client_config()
        if not good:
            print(reason)
            return None
        print('Start receiving from {}...'.format(self.client_address[0]))
        while True:
            data = self.rfile.readline().strip()
            if len(data) == 0:
                break
            self.r.lpush('new', data)
        print('... Done with {}.'.format(self.client_address[0]))


if __name__ == "__main__":
    HOST, PORT = "0.0.0.0", 9999

    # Create the server, binding to localhost on port 9999
    server = socketserver.TCPServer((HOST, PORT), MyTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    server.serve_forever()
