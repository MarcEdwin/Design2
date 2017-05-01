#include <SPI.h>
#include <SdFat.h>
#include <Wire.h>
#include "lsm303dlhc.h"
#include <RTClib.h>
#include <string.h>

SdFat SD;
File data_file;
RTC_PCF8523 rtc;

const int chip_select = 10;
const int samples = 250;

int geophone_pin = A5;
//int velocity; // not used
float AccelX, AccelY, AccelZ;

int velocity_array[samples];
float acceleration_array[samples];
String time_array[samples];
int millis_array[samples];
float magnitude;

void setup()
{
	Serial.begin(115200);
//	while (!Serial);
	initLsm();

	if (!rtc.begin())
	{
		Serial.println("Couldn't find RTC");
		while (1);
	}

	// See if the SD card is present and can be initialized
	if (!SD.begin(chip_select))
	{
		Serial.println("Card failed, or not present");
		// Don't do anything more
		return;
	}

	Serial.readString(); // clear buffer
}


int seconds_of_data = 100;
int seconds = 0;

int reference_millis = millis();
int current_millis;

void loop()
{
	/* Read data from sensor(s) and timestamp it */
	if (seconds < seconds_of_data) {

		for(int i=0; i<samples; i++) {

			DateTime now = rtc.now();
			current_millis = millis();

			String timestamp;
			timestamp += String(now.year());
			timestamp += '/';
			timestamp += String(now.month());
			timestamp += '/';
			timestamp += String(now.day());
			timestamp += ' ';
			timestamp += String(now.hour());
			timestamp += ':';
			timestamp += String(now.minute());
			timestamp += ':';
			timestamp += String(now.second());

			time_array[i] = timestamp;
			velocity_array[i] = analogRead(geophone_pin);

			getNextSample(&AccelX, &AccelY, &AccelZ);
			magnitude = sqrt(pow(AccelX, 2) + pow(AccelY, 2) + pow(AccelZ, 2));
			Serial.print(velocity_array[i]); Serial.print("    ");
			Serial.println(magnitude);
			acceleration_array[i] = magnitude;

			millis_array[i] = current_millis - reference_millis;
			delay(1000/samples-2); // gives us a sampling period of about 4 ms ~> 250 Hz
		}


		/* If the file is available, write to it. Else, print an error. */
		data_file = SD.open("sensor_data.csv", FILE_WRITE);

		if (data_file) {

			for (int i=0; i<samples; i++) {
				data_file.print(time_array[i]);
				data_file.print(',');
				data_file.print(velocity_array[i]);
				data_file.print(',');
				data_file.print(acceleration_array[i]);
				data_file.print(',');
				data_file.println(millis_array[i]);
			}
			data_file.close();
		} else {
			Serial.println("error opening geophone_data.csv");
		}

		seconds++;
	}
}
