#ifndef __DEFINES_H_
#define __DEFINES_H_

// four minutes should be enough (seconds)
// timeout set to two seconds for testing purposes
#define BUFTIMEOUT 2

#define NUMBUFS 1000

#define REMOTE_IP "127.0.0.1"
#define REMOTE_PORT 4422


#define HASH "s2l\n<8<18 "
// one byte number + 10 character hash plus newline
#define HEADEROFFSET 12

// 12 windows per floor, 8 floors, Value:Alpha
#define WIDTH 12
#define HEIGHT 8
#define CHANNELS 4

#define WINDOWOFFSET (WIDTH*CHANNELS+1)*HEIGHT

// 12 windows wide, 8 segment per window, RGBA
#define SEGWIDTH 12
#define SEGNUM 8
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
