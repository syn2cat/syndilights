from s2llib import *
import time

width = 12
height = 8

test = syndelights_canvas("s2l\n<8<18 ", 1, width, height, width, "127.0.0.1", 5001, "127.0.0.1", 4422, 3)
test = syndelights_canvas("s2l\n<8<18 ", 1, width, height, width, "127.0.0.1", 5001, "127.0.0.1", 4321, 3)

test.connect()
test.reset_windows(0, 0, 0, 0, 127)
test.reset_segment_windows(0, 62, 62, 62, 63)


for y in xrange(height):
	for x in xrange(width):
		test.setwin_xy_srgba_array(x, y, [0, 255, 0, 0, 127])
		test.draw()
		time.sleep(0.5)
		test.setwin_xy_srgba_array(x, y, [0, 0, 0, 0, 127])

test.disconnect()
