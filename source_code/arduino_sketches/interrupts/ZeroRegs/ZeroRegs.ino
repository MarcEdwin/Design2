/*
  Prints the configuration registers for the Arduino Zero (and similar boards).

  Intended Audience: advanced users who have already looked at the SAM D21 datasheet.
*/

#include <ZeroRegs.h>
void setup() {
  Serial.begin(9600);
  while (! Serial) {}  // wait for serial monitor to attach
  ZeroRegOptions opts = {Serial, false};
  printZeroRegs(opts);
}

// Don't forget the loop function or else the program won't compile.
void loop() {

}

