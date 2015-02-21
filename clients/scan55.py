# Client program "cellular"

import sys, time
from math import *
from random import randint
from s2llib import *

# setup window area, channels and segments
seg_windows = 12
segments = 8 # number of segments in a window (7 + period/dot/point)
width = 5
height = 5
zlevel = 20
segchannels = 4 # RGBA

cell = syndelights_canvas("s2l\n55<18 ", zlevel, width, height, seg_windows, "10.2.113.175", 8888, "10.2.113.161", 8888, 2)

cell.connect()

# reset/initialize map
cell.reset_windows(0, 0, 0, 0, 0)
cell.reset_segment_windows(0, 255, 255, 255, 255)

# main loop

x = 0
y = 0
w = 0
s = 0
c = []
for i in xrange(width * height):
	# define colour
	if i%3 == 0:
		c = [0, 255, 0, 0, 255]
	elif i%3 == 1:
		c = [0, 0, 255, 0, 255]
	elif i%3 == 2:
		c = [0, 0, 0, 255, 255]
		
	# go to next floor
	if x == width:
		y = y + 1
		x = 0

	# go to next window
	if s == segments:
		w = w + 1
		s = 0
	
	cell.setwin_xy_srgba_array(x, y, c)
	cell.setsegwin_ws_srgba_array(w, s, c)
	cell.draw()
	time.sleep(0.5)
	cell.setwin_xy_srgba_array(x, y, [0, 0, 0, 0, 127])
	cell.setsegwin_ws_srgba_array(w, s, [0, 0, 0, 0, 127])

	# increment windows, segments
	x = x + 1
	s = s + 1


cell.disconnect()
