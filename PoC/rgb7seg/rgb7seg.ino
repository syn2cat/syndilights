// simulates RGB 7 segments RGBmatrixPanel library.
// Renders 512 colors on a 16x32 RGB LED matrix.
// Library supports 4096 colors, but there aren't that many pixels!

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

#define CLK 8  // MUST be on PORTB!
#define LAT A3
#define OE  9
#define A   A0
#define B   A1
#define C   A2
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

void setup() {
  matrix.begin();

  //myWindows(random(1,21));
  int window = 13;
  int brightness = 255;
  int del = 100;
  // White is 7,7,7 (3bit) 255,255,255 (8bit)
  uint8_t red=255, green=255, blue=255;


while ( true ) {
  myWindows(16, brightness, red, green, blue);
  delay(del);
  // fill the screen with 'black'
  matrix.fillScreen(matrix.Color888(0, 0, 0));
  matrix.swapBuffers(false);
}

}


intRGB7seg (int displayNR, int segment, int r, int g, int b) {
// segment is standard numbered, top=0, DP=7
// displayNR is the display number, counted from the left 0, 1, 2 ,3
// simulated 7seg is like this:
//|1 1 1 1 1 1 0 0 0              |
//|5 4 3 2 1 0 9 8 7 - - - - - - 0|
//|. 0 0 . . X X . X X X X X X X X| 0
//|5 d0  1 X  d1 X X       X      | 1
//|5     1 X     X X  W    X  W   | 2
//|. 6 6 . , X X . X       X      | 3
//|4     2 X     X X   11  X   16 | 4
//|4     2 X     X X       X      | 5
//|. 3 3 7 . X X . X X X X X X X X| 6
//|X       X       X       X      |
//|X  W    X  W    X  W    X  W   |
//|X       X       X       X      |
//|X    2  X    7  X   12  X   17 |
//|X       X       X       X      |
//|X X X X X X X X X X X X X X X X|
int d;
  switch (displayNR) {
  case 0:
    d=12;
    break;
  case 1:
    d=8;
    break;
  case 2:
    d=4;
    break;
  case 3:
    d=0;
    break;
  default:
    return;
  } 
  switch (segment) {
  case 0:
    matrix.fillRect(0,d+1,0,d+2, matrix.Color888(r,g,b));
    break;
  case 1:
    matrix.fillRect(1,d,2,d, matrix.Color888(r,g,b));
    break;
  case 2:
    matrix.fillRect(4,d,5,d, matrix.Color888(r,g,b));
    break;
  case 3:
    matrix.fillRect(6,d+1,6,d+2, matrix.Color888(r,g,b));
    break;
  case 4:
    matrix.fillRect(4,d+3,5,d+3, matrix.Color888(r,g,b));
    break;
  case 5:
    matrix.fillRect(1,d+3,2,d+3, matrix.Color888(r,g,b));
    break;
  case 6:
    matrix.fillRect(3,d+1,3,d+2, matrix.Color888(r,g,b));
    break;
  case 7:
    matrix.fillRect(7,d,7,d, matrix.Color888(r,g,b));
    break;
  default:
    return;
  }
}

  int myWindows (int w, int br, int r, int g, int b){
  
switch (w) {
  case 1:
    // Window 1
    matrix.fillRect(1,12,4,3, matrix.Color888(r,g,b));
    break;
  case 2:
    // Window 2
    matrix.fillRect(7,12,4,3, matrix.Color888(r,g,b));
    break;
  case 3:
    // Window 3
    matrix.fillRect(13,12,4,3, matrix.Color888(r,g,b));
    break;
  case 4:
    // Window 4
    matrix.fillRect(19,12,4,3, matrix.Color888(r,g,b));
    break;
  case 5:
    // Window 5
    matrix.fillRect(25,12,4,3, matrix.Color888(r,g,b));
    break;
  case 6:
    // Window 6
    matrix.fillRect(1,8,4,3, matrix.Color888(r,g,b));
    break;
  case 7:
    // Window 7
    matrix.fillRect(7,8,4,3, matrix.Color888(r,g,b));
    break;
  case 8:
    // Window 8
    matrix.fillRect(13,8,4,3, matrix.Color888(r,g,b));
    break;
  case 9:
    // Window 9
    matrix.fillRect(19,8,4,3, matrix.Color888(r,g,b));
    break;
  case 10:
    // Window 10
    matrix.fillRect(25,8,4,3, matrix.Color888(r,g,b));
    break;
  case 11:
    // Window 11
    matrix.fillRect(1,4,4,3, matrix.Color888(r,g,b));
    break;
  case 12:
    // Window 12
    matrix.fillRect(7,4,4,3, matrix.Color888(r,g,b));
    break;
  case 13:
    // Window 13
    matrix.fillRect(13,4,4,3, matrix.Color888(r,g,b));
    break;
  case 14:
    // Window 14
    matrix.fillRect(19,4,4,3, matrix.Color888(r,g,b));
    break;
  case 15:
    // Window 15
    matrix.fillRect(25,4,4,3, matrix.Color888(r,g,b));
    break;
  case 16:
    // Window 16
    matrix.fillRect(1,0,4,3, matrix.Color888(r,g,b));
    break;
  case 17:
    // Window 17
    matrix.fillRect(7,0,4,3, matrix.Color888(r,g,b));
    break;
  case 18:
    // Window 18
    matrix.fillRect(13,0,4,3, matrix.Color888(r,g,b));
    break;
  case 19:
    // Window 19
    matrix.fillRect(19,0,4,3, matrix.Color888(r,g,b));
    break;
  case 20:
    // Window 20
    matrix.fillRect(25,0,4,3, matrix.Color888(r,g,b));
    break;

  default:
      break;      
}
  }

void loop() {
  // do nothing
}

