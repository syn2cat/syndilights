/*  OctoWS2811 VideoDisplay.ino - Video on LEDs, from a PC, Mac, Raspberry Pi
    http://www.pjrc.com/teensy/td_libs_OctoWS2811.html
    Copyright (c) 2013 Paul Stoffregen, PJRC.COM, LLC

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.


  Required Connections
  --------------------
    pin 2:  LED Strip #1    OctoWS2811 drives 8 LED Strips.
    pin 14: LED strip #2    All 8 are the same length.
    pin 7:  LED strip #3
    pin 8:  LED strip #4    A 100 to 220 ohm resistor should used
    pin 6:  LED strip #5    between each Teensy pin and the
    pin 20: LED strip #6    wire to the LED strip, to minimize
    pin 21: LED strip #7    high frequency ringining & noise.
    pin 5:  LED strip #8
    pin 15 & 16 - Connect together, but do not use
    pin 4:  Do not use
    pin 3:  Do not use as PWM.  Normal use is ok.
    pin 12: Frame Sync

    When using more than 1 Teensy to display a video image, connect
    the Frame Sync signal between every board.  All boards will
    synchronize their WS2811 update using this signal.

    Beware of image distortion from long LED strip lengths.  During
    the WS2811 update, the LEDs update in sequence, not all at the
    same instant!  The first pixel updates after 30 microseconds,
    the second pixel after 60 us, and so on.  A strip of 120 LEDs
    updates in 3.6 ms, which is 10.8% of a 30 Hz video frame time.
    Doubling the strip length to 240 LEDs increases the lag to 21.6%
    of a video frame.  For best results, use shorter length strips.
    Multiple boards linked by the frame sync signal provides superior
    video timing accuracy.

    A Multi-TT USB hub should be used if 2 or more Teensy boards
    are connected.  The Multi-TT feature allows proper USB bandwidth
    allocation.  Single-TT hubs, or direct connection to multiple
    ports on the same motherboard, may give poor performance.
*/

// https://stackoverflow.com/questions/18806141/move-object-creation-to-setup-function-of-arduino

#include "OctoWS2811.h"

int ledsPerStrip = 0;

int count = 0;
DMAMEM int* displayMemory;
int* drawingMemory;
elapsedMicros elapsedUsecSinceLastFrameSync = 0;

const int config = WS2811_800kHz; // color config is on the PC side

OctoWS2811 leds;

// Required for soft reboot
#define RESTART_ADDR       0xE000ED0C
#define READ_RESTART()     (*(volatile uint32_t *)RESTART_ADDR)
#define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.setTimeout(5000);
  Serial.readBytes((char *)&ledsPerStrip, sizeof(ledsPerStrip));
  Serial.write((char *)&ledsPerStrip, sizeof(ledsPerStrip));
  digitalWrite(13, LOW);
  Serial.setTimeout(50);
  displayMemory = new int[ledsPerStrip*6];
  drawingMemory = new int[ledsPerStrip*6];
  leds.attach(ledsPerStrip, displayMemory, drawingMemory, config);
  leds.begin();
  leds.show();
}

void loop() {
  int startChar = Serial.read();
  
  if (startChar == '*') {
    unsigned int startAt = micros();
    unsigned int usecUntilFrameSync = 0;
    count = Serial.readBytesUntil('#', (char *)drawingMemory, sizeof(int) * ledsPerStrip*6 + 1);
    digitalWrite(13, HIGH);
    leds.show();
    digitalWrite(13, LOW);
  }
  else if (startChar == '!') {
    WRITE_RESTART(0x5FA0004);
  }
  else if (startChar >= 0) {
    // discard unknown characters
  }
}

