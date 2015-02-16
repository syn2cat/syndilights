/* syndilights display with arduino and neopixel */
/* !!!!! this does NOT YET use the syndilights format !!!! */
/* Based on http://arduino.cc/en/pmwiki.php?n=Reference/EthernetUDPRead */

#include <SPI.h>        
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>     // https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library


// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            6

// How many NeoPixels are attached to the Arduino?
// at 800Mhz we can send 33333 color values / second
// so to have 60hz refresh the max pixels is 555 so we set it to arbitryrily 512
#define ROWS    5
#define COLS    5
#define NUMPIXELS      ROWS*COLS


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// the mac will be filled out by serial number, stored as ascii hex in the eeprom 6 first bytes
byte mac[] = { '2', 'S', 'L', 0, 0, 0 };  // 2(to) S-yn2-L-ights  (2 because unicast + locally administered)

// IPAddress ip(10, 2, 113, 51);   // not using ip, will use DHCP

unsigned int localPort = 8888;      // local port to listen on

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
byte tomac[4];
void setup() {
   Serial.begin(9600);

  // you need to store into the EEPROM in the first 6 bytes an ascii version of the serial number.
  // read that in and use it for the mac
  int j=0;
  for(int i=0;i<7;i+=2) {
    int v;
    v=(int)(EEPROM.read(i)-'0');
    if (v>9) {v-=7;}  // adjust for A-F are 7 chars later in ascii table
    tomac[j]=v<<4;
    v=(int)(EEPROM.read(i+1)-'0');
    if (v>9) {v-=7;}  // adjust for A-F are 7 chars later in ascii table
    tomac[j++]+=v;
  }
  mac[3]=tomac[0];
  mac[4]=tomac[1];
  mac[5]=tomac[2];
  
  // start the Ethernet and UDP:
// Ethernet.begin(mac,ip);
  Ethernet.begin(mac);     // use DHCP. This eats 2732 of sketch size
  Udp.begin(localPort);
  Serial.println("---");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
  pixels.begin();
  pixels.show(); // Initialize all pixels to 'off'

  for(int x=0;x<COLS;x++) {
   for(int y=0;y<ROWS;y++) {
    // pixels.setPixelColor(indirect(x,y), pixels.Color(r, g, b));
    pixels.setPixelColor(indirect(x,y), pixels.Color(255, 0, 0));
    pixels.show();
    delay(100);
    pixels.setPixelColor(indirect(x,y), pixels.Color(0, 0, 0));
   }
  }
  pixels.show();
  
}


void loop() {
  int packetSize = Udp.parsePacket();
  if(packetSize)
  {
   drawCommand();
  }
}

uint16_t readSubPixelValue(byte in){
  char s[2];
  pinMode(13, OUTPUT);
 
  s[0] = (char)in;
  //if (s[0] == -1) s[0]='0';
 
  s[1] = 0;
  uint16_t r = (uint16_t)strtoul(s,NULL,16);
  return r;
}
 
 
#define S2LHEADERLEN 12-1
void drawCommand() {
  Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
  if(strncmp("s2l\n",packetBuffer,4)) {
    Serial.println("unknown packet recieved");
    return;
  }
  int width=(int)(packetBuffer[4]-'0');
  int height=(int)(packetBuffer[5]-'0');
  Serial.print(" width:");
    Serial.println(width);
  Serial.print("height:");
    Serial.println(height);
  // now we have RGBA values coming in, ignore the A channel
  // rows * (columns * 4 +1) 
  // e.g. 20  * (200 * 4 + )   20 rows of 200 pixels (+1 as there is a \n after each row)
  for(int y=0;y<height;y++) {
    for(int x=0;x<width;x++) {
      // determine r g b for this pixel x,y
      // for(uint16_t i = S2LHEADERLEN-1; i<NUMPIXELS; i++){
      uint16_t r=(uint16_t)packetBuffer[S2LHEADERLEN+(y * (width * 4 + 1)) + (x * 4 + 1)];
      Serial.print((char)r);
      uint16_t g=(uint16_t)packetBuffer[S2LHEADERLEN+(y * (width * 4 + 1)) + (x * 4 + 2)];
      Serial.print((char)g);
      uint16_t b=(uint16_t)packetBuffer[S2LHEADERLEN+(y * (width * 4 + 1)) + (x * 4 + 3)];
      Serial.print((char)b);
      Serial.print(' ');      
      uint16_t pix=indirect(x,y);
      if (pix>=0) {
        pixels.setPixelColor(indirect(x,y), pixels.Color(r, g, b));
      }
    }
    Serial.println();
  }
  pixels.show();
}
 
uint16_t indirect(uint16_t col,uint16_t row) {
  if(col>COLS) { return -1;}
  if(row>ROWS) { return -1;}
  
  // orientation is top to bottom, one right, then bottom to top
  // we may want to add a mirror into this if it's put into a window

  if(col%2) { // odd column, this reverses the direction
    // invert the row
    uint16_t invrow=ROWS-row-1;
    return col*ROWS+invrow;
  } else {    // check if column is even, i.e. the first = 0 = even
    return col*ROWS+row;   
  }
}

