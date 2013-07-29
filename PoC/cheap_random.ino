// testcolors demo for RGBmatrixPanel library.
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
}

int myWindows (int w){
  
    // White is 7,7,7
  uint8_t r=7, g=7, b=7;


switch (w) {
  case 1:
    // Window 1
    matrix.fillRect(1,12,4,3, matrix.Color333(r,g,b));
    break;
  case 2:
    // Window 2
    matrix.fillRect(7,12,4,3, matrix.Color333(r,g,b));
    break;
  case 3:
    // Window 3
    matrix.fillRect(13,12,4,3, matrix.Color333(r,g,b));
    break;
  case 4:
    // Window 4
    matrix.fillRect(19,12,4,3, matrix.Color333(r,g,b));
    break;
  case 5:
    // Window 5
    matrix.fillRect(25,12,4,3, matrix.Color333(r,g,b));
    break;
  case 6:
    // Window 5
    matrix.fillRect(1,8,4,3, matrix.Color333(r,g,b));
    break;
  case 7:
    // Window 5
    matrix.fillRect(7,8,4,3, matrix.Color333(r,g,b));
    break;
  case 8:
    // Window 5
    matrix.fillRect(13,8,4,3, matrix.Color333(r,g,b));
    break;
  case 9:
    // Window 5
    matrix.fillRect(19,8,4,3, matrix.Color333(r,g,b));
    break;
  case 10:
    // Window 5
    matrix.fillRect(25,8,4,3, matrix.Color333(r,g,b));
    break;
  case 11:
    // Window 5
    matrix.fillRect(1,4,4,3, matrix.Color333(r,g,b));
    break;
  case 12:
    // Window 5
    matrix.fillRect(7,4,4,3, matrix.Color333(r,g,b));
    break;
  case 13:
    // Window 5
    matrix.fillRect(13,4,4,3, matrix.Color333(r,g,b));
    break;
  case 14:
    // Window 5
    matrix.fillRect(19,4,4,3, matrix.Color333(r,g,b));
    break;
  case 15:
    // Window 5
    matrix.fillRect(25,4,4,3, matrix.Color333(r,g,b));
    break;
  case 16:
    // Window 5
    matrix.fillRect(1,0,4,3, matrix.Color333(r,g,b));
    break;
  case 17:
    // Window 5
    matrix.fillRect(7,0,4,3, matrix.Color333(r,g,b));
    break;
  case 18:
    // Window 5
    matrix.fillRect(13,0,4,3, matrix.Color333(r,g,b));
    break;
  case 19:
    // Window 5
    matrix.fillRect(19,0,4,3, matrix.Color333(r,g,b));
    break;
  case 20:
    // Window 5
    matrix.fillRect(25,0,4,3, matrix.Color333(r,g,b));
    break;

  default:
      break;      
}
  }

void loop() {
  // do nothing
  myWindows(random(1,21));
  //myWindows(16);
  int del = 100;
  delay(del);
// fill the screen with 'black'
matrix.fillScreen(matrix.Color333(0, 0, 0));
matrix.swapBuffers(false);

}

