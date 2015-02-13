// WRITES TO SERIAL EEPROM Arduino
//
// Do this only once on an Arduino,
// Write the Serial of the Arduino in the
// First 6 bytes of the EEPROM
// you find the serial by connecting to usb in /dev/serial/by-id
// usb-Arduino__www.arduino.cc__0042_7533030393435171B1A0-if00
// take the last 6 positions 71B1A0
#include <EEPROM.h>
char sID[7]="71B1A0";  // change this to the serial number on the board
#define EEPROM_MIN_ADDR 0
#define EEPROM_MAX_ADDR 511
//
// Returns true if the address is between the
// minimum and maximum allowed values,
// false otherwise.
//
// This function is used by the other, higher-level functions
// to prevent bugs and runtime errors due to invalid addresses.
//
boolean eeprom_is_addr_ok(int addr) {
  return ((addr >= EEPROM_MIN_ADDR) && (addr <= EEPROM_MAX_ADDR));
}


boolean eeprom_write_bytes(int startAddr, const byte* array, int numBytes) {
  // counter
  int i;

  // both first byte and last byte addresses must fall within
  // the allowed range  
  if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes)) {
    return false;
  }

  for (i = 0; i < numBytes; i++) {
    EEPROM.write(startAddr + i, array[i]);
  }

  return true;
}
//
// Writes a string starting at the specified address.
// Returns true if the whole string is successfully written.
// Returns false if the address of one or more bytes
// fall outside the allowed range.
// If false is returned, nothing gets written to the eeprom.
//
boolean eeprom_write_string(int addr, const char* string) {
  // actual number of bytes to be written
  int numBytes;

  // we'll need to write the string contents
  // plus the string terminator byte (0x00)
  numBytes = strlen(string) + 1;

  return eeprom_write_bytes(addr, (const byte*)string, numBytes);
}

//
// Reads the specified number of bytes from the specified address into the provided buffer.
// Returns true if all the bytes are successfully read.
// Returns false if the star or end addresses aren't between
// the minimum and maximum allowed values.
// When returning false, the provided array is untouched.
//
// Note: the caller must ensure that array[] has enough space
// to store at most numBytes bytes.
//
boolean eeprom_read_bytes(int startAddr, byte array[], int numBytes) {
  int i;

  // both first byte and last byte addresses must fall within
  // the allowed range  
  if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes)) {
    return false;
  }

  for (i = 0; i < numBytes; i++) {
    array[i] = EEPROM.read(startAddr + i);
  }

  return true;
}


//
// Reads a string starting from the specified address.
// Returns true if at least one byte (even only the
// string terminator one) is read.
// Returns false if the start address falls outside
// or declare buffer size os zero.
// the allowed range.
// The reading might stop for several reasons:
// - no more space in the provided buffer
// - last eeprom address reached
// - string terminator byte (0x00) encountered.
// The last condition is what should normally occur.
//
boolean eeprom_read_string(int addr, char* buffer, int bufSize) {
  // byte read from eeprom
  byte ch;

  // number of bytes read so far
  int bytesRead;

  // check start address
  if (!eeprom_is_addr_ok(addr)) {
    return false;
  }

  // how can we store bytes in an empty buffer ?
  if (bufSize == 0) {
    return false;
  }

  // is there is room for the string terminator only,
  // no reason to go further
  if (bufSize == 1) {
    buffer[0] = 0;
    return true;
  }

  // initialize byte counter
  bytesRead = 0;

  // read next byte from eeprom
  ch = EEPROM.read(addr + bytesRead);

  // store it into the user buffer
  buffer[bytesRead] = ch;

  // increment byte counter
  bytesRead++;

  // stop conditions:
  // - the character just read is the string terminator one (0x00)
  // - we have filled the user buffer
  // - we have reached the last eeprom address
  while ( (ch != 0x00) && (bytesRead < bufSize) && ((addr + bytesRead) <= EEPROM_MAX_ADDR) ) {
    // if no stop condition is met, read the next byte from eeprom
    ch = EEPROM.read(addr + bytesRead);

    // store it into the user buffer
    buffer[bytesRead] = ch;

    // increment byte counter
    bytesRead++;
  }

  // make sure the user buffer has a string terminator
  // (0x00) as its last byte
  if ((ch != 0x00) && (bytesRead >= 1)) {
    buffer[bytesRead - 1] = 0;
  }

  return true;
}


char buf[7];
void setup()
{
 Serial.begin(9600);
 eeprom_read_string(0,buf, 7); // read from addr 0 into buf 7 chars
 if(strncmp(buf,sID,6) != 0) {   // only write if not yet written
  eeprom_write_string(0, sID);  // write to adress 0 the ID
 }
}

void loop () {
 Serial.println("---");
 Serial.print("old value: ");
 Serial.println(buf);
 Serial.print("new value: ");
 Serial.println(sID);
 if(strncmp(buf,sID,6) != 0) {
    Serial.println("different");
 } else {
    Serial.println("same");
 }
}

