// gunsticks little pattern player
// has several routines displaying various patterns
// using x/y coords or window addresses

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

#define CLK 8  // MUST be on PORTB!
#define LAT A3
#define OE  9
#define A   A0
#define B   A1
#define C   A2
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// number of windows in a row (e.g. 12)
#define MAXX 4
// number of stories, eg 8
#define MAXY 5


int lifearray[MAXX][MAXY];
void setup() {
  matrix.begin();

  //myWindows(random(1,21));
  int window = 13;
  int brightness = 255;
  int del = 100;
  // White is 7,7,7 (3bit) 255,255,255 (8bit)
  uint8_t red=255, green=255, blue=255;

for(int y=0;y<MAXY;y++)
  for(int x=0;x<MAXX;x++)
    lifearray[x][y]=0;

// now load a pattern into the life buffer
//  X   => 1,0
// X    => 0,1
// XXX  => 0-2,2
int p[]={1,0,0,1,0,2,1,2,2,2,-1};

int n=0;
while (p[n]!=-1)
{
  lifearray[p[n]][p[n+1]]=1;
  n+=2;
}

int x=0;
int y=0;
while ( true ) {
  for(y=0;y<MAXY;y++)
    for(x=0;x<MAXX;x++) 
    {
      int count=0;
      int lifebuf[MAXX][MAXY];
      // copy the playfield into temp buffer
      for(int j=0;j<MAXY;j++)
        for(int i=0;i<MAXX;i++)
          lifebuf[i][j]=lifearray[i][j];

      // now we count cells neighbors
      count+=lifebuf[(x-1+MAXX)%MAXX][(y-1+MAXY)%MAXY];   // top left (wrap around)
      count+=lifebuf[x]              [(y-1+MAXY)%MAXY];   // top
      count+=lifebuf[(x+1+MAXX)%MAXX][(y-1+MAXY)%MAXY];   // top right
      count+=lifebuf[(x-1+MAXX)%MAXX][y];                 // left
      count+=lifebuf[(x+1+MAXX)%MAXX][y];                 // right
      count+=lifebuf[(x-1+MAXX)%MAXX][(y+1+MAXY)%MAXY];   // bottom left 
      count+=lifebuf[x]              [(y+1+MAXY)%MAXY];   // bottom 
      count+=lifebuf[(x+1+MAXX)%MAXX][(y+1+MAXY)%MAXY];   // bottom right 
      // dependent on neibours do something
      if(count==3) {       // birth with 3
        lifearray[x][y]=1;
      } else { 
        if (count != 2) {  // stay same with 2
          lifearray[x][y]=0; // die with anything else
        }
      }
    }


  // display the array
  for(y=0;y<MAXY;y++)
    for(x=0;x<MAXX;x++) {
      if(lifearray[x][y]==1)
        myWindows(xyToWindow(x,y), brightness, red, green, blue);
    }
     }
  delay(del);
  // fill the screen with 'black'
  matrix.fillScreen(matrix.Color888(0, 0, 0));
  matrix.swapBuffers(false);
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

