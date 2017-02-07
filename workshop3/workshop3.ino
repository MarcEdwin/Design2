#define ledPin1 12
#define ledPin2 13

int counter = 0;

void setup()
{
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  // Initialize timer1
//  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 62500;            // compare match register 16MHz/256/1Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

  // Initialize timer2
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;

  OCR2A = 249;              // 16MHz / (8000*8) - 1 (must be <256)
  TCCR2A |= (1 << WGM21);   // CTC mode
  TCCR2B |= (1 << CS21);    // 8 prescaler
  TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt

//  interrupts();             // enable all interrupts
}

ISR(TIMER1_COMPA_vect)      // timer compare interrupt service routine
{
  digitalWrite(ledPin1, digitalRead(ledPin1) ^ 1);   // Toggle LED pin. The ^ symbol is a bitwise XOR.
  counter = 0;
}

ISR(TIMER2_COMPA_vect) {    //timer1 interrupt 8kHz toggles pin 9
  counter++;
  if (counter > 2000)
    //    digitalWrite(ledPin2, digitalRead(LOW));            // toggle LED pin
    //  else
    digitalWrite(ledPin2, digitalRead(ledPin2) ^ 1);    // toggle LED pin

}
void loop()
{
}

