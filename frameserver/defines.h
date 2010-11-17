#ifndef __DEFINES_H_
#define __DEFINES_H_

// four minutes should be enough
#define BUFTIMEOUT 2

#define NUMBUFS 1000

// one number + newline
#define HEADEROFFSET 2

// 12 windows per floor, 7 floors, Value:Alpha
#define WIDTH 12
#define HEIGHT 7
#define CHANNELS 2

#define WINDOWOFFSET (WIDTH*CHANNELS+1)*HEIGHT

// 8 segments per window, 12 segments per floor, RGBA
#define SEGNUM 8
#define SEGWIDTH 12
#define SEGCHANNELS 4

#define BUFLEN HEADEROFFSET+WINDOWOFFSET+ (SEGNUM*SEGCHANNELS+1)*SEGWIDTH

// not used for simplicity
//#define SEGHEIGHT 1



struct frame_t 
{
	unsigned char z;
  unsigned char windows[HEIGHT][WIDTH][CHANNELS];
  unsigned char segments[SEGWIDTH][SEGNUM][SEGCHANNELS];
};

enum modes
{
  FRAME,
  FRAME_VALUES
};

#endif
