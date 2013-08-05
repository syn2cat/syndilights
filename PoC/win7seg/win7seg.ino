// simulates RGB 7 segments and 4x4 normal windows
// RGBmatrixPanel library.
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


// http://www.josepino.com/microcontroller/7-segment-ascii
// http://www.raspberrypi.org/phpBB3/viewtopic.php?t=13645&p=142539
uint8_t ascii2segments[] = {
 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x02, /*   ! " # $ % & '  */
 0x80, 0x0f, 0x80, 0x80, 0x04, 0x40, 0x80, 0x80, /* ( ) * + , - . /  */
 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, /* 0 1 2 3 4 5 6 7  */
 0x7F, 0x6F, 0x80, 0x80, 0x80, 0x48, 0x80, 0x27, /* 8 9 : ; < = > ?  */
 0x80, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x3d, /* @ A B C D E F G  */
 0x76, 0x30, 0x1E, 0x76, 0x38, 0x15, 0x37, 0x3f, /* H I J K L M N O  */
 0x73, 0x67, 0x31, 0x6d, 0x78, 0x3e, 0x1C, 0x2A, /* P Q R S T U V W  */
 0x76, 0x6e, 0x5b, 0x39, 0x80, 0x0F, 0x80, 0x08, /* X Y Z [ \ ] ^ _  */
 0x80, 0x5f, 0x7c, 0x58, 0x5e, 0x7b, 0x71, 0x6F, /* ` a b c d e f g  */
 0x74, 0x30, 0x0E, 0x76, 0x06, 0x15, 0x54, 0x5c, /* h i j k l m n o  */
 0x73, 0x67, 0x50, 0x6d, 0x78, 0x1c, 0x1c, 0x2A, /* p q r s t u v w  */
 0x76, 0x6e, 0x5b, 0x39, 0x80, 0x0F, 0x80, 0x08, /* x y z { | } ~    */
};


void setup() {
  matrix.begin();
  //Start Serial for debuging purposes  
  Serial.begin(9600);

  //myWindows(random(1,21));
  int window = 13;
  int brightness = 255;
  int del = 100;
  // White is 7,7,7 (3bit) 255,255,255 (8bit)
  uint8_t red=255, green=255, blue=255;

}




int RGB7seg (int displayNR, int segment, int r, int g, int b) {
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
    return 1;
  } 
  switch (segment) {
  case 0:
//    matrix.drawLine(0,d+1,0,d+2, matrix.Color888(r,g,b));
    matrix.drawLine(0,d,0,d+3, matrix.Color888(r,g,b));
    break;
  case 1:
//    matrix.drawLine(1,d,2,d, matrix.Color888(r,g,b));
    matrix.drawLine(0,d,3,d, matrix.Color888(r,g,b));
    break;
  case 2:
//    matrix.drawLine(4,d,5,d, matrix.Color888(r,g,b));
    matrix.drawLine(3,d,6,d, matrix.Color888(r,g,b));
    break;
  case 3:
//    matrix.drawLine(6,d+1,6,d+2, matrix.Color888(r,g,b));
    matrix.drawLine(6,d,6,d+3, matrix.Color888(r,g,b));
    break;
  case 4:
//    matrix.drawLine(4,d+3,5,d+3, matrix.Color888(r,g,b));
    matrix.drawLine(3,d+3,6,d+3, matrix.Color888(r,g,b));
    break;
  case 5:
//    matrix.drawLine(1,d+3,2,d+3, matrix.Color888(r,g,b));
    matrix.drawLine(0,d+3,3,d+3, matrix.Color888(r,g,b));
    break;
  case 6:
//    matrix.drawLine(3,d+1,3,d+2, matrix.Color888(r,g,b));
    matrix.drawLine(3,d,3,d+3, matrix.Color888(r,g,b));
    break;
  case 7:
    matrix.drawLine(6,d,6,d, matrix.Color888(r,g,b));
    break;
  default:
    return 1;
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
    matrix.fillRect(7+3,12,4,3, matrix.Color888(r,g,b));
    break;
  case 3:
    // Window 3
    matrix.fillRect(13+3,12,4,3, matrix.Color888(r,g,b));
    break;
  case 4:
    // Window 4
    matrix.fillRect(19+3,12,4,3, matrix.Color888(r,g,b));
    break;
  case 5:
    // Window 5
    matrix.fillRect(25+3,12,4,3, matrix.Color888(r,g,b));
    break;
  case 6:
    // Window 6
    matrix.fillRect(1,8,4,3, matrix.Color888(r,g,b));
    break;
  case 7:
    // Window 7
    matrix.fillRect(7+3,8,4,3, matrix.Color888(r,g,b));
    break;
  case 8:
    // Window 8
    matrix.fillRect(13+3,8,4,3, matrix.Color888(r,g,b));
    break;
  case 9:
    // Window 9
    matrix.fillRect(19+3,8,4,3, matrix.Color888(r,g,b));
    break;
  case 10:
    // Window 10
    matrix.fillRect(25+3,8,4,3, matrix.Color888(r,g,b));
    break;
  case 11:
    // Window 11
    matrix.fillRect(1,4,4,3, matrix.Color888(r,g,b));
    break;
  case 12:
    // Window 12
    matrix.fillRect(7+3,4,4,3, matrix.Color888(r,g,b));
    break;
  case 13:
    // Window 13
    matrix.fillRect(13+3,4,4,3, matrix.Color888(r,g,b));
    break;
  case 14:
    // Window 14
    matrix.fillRect(19+3,4,4,3, matrix.Color888(r,g,b));
    break;
  case 15:
    // Window 15
    matrix.fillRect(25+3,4,4,3, matrix.Color888(r,g,b));
    break;
  case 16:
    // Window 16
    matrix.fillRect(1,0,4,3, matrix.Color888(r,g,b));
    break;
  case 17:
    // Window 17
    matrix.fillRect(7+3,0,4,3, matrix.Color888(r,g,b));
    break;
  case 18:
    // Window 18
    matrix.fillRect(13+3,0,4,3, matrix.Color888(r,g,b));
    break;
  case 19:
    // Window 19
    matrix.fillRect(19+3,0,4,3, matrix.Color888(r,g,b));
    break;
  case 20:
    // Window 20
    matrix.fillRect(25+3,0,4,3, matrix.Color888(r,g,b));
    break;

  default:
      break;      
}
  }

int printChar(int display, char c, int r, int g, int b) {
  int segs;
  segs=ascii2segments[c-32];   // get bit pattern what to light
  for(int i=8;i>=0;i--) {
    if(segs & (1<<i)) {
      // RGB7seg(display,i,r,g,b);
    } else {
      RGB7seg(display,i,0,0,0);
    }
  }
  for(int i=8;i>=0;i--) {
    if(segs & (1<<i)) {
      RGB7seg(display,i,r,g,b);
    } else {
      // RGB7seg(display,i,0,0,0);
    }
  }
}

int xyToWindow(int x, int y) {
  // this uses the building true xy coords like a screen
  // so pixel 0,0 is top left
  // pixel 3,4 is bottom right
  // 1,0->6 =1 + x*5 + y
  // 3,0->16=1 x x*5
  // 2,3->14=1 +x*5 +y
  return (1+x*5+y);
}



const char* scrolltext="    WELCOME    S Y N 2 C A T   IN   COLLABORATION   WITH  G R A F F I T I  R E S E A R C H   L A B   L U X E M B O U R G    PRESENTS     S Y N 2 L I G H T S        ";
//  const char* scrolltext="    WELCOME    S Y N 2 C A T        ";

int myclock;

// display machine vars
int state7=0;
int stateW=0;
int j;
int i;
int lasti;
int nextdclock=0;

// windows machine vars
int pos=0;
int lastpos=0;
int lastwin=0;
int patternSwitcher=0;
int x=0;
int lastx=0;
int y=1;  // we have top row occupied by displays
int lasty=1;
uint8_t red=255, green=255, blue=255;
int brightness = 255;
int nextwclock=0;
void loop() {
  delay(1);
  myclock=millis();   // ever running clock
  // manage the 7 segment display routines
  //  Serial.print("state= ");Serial.println(state7);
  switch(state7) {
    case 0: {
      // init of counter rout
      j=2560;
      state7++;
//     Serial.println("init counter");
    }
    case 1: {
//      Serial.print("counter= ");Serial.println(j);
      //count down routine
      // send data, left char first
      char dataOut[5];
      sprintf(dataOut,"%04d",j);
      for(int i=0; i<4; i++) {
        printChar(i,dataOut[i],255*(i%2),255*(i%3),sin(j/200)*100)+55;
      }
      j--;
//      Serial.print("    counter= ");Serial.println(j);
      if(j<0) {state7++ ; }
      matrix.swapBuffers(false);
      break;
    }
   
    case 2: {
      // scroller init
      state7++;
      i=0;
    }
    case 3: { 
      // scroller
      if(myclock%200 == 0) {
        printChar(0,scrolltext[i],255*(i%3),255*(i%2),255);
        printChar(1,scrolltext[i+1],255*((i+1)%3),255*((i+1)%2),200);
        printChar(2,scrolltext[i+2],255*((i+2)%3),255*((i+2)%2),150);
        printChar(3,scrolltext[i+3],255*((i+3)%3),255*((i+3)%2),100);
        if(scrolltext[i+5] == 0) { state7++ ; }
        i++;
      }
      break;
    }
    case 4: {
      // pattern init
      i=0;
      lasti=0;
      state7++;
    }
    case 5: { 
//      matrix.fillScreen(matrix.Color888(0, 0, 0));
      if(myclock%200 == 0) {
        // pattern player
        // coords are simply: display, segment
        int pats[]={0,5,0,1,1,5,1,1,2,5,2,1,3,5,3,1,
                    3,2,3,4,2,2,2,4,1,2,1,4,0,2,0,4,
                    -1,-1};
        RGB7seg(pats[lasti],pats[lasti+1],0,0,0);  // clear
        RGB7seg(pats[i],pats[i+1],255,255,255);  // draw
        lasti=i;
        i+=2;
        if(pats[i] == -1) { state7++ ; }
      }
      break;
    }
    default: {
      state7=0;
    }
  } // esac 

if(myclock>nextwclock) {
  nextwclock=myclock+50;
  switch(patternSwitcher) {
    case 0:
    case 1:   // this will repeat the following patterns 3 times
    case 2: {
      // scan through line by line from top to bottom
      myWindows(lastwin, brightness, 0, 0, 0);
      myWindows(xyToWindow(x,y), brightness, red, green, blue);
      lastwin=xyToWindow(x,y);
      if(x++>2) {
        x=0;
        if(y++>3) {
          y=1;
          patternSwitcher++;
        }
      }
      break;
    }
    case 3:
       matrix.fillRect(7,0,25,16, matrix.Color888(0,0,0));
    case 4: {
      // move a pixel around a path
      int path[] =   { 2, 7,12,17,
                       18,19,20,
                       15,10, 5,
                        4, 3, 8,
                        13,14,9,8,
                        13,14,
                         15,10,5,4,3,
                       0};
      myWindows(path[lastpos], brightness, 0, 0, 0);
      myWindows(path[pos], brightness, red, green, blue);
      lastpos=pos;
      pos++; 
      if(path[pos]==0) { pos=0; patternSwitcher++; }
      break;
     }
     case 5:
       x=0;y=1;patternSwitcher++;
       matrix.fillRect(7,0,25,16, matrix.Color888(0,0,0));
     case 6:
     case 7:
     case 8: {
       // top bottom line wiper
      myWindows(xyToWindow(lastx,lasty), brightness, 0,0,0);
      myWindows(xyToWindow(lastx+1,lasty), brightness, 0,0,0);
      myWindows(xyToWindow(lastx+2,lasty), brightness, 0,0,0);
      myWindows(xyToWindow(lastx+3,lasty), brightness, 0,0,0);
      myWindows(xyToWindow(x,y), brightness, red, green, blue);
      myWindows(xyToWindow(x+1,y), brightness, red, green, blue);
      myWindows(xyToWindow(x+2,y), brightness, red, green, blue);
      myWindows(xyToWindow(x+3,y), brightness, red, green, blue);
      lastx=x;lasty=y;
        if(y++>3) {
          y=1;
          patternSwitcher++;
        }
      }
      
      break;
     case 9: 
       matrix.fillRect(7,0,25,16, matrix.Color888(0,0,0));
       x=0;y=1;patternSwitcher=0;
 /*    case 10:     // flash screen
       for(x=0;x<4;x++)
         for(y=0;y<5;y++)
  //         myWindows(xyToWindow(x,y), brightness, red, green, blue);  
         patternSwitcher++;  
       break;
   */  
  default:
        patternSwitcher=0;
      break;      
  }
}  // myclock

}


