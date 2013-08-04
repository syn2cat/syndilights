// Arduino 7 segment x 4 display
 
// Define the LED digit patters, from 0 - 9

// segment layout
//    0
// 5     1
//    6
// 4     2
//    3     7

// steve layout
//    1
// 3     2
//    4
// 6     0
//    5     7

byte steve_converter(byte inval) {
  byte outval=0;
  outval|=(inval & 0x1)<<1;  // seg 0 goes to 1 (plus 1)
  outval|=(inval & 0x2)<<1;  // seg 1 goes to 2 (plus 1)
  outval|=(inval & 0x4)>>2;  // seg 2 goes to 0 (minus 2)
  outval|=(inval & 0x8)<<2;  // seg 3 goes to 5 (plus 2)
  outval|=(inval & 0x10)<<2; // seg 4 goes to 6 (plus 2)
  outval|=(inval & 0x20)>>2; // seg 5 goes to 3 (minus 2)
  outval|=(inval & 0x40)>>2; // seg 6 goes to 4 (minus 2)
  outval|=(inval & 0x80);    // dot 7 stays
  return outval;
}

byte seven_seg_digits[10][7] = { { 1,1,1,1,1,1,0 },  // = 0
                                                           { 0,1,1,0,0,0,0 },  // = 1
                                                           { 1,1,0,1,1,0,1 },  // = 2
                                                           { 1,1,1,1,0,0,1 },  // = 3
                                                           { 0,1,1,0,0,1,1 },  // = 4
                                                           { 1,0,1,1,0,1,1 },  // = 5
                                                           { 1,0,1,1,1,1,1 },  // = 6
                                                           { 1,1,1,0,0,0,0 },  // = 7
                                                           { 1,1,1,1,1,1,1 },  // = 8
                                                           { 1,1,1,0,0,1,1 }   // = 9
                                                           };
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

//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;


void setup() {                
  //Start Serial for debuging purposes	
  Serial.begin(9600);
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  //    shiftOut(dataPin, clockPin, steve_converter(1<<7)); 
}

void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low

  //internal function setup
  int i=0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  //for each bit in the byte myDataOut�
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
  for (i=7; i>=0; i--)  {
    digitalWrite(myClockPin, 0);

    //if the value passed to myDataOut and a bitmask result 
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000 
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {	
      pinState= 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin  
//    delay(10);
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
//    delay(10);
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}


void loop() {
  //count up routine
  for (int j = 0; j < 9999; j++) {
    //ground latchPin and hold low for as long as you are transmitting
    digitalWrite(latchPin, 0);
    // send data, left char first
    char dataOut[5];
    sprintf(dataOut,"%04d",j);
    for(int i=0; i<4; i++) {
      shiftOut(dataPin, clockPin, steve_converter(ascii2segments[dataOut[i]-32])); 
    }
    //return the latch pin high to signal chip that it 
    //no longer needs to listen for information
    delay(500);
    digitalWrite(latchPin, 1);
    delay(50);
  }
}

