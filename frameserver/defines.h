#ifndef __DEFINES_H_
#define __DEFINES_H_

// four minutes should be enough (seconds)
// timeout set to two seconds for testing purposes
#define BUFTIMEOUT 2

#define NUMBUFS 1000

#define REMOTE_IP "127.0.0.1"
#define REMOTE_PORT 4422


#define HASH "s2j\n8<18< "
// one byte number + 10 character hash plus newline
#define HEADEROFFSET 12

// 12 windows per floor, 7 floors, Value:Alpha
#define WIDTH 12
#define HEIGHT 7
#define CHANNELS 4

#define WINDOWOFFSET (WIDTH*CHANNELS+1)*HEIGHT

// 8 segments per window, 12 segments per floor, RGBA
#define SEGNUM 8
#define SEGWIDTH 12
#define SEGCHANNELS 4

#define BUFLEN HEADEROFFSET+WINDOWOFFSET+(SEGNUM*SEGCHANNELS+1)*SEGWIDTH

// not used for simplicity
//#define SEGHEIGHT 1

struct frame_t 
{
	unsigned char z;
  unsigned char windows[HEIGHT][WIDTH][CHANNELS];
  unsigned char segments[SEGWIDTH][SEGNUM][SEGCHANNELS];
//  int windows[HEIGHT][WIDTH][CHANNELS];
//  int segments[SEGWIDTH][SEGNUM][SEGCHANNELS];
};

#endif
