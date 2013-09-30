# s2llib.py
# Python utility functions for syndelights

from socket import *
import sys, time
#from copy import deepcopy

class syndelights_canvas:
	"""Holds the state of the canvas to be sent"""
	segments_in_window = 8

	def __init__(self, hpc, zb, w, h, seg_wins, hs, hsp, ts, tsp, deblvl):
		# get hash passcode and z-buffer depth
		self.hash_passcode = hpc
		self.zed_buffer = zb

		# main window dimensions
		self.width = w
		self.height = h
		
		# number of windows with segment displays
		self.segmented_windows_num = seg_wins

		# set servers and ports
		self.host_server = hs
		self.host_server_port = hsp
		self.target_server = ts
		self.target_server_port = tsp

		# debugging
		self.debug_level = deblvl

		# make data structures for windows and segments
		self.reset_windows(0,127,127,127, 255)
		self.reset_segment_windows(0, 127, 127, 127, 255)

	
	def reset_windows(self, state, red, green, blue, alpha):
		self.window_data = []
		for y in xrange(self.height):
			self.window_data.append([0] * self.width)
			for x in xrange(self.width):
				self.window_data[y][x] = dict(s=state, r=red, g=green, b=blue, a=alpha)

	def reset_segment_windows(self, state, red, green, blue, alpha):
		self.segmented_windows = []
		for w in xrange(self.segmented_windows_num):
			self.segmented_windows.append([0] * self.segments_in_window)
			for s in xrange(self.segments_in_window):
				self.segmented_windows[w][s] = dict(s=state, r=red, g=green, b=blue, a=alpha)
		
	# convert [r, g, b, a] into a 4 character string
	def int2chr(self, rgba_dict):
		return chr(int(rgba_dict["r"])) + chr(int(rgba_dict["g"])) + chr(int(rgba_dict["b"])) + chr(int(rgba_dict["a"]))

	# draw the data object, send it to the target server
	def draw(self):
		# prep header with hash, z-buffer and new line (12 bytes)
		data = self.hash_passcode + chr(self.zed_buffer) + "\n"

		# package window data
		for y in xrange(self.height):
			for x in xrange(self.width):
				data = data + self.int2chr( self.window_data[y][x] )
			data = data + "\n"

		# package segment data
		for w in xrange(self.segmented_windows_num):
			for s in xrange(self.segments_in_window):
				data = data + self.int2chr( self.segmented_windows[w][s] )
			data = data + "\n"

		if self.debug_level > 2:
			print data

		# send the data through UDP to remote server
		self.UDPSock.sendto(data, (self.target_server, self.target_server_port))

	def setwin_xy_srgba_array(self, x, y, srgba):
		self.window_data[y][x] = dict(s=srgba[0], r=srgba[1], g=srgba[2], b=srgba[3], a=srgba[4])

	def setwin_xy_sval(self, x, y, col, val):
		if col == 's':
			self.window_data[y][x]['s'] = val
		if col == 'r':
			self.window_data[y][x]['r'] = val
		if col == 'g':
			self.window_data[y][x]['r'] = val
		if col == 'b':
			self.window_data[y][x]['r'] = val
		if col == 'a':
			self.window_data[y][x]['r'] = val

	def connect(self):
		self.UDPSock = socket(AF_INET, SOCK_DGRAM)
		self.UDPSock.bind((self.host_server, self.host_server_port))

	def disconnect(self):
		self.UDPSock.close()
