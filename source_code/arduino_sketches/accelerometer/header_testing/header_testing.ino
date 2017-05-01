#include "lsm303dlhc.h"

float AccelX, AccelY, AccelZ;

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
	getNextSample(&AccelX, &AccelY, &AccelZ);
	Serial.print(AccelX); Serial.print("    ");
	Serial.print(AccelY); Serial.print("    ");
	Serial.println(AccelZ);
	delay(500);
//	SERCOM0->USART.DATA.reg = B01010101;
//	delay(1000);
}
