# Client program "cellular"

import sys, time
from math import *
from random import randint
from s2llib import *

# setup window area, channels and segments
seg_windows = 12
segments = 8 # number of segments in a window (7 + period/dot/point)
width = 12
height = 8
zlevel = 20
segchannels = 4 # RGBA

cell = syndelights_canvas("s2l\n<8<18 ", zlevel, width, height, seg_windows, "127.0.0.1", 5001, "127.0.0.1", 4321, 2)

cell.connect()

# reset/initialize map
cell.reset_windows(0, 0, 0, 0, 0)
cell.reset_segment_windows(0, 255, 255, 255, 255)

# main loop
while True:

	# set start seed
	# get random location
	rx = randint(0, width - 1)
	ry = randint(0, height - 1)
				
	cell.setwin_xy_srgba_array(rx, ry, [0, randint(0,127), randint(0, 127), randint(0, 127), 255])
	cell.draw()
	
	time.sleep(0.1)

	# end of main animation loop

cell.disconnect()
