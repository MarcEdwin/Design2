#include "lsm303dlhc.h"

float AccelX, AccelY, AccelZ;

void setup()
{
	while (!Serial);
	Serial.begin(9600);
	initLsm();
}

void loop()
{
	getNextSample(&AccelX, &AccelY, &AccelZ);
	Serial.print(AccelX); Serial.print("    ");
	Serial.print(AccelY); Serial.print("    ");
	Serial.println(AccelZ);
	delay(500);
}
