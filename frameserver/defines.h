#ifndef __DEFINES_H_
#define __DEFINES_H_
#define BUFLEN 1024
#define WIDTH 12
#define HEIGHT 7
#define SEGWIDTH 12

// not used for simplicity
//#define SEGHEIGHT 1

struct segment_t {
  char r,g,b;
};

struct frame_t 
{
  char windows[HEIGHT][WIDTH];
  segment_t segments[SEGWIDTH];
};

#endif