#include "lsm303dlhc.h"

float accelX, accelY, accelZ;

void setup()
{
	Serial.begin(57600);
	while (!Serial);
	
//	SERCOM0->USART.CTRLA.reg = (1 << SERCOM_USART_CTRLA_MODE_Pos);
//	SERCOM0->USART.CTRLA.reg = (1 << SERCOM_USART_CTRLA_CMODE_Pos);
//	SERCOM0->USART.CTRLA.reg = (1 << SERCOM_USART_CTRLA_RXPO_Pos);
//	SERCOM0->USART.CTRLA.reg = (1 << (SERCOM_USART_CTRLA_RXPO_Pos+1));
//	SERCOM0->USART.CTRLA.reg = (1 << (SERCOM_USART_CTRLA_TXPO_Pos+1));
//	SERCOM0->USART.BAUD.reg = 57600;
//	SERCOM0->USART.CTRLA.reg = (1 << SERCOM_USART_CTRLA_ENABLE_Pos);
	
	initLsm();
}

void loop()
{
	getNextSample(&accelX, &accelY, &accelZ);
	Serial.print(accelX); Serial.print("    ");
	Serial.print(accelY); Serial.print("    ");
	Serial.println(accelZ);
	delay(100);
//	SERCOM0->USART.DATA.reg = B01010101;
//	delay(1000);
}
