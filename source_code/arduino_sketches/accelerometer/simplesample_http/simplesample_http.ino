// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*
	Device specifications:
	1. We are measuring up to 100 Hz
	2. Therefore, we must to capture 200 acceleration samples every second
	3. We are going to perform an FFT every 2 seconds (400 points)
*/

/* Azure libraries */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <SPI.h>
#ifdef ARDUINO_ARCH_ESP8266
// for ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_SAMD_FEATHER_M0)
// for Adafruit WINC1500
#include <Adafruit_WINC1500.h>
#include <Adafruit_WINC1500SSLClient.h>
#include <Adafruit_WINC1500Udp.h>
#include <NTPClient.h>
#else
#include <WiFi101.h>
#include <WiFiSSLClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#endif

#ifdef ARDUINO_SAMD_FEATHER_M0
// For the Adafruit WINC1500 we need to create our own WiFi instance
// Define the WINC1500 board connections below.
#define WINC_CS   8
#define WINC_IRQ  7
#define WINC_RST  4
#define WINC_EN   2     // or, tie EN to VCC
// Setup the WINC1500 connection with the pins above and the default hardware SPI.
Adafruit_WINC1500 WiFi(WINC_CS, WINC_IRQ, WINC_RST);
#endif

#include <AzureIoTHub.h>
#include <AzureIoTUtility.h>
#include <AzureIoTProtocol_HTTP.h>

#include "simplesample_http.h"

/* Sensor libraries */
//#define DEBUG

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>


/* Azure variables */
static char ssid[] = "MER";     // your network SSID (name)
static char pass[] = "Exodia12";    // your network password (use for WPA, or use as key for WEP)

// In the next line we decide each client ssl we'll use.
#ifdef ARDUINO_ARCH_ESP8266
static WiFiClientSecure sslClient; // for ESP8266
#elif defined(ARDUINO_SAMD_FEATHER_M0)
static Adafruit_WINC1500SSLClient sslClient; // for Adafruit WINC1500
#else
static WiFiSSLClient sslClient;
#endif

static AzureIoTHubClient iotHubClient;

/* Sensor varibles */
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(0); // create accelerometer object

double magnitude; // store the magnitude of acceleration
int counter = 0;
int milliseconds;

const int samples = 200;
int seconds_of_data = 1;
int seconds = 0;



void setup()
{
	// Initialise the sensor
	if (!accel.begin())
	{
		while (1);
	}

	/* Initialize Azure stuff*/
	initSerial();
	initWifi();
	initTime();
	iotHubClient.begin(sslClient);
}

int reference_millis = millis();
int current_millis;

void loop()
{
	String magnitude_string = "";
	while (seconds < seconds_of_data)
	{
		for (int i=0; i<samples; i++)
		{
			current_millis = millis();
			
			sensors_event_t event;
			accel.getEvent(&event);
		
			magnitude_string += String(current_millis - reference_millis) + "," + String(sqrt(pow(event.acceleration.x, 2) + pow(event.acceleration.y, 2) + pow(event.acceleration.z, 2))) + ";";
		}
		seconds++;
	}
	magnitude_string += "0"; // null character
	int string_length = magnitude_string.length();
	char charBuf[string_length];

	
	magnitude_string.toCharArray(charBuf, string_length);
	simplesample_http_run(charBuf);
	
	#ifdef DEBUG
	while (counter<10)
	{
		milliseconds = millis();
		
		// Get a new sensor event
		sensors_event_t event;
		accel.getEvent(&event);
	
		// Get the XYZ readings
		//Serial.print(event.acceleration.x); Serial.print("    "); Serial.print(event.acceleration.y); Serial.print("    "); Serial.println(event.acceleration.z);
	
		// Calculate the magnitude
		magnitude = sqrt(pow(event.acceleration.x, 2) + pow(event.acceleration.y, 2) + pow(event.acceleration.z, 2));
		Serial.print(milliseconds); Serial.print("    "); Serial.println(magnitude);
	
		counter++;
		delay(100);
	}
	#endif
}

void initSerial() {
	// Start serial and initialize stdout
	Serial.begin(115200);
}

void initWifi() {
#ifdef ARDUINO_SAMD_FEATHER_M0
	// for the Adafruit WINC1500 we need to enable the chip
	pinMode(WINC_EN, OUTPUT);
	digitalWrite(WINC_EN, HIGH);
#endif

	// check for the presence of the shield :
	if (WiFi.status() == WL_NO_SHIELD) {
		Serial.println("WiFi shield not present");
		// don't continue:
		while (true);
	}

	// attempt to connect to Wifi network:
	Serial.print("Attempting to connect to SSID: ");
	Serial.println(ssid);

	// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
	while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
		// unsuccessful, retry in 4 seconds
		Serial.print("failed ... ");
		delay(4000);
		Serial.print("retrying ... ");
	}

	Serial.println("Connected to wifi");
}

void initTime() {
#if defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_FEATHER_M0)
#ifdef ARDUINO_SAMD_FEATHER_M0
	Adafruit_WINC1500UDP ntpUdp; // for Adafruit WINC1500
#else
	WiFiUDP ntpUdp;
#endif
	NTPClient ntpClient(ntpUdp);

	ntpClient.begin();

	while (!ntpClient.update()) {
		Serial.println("Fetching NTP epoch time failed! Waiting 5 seconds to retry.");
		delay(5000);
	}

	ntpClient.end();

	unsigned long epochTime = ntpClient.getEpochTime();

	Serial.print("Fetched NTP epoch time is: ");
	Serial.println(epochTime);

	iotHubClient.setEpochTime(epochTime);

#elif ARDUINO_ARCH_ESP8266
	time_t epochTime;

	configTime(0, 0, "pool.ntp.org", "time.nist.gov");

	while (true) {
		epochTime = time(NULL);

		if (epochTime == 0) {
			Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
			delay(2000);
		} else {
			Serial.print("Fetched NTP epoch time is: ");
			Serial.println(epochTime);
			break;
		}
	}
#endif
}
