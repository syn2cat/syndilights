/*  OctoWS2811 movie2serial.pde - Transmit video data to 1 or more
      Teensy 3.0 boards running OctoWS2811 VideoDisplay.ino
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
*/

// To configure this program, edit the following sections:
//
//  1: change myMovie to open a video file of your choice    ;-)
//
//  2: edit the serialConfigure() lines in setup() for your
//     serial device names (Mac, Linux) or COM ports (Windows)
//
//  3: if your LED strips have unusual color configuration,
//     edit colorWiring().  Nearly all strips have GRB wiring,
//     so normally you can leave this as-is.
//
//  4: if playing 50 or 60 Hz progressive video (or faster),
//     edit framerate in movieEvent().

import processing.serial.*;

float gamma = 1.7;

Serial ledSerial;     // each port's actual Serial port
int[] gammatable = new int[256];
float framerate=0;
int brightness = 4;
int errorCount=0;

PImage img;  // Declare variable "a" of type PImage

void setup() {
  String[] list = Serial.list();
  delay(20);
  println("Serial Ports List:");
  println(list);
  serialConfigure("/dev/ttyACM0");
  if (errorCount > 0) exit();
  for (int i=0; i < 256; i++) {
    gammatable[i] = (int)(pow((float)i / 255.0, gamma) * 255.0 + 0.5);
  }
  size(40, 1);
  img = loadImage("test1.png");
}

// image2data converts an image to OctoWS2811's raw data format.
// The number of vertical pixels in the image must be a multiple
// of 8.  The data array must be the proper size for the image.
void image2data(PImage image, byte[] data) {
  int offset = 3;
  int x, y, xbegin, xend, xinc, mask;
  int pixel[] = new int[8];
  xbegin = 0;
  xend = image.width;
  xinc = 1;
 
  for (x = xbegin; x != xend; x += xinc) {
    for (int i=0; i < 8; i++) {
      // fetch 8 pixels from the image, 1 for each pin
      pixel[i] = colorWiring(image.pixels[x]);
    }
    // convert 8 pixels to 24 bytes
    for (mask = 0x800000; mask != 0; mask >>= 1) {
      byte b = 0;
      for (int i=0; i < 8; i++) {
        if ((pixel[i] & mask) != 0) b |= (1 << i);
      }
      data[offset++] = b;
     }
  } 
}


// translate the 24 bit color from RGB to the actual
// order used by the LED wiring.  GRB is the most common.
int colorWiring(int c) {
  int red = (c & 0xFF0000) >> 16;
  int green = (c & 0x00FF00) >> 8;
  int blue = (c & 0x0000FF);
  red = gammatable[red];
  green = gammatable[green];
  blue = gammatable[blue];
  red = (red * brightness) >> 8;
  green = (green * brightness) >> 8;
  blue = (blue * brightness) >> 8;
  return (green << 16) | (red << 8) | (blue); // GRB - most common wiring
}

// ask a Teensy board for its LED configuration, and set up the info for it.
void serialConfigure(String portName) {
  try {
    ledSerial = new Serial(this, portName);
    if (ledSerial == null) throw new NullPointerException();
    ledSerial.write('?');
  } catch (Throwable e) {
    println("Serial port " + portName + " does not exist or is non-functional");
    errorCount++;
    return;
  }
  delay(50);
  String line = ledSerial.readStringUntil(10);
  if (line == null) {
    println("Serial port " + portName + " is not responding.");
    println("Is it really a Teensy 3.0 running VideoDisplay?");
    errorCount++;
    return;
  }
  String param[] = line.split(",");
  if (param.length != 12) {
    println("Error: port " + portName + " did not respond to LED config query");
    errorCount++;
    return;
  }
}

// draw runs every time the screen is redrawn - show the movie...
void draw() {
  image(img, 0, 0);
  framerate = 30.0;
  byte[] ledData =  new byte[(img.width * img.height * 24) + 3];
  image2data(img, ledData);
  ledData[0] = '*'; // first Teensy is the frame sync master
  int usec = (int)((1000000.0 / framerate) * 0.75);
  ledData[1] = (byte)(usec); // request the frame sync pulse
  ledData[2] = (byte)(usec >> 8); // at 75% of the frame time
  ledSerial.write(ledData); 
}
