// gunsticks building life
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

// number of cells in a row (e.g. 12)
#define MAXX 12
// number of rows, eg 8
#define MAXY 8

  uint8_t red=255, green=255, blue=255;

int lifearray[MAXX][MAXY];
void setup() {
  matrix.begin();
  Serial.begin(9600);
  //myWindows(random(1,21));
  int window = 13;
  int brightness = 255;
  int del = 100;
  // White is 7,7,7 (3bit) 255,255,255 (8bit)

for(int y=0;y<MAXY;y++)
  for(int x=0;x<MAXX;x++)
    lifearray[x][y]=0;

delay(1000);
// now load a pattern into the life buffer
//  X   => 1,0
// X    => 0,1
// XXX  => 0-2,2
// glider
int p[]={
1,0,0,1,0,2,1,2,2,2,
//-1};
// block
// int p[]={
// 5,5,5,6,6,6,6,5,
// -1};
// flipper
// int p[]={
// 5,6,6,6,7,6,
//-1};
// pento1000
// X
// XXX
//  X
// int p[]={0,0,1,0,1,1,1,2,2,1,-1};
// box
// int p[]={
//3,3,4,3,5,3,5,4,5,5,4,5,3,5,3,4,
//-1};
// acorn
//int p[]={ 
// 1,4,2,4,2,2,4,3,5,4,6,4,7,4, 
-1};

int n=0;
while (p[n]!=-1)
{
  lifearray[p[n]][p[n+1]]=1;
        xyToWindowLight(p[n],p[n+1], red, green, blue);
  n+=2;
}


delay(2000);
int x=0;
int y=0;
int birth=0;   // how many cells are new
int generations=0;
while ( true ) {
  delay(100);
      int lifebuf[MAXX][MAXY];


  for(int j=0;j<MAXY;j++) {
        for(int i=0;i<MAXX;i++) {
          lifebuf[i][j]=lifearray[i][j];
          if(lifearray[i][j]==1) {
//            Serial.print("X");
          } else {
//            Serial.print(".");
          }
        }
//        Serial.println("");
      }

  for(y=0;y<MAXY;y++)
    for(x=0;x<MAXX;x++) 
    {
      int count=0;
      // copy the playfield into temp buffer
      // now we count cells neighbors
      count =lifebuf[(x-1+MAXX)%MAXX][(y-1+MAXY)%MAXY];   // top left (wrap around)
      count+=lifebuf[x]              [(y-1+MAXY)%MAXY];   // top
      count+=lifebuf[(x+1+MAXX)%MAXX][(y-1+MAXY)%MAXY];   // top right
      count+=lifebuf[(x-1+MAXX)%MAXX][y];                 // left
      count+=lifebuf[(x+1+MAXX)%MAXX][y];                 // right
      count+=lifebuf[(x-1+MAXX)%MAXX][(y+1+MAXY)%MAXY];   // bottom left 
      count+=lifebuf[x]              [(y+1+MAXY)%MAXY];   // bottom 
      count+=lifebuf[(x+1+MAXX)%MAXX][(y+1+MAXY)%MAXY];   // bottom right 
      // dependent on neibours do something
      if(count==3) {       // birth with 3
        if(lifebuf[x][y]==0) {birth++;}
        lifearray[x][y]=1;
      } else { 
        if (count != 2) {  // stay same with 2
          lifearray[x][y]=0; // die with anything else
        }
      }
    }

  if (( birth < 2 ) || (generations++ > 200)) {
    generations=0;
    for(int i=10;i>0;i--) {
       x=random(MAXX);
       y=random(MAXY); 
       lifearray[x][y]=1; // birth
         displayArray();
    delay(500);

    }
  }
  birth=0;


  // display the array
  displayArray();
  delay(del);
  // fill the screen with 'black'
//  matrix.fillScreen(matrix.Color888(0, 0, 0));
//  matrix.swapBuffers(false);
  }

}
void displayArray() {
  int x,y;
  for(y=0;y<8;y++)
    for(x=0;x<8;x++) {
      if(lifearray[x][y]==1)
        xyToWindowLight(x,y, red, green, blue);
      else
        xyToWindowLight(x,y, 0,0,0);
    }
  }

int xyToWindowLight(int x, int y, int r, int g, int b) {
  // this lights the window the building true xy coords like a screen
  // so pixel 0,0 is top left
  // pixel 3,4 is bottom right
  // 1,0->6 =1 + x*5 + y
  // 3,0->16=1 x x*5
  // 2,3->14=1 +x*5 +y
  // building pixel 0,0 (top left) is matrix pixel 0,15,1,15
  // building pixel 1,0 is matrix pixel 0,13,1,13
  // building pixel 2,0 is matrix pixel 0,11,1,11
  // building pixel x,0 is matrix pixel 0,15-2*x,1,15-2*x
  // building pixel 0,1 is matrix pixel 3,15-4,15
  // building pixel 0,y is matrix pixel y*3,15,y*3+1,15

  matrix.drawLine(y*4,15-2*x,y*4+1,15-2*x, matrix.Color888(r,g,b));
 // return (1+x*5+y);
}


void loop() {
  // do nothing
}

