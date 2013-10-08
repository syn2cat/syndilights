from s2llib import *
import time

width = 12
height = 8
seg_windows = 12
segments = 8

test = syndelights_canvas("s2l\n<8<18 ", 1, width, height, seg_windows, "127.0.0.1", 5001, "127.0.0.1", 4422, 3)
test = syndelights_canvas("s2l\n<8<18 ", 1, width, height, seg_windows, "127.0.0.1", 5002, "127.0.0.1", 4321, 3)

test.connect()
test.reset_windows(0, 0, 0, 0, 0)
test.reset_segment_windows(0, 255, 255, 255, 0)

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
	
	test.setwin_xy_srgba_array(x, y, c)
	test.setsegwin_ws_srgba_array(w, s, c)
	test.draw()
	time.sleep(0.5)
	test.setwin_xy_srgba_array(x, y, [0, 0, 0, 0, 127])
	test.setsegwin_ws_srgba_array(w, s, [0, 0, 0, 0, 127])

	# increment windows, segments
	x = x + 1
	s = s + 1

test.disconnect()
