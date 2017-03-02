#include <Wire.h>
#include <SparkFun_MMA8452Q.h>

MMA8452Q accel;

void setup(){
  Serial.begin(9600);
  accel.init();
  
  // Initialize timer1
  noInterrupts();           // disable all interrupts
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 62500;            // compare match register 16MHz/256/1Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

  interrupts();             // enable all interrupts
}

ISR(TIMER1_COMPA_vect) {    // timer compare interrupt service routine
//  reading = accel.read();
  sei();
  if (accel.available()) {
    accel.read();
    printCalculatedAccels();
    Serial.println();
  }
}

void loop(){
  
}

void printCalculatedAccels()
{ 
  Serial.print(accel.cx, 3);
  Serial.print("\t");
  Serial.print(accel.cy, 3);
  Serial.print("\t");
  Serial.print(accel.cz, 3);
  Serial.print("\t");
}

