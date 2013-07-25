#include "Adafruit_GFX.h"
#include "RGBmatrixPanel.h"

#define CLK 8
#define LAT A3
#define OE  9
#define SIZEOF_BITMAP 24
#define N 0
#define O 1
#define T 2
#define R 3
#define P 4
#define I 5
#define C 6
#define D 7
#define E 8
#define S 9
#define G 10

#define Y 3


extern prog_uchar pacman_bitmaps[];
extern prog_uchar letter_bitmaps[];
extern prog_uchar invader_bitmaps[];

RGBmatrixPanel matrix(A0, A1, A2, CLK, LAT, OE, true);
uint16_t black = matrix.Color444(0, 0, 0);
uint16_t yellow = matrix.Color444(15, 15, 0);
uint16_t darkyellow = matrix.Color444(1, 1, 0);
uint16_t red = matrix.Color444(15, 0, 0);
uint16_t white = matrix.Color444(15, 15, 15);
uint16_t pink = matrix.Color444(15, 3, 15);
uint16_t palePink = matrix.Color444(15, 8, 15);
uint16_t blue = matrix.Color444(0, 0, 10);
uint16_t cyan = matrix.Color444(0, 15, 15);
uint16_t orange = matrix.Color444(15, 5, 0);
uint16_t darkOrange = matrix.Color444(15, 1, 0);
uint16_t green = matrix.Color444(0, 15, 0);


int direction = 1;
int biteDirection = 1; //opening or closing
int dotsY = 8;
unsigned long loopCounter = 0;
boolean didChange = false;
boolean caught = false;
uint16_t pillColor = palePink;
unsigned char *letterBitmaps[] = {(letter_bitmaps), (letter_bitmaps+18), (letter_bitmaps+36), (letter_bitmaps+54), (letter_bitmaps+72), (letter_bitmaps+94), (letter_bitmaps+104), (letter_bitmaps+122), (letter_bitmaps+140), (letter_bitmaps+158), (letter_bitmaps+176)};


void setup() {
  matrix.begin();
}

void loop() {
  nootropicDesign();
  clear();
  matrix.swapBuffers(false);
  delay(500);
}


void nootropicDesign() {
  int y = 1;
  int offset = 0;
  for(int x=32;x>-140;x--) {
    offset = 0;
    clear();
    offset += drawBitmap(x+offset, y, letterBitmaps[N], darkOrange) + 1;
    offset += drawBitmap(x+offset, y, letterBitmaps[O], darkOrange) + 1;
    offset += drawBitmap(x+offset, y, letterBitmaps[O], darkOrange) + 1;
    offset += drawBitmap(x+offset, y, letterBitmaps[T], darkOrange) + 1;
    offset += drawBitmap(x+offset, y, letterBitmaps[R], darkOrange) + 1;
    offset += drawBitmap(x+offset, y, letterBitmaps[O], darkOrange) + 1;
    offset += drawBitmap(x+offset, y, letterBitmaps[P], darkOrange) + 1;
    offset += drawBitmap(x+offset, y, letterBitmaps[I], darkOrange) + 1;
    offset += drawBitmap(x+offset, y, letterBitmaps[C], darkOrange) + 1;
    offset += 3;
    offset += drawBitmap(x+offset, y, letterBitmaps[D], darkOrange) + 1;
    offset += drawBitmap(x+offset, y, letterBitmaps[E], darkOrange) + 1;
    offset += drawBitmap(x+offset, y, letterBitmaps[S], darkOrange) + 1;
    offset += drawBitmap(x+offset, y, letterBitmaps[I], darkOrange) + 1;
    offset += drawBitmap(x+offset, y, letterBitmaps[G], darkOrange) + 1;
    offset += drawBitmap(x+offset, y, letterBitmaps[N], darkOrange) + 1;
    matrix.swapBuffers(false);
    delay(15);
  }
}

void clear() {
  matrix.fillScreen(black);
}

uint8_t drawBitmap(int x, int y, unsigned char *bmp, uint16_t color) {
  uint8_t width = pgm_read_byte(bmp);
  uint8_t height = pgm_read_byte(bmp+1);
  unsigned char *p = bmp+2;
  uint8_t b;
  uint8_t bit;

  for (uint8_t j=0;j<height;j++) {
    for (uint8_t i=0;i<width;i++) {
      if ((i % 8) == 0) {
	b = pgm_read_byte(p);
	p++;
	bit = 7;
      }
      if ((b >> bit) & 0x1) {
	matrix.drawPixel(x+i, y+j, color);
      }
      bit--;
    }
  }
  return width;
}
