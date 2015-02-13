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
#define NUMPIXELS      25
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
 
uint16_t brightness = 10;

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
 
 
void drawCommand() {
  Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
  for(uint16_t i = 0; i<NUMPIXELS; i++){
    byte r=(byte)packetBuffer[i*3];
    byte g=(byte)packetBuffer[i*3+1];
    byte b=(byte)packetBuffer[i*3+2];
    pixels.setPixelColor(indirect(i), pixels.Color(readSubPixelValue(r), readSubPixelValue(g), readSubPixelValue(b)));
  }
  pixels.show();
}
 
#define ROWS 5
uint16_t indirect(uint16_t in) {
  if((in/ROWS)%2) {
    uint16_t t;
    t=in/ROWS;
    t*=ROWS;
    t+=ROWS-(in%ROWS)-1;
    return t;
  } else {
    return in;
  }
}
