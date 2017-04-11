// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include "lsm303dlhc.h"

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(0); // create accelerometer object

bool Lsm_init_result = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
void initLsm(void)
{
	Serial.println("Checking for the presence of the LSM303DLHC accelerometer.");
	Lsm_init_result = accel.begin();
  	// Initialise the sensor
	if (Lsm_init_result)
	{
		Serial.println("Found and initialized the LSM303DLHC accelerometer.");
	}
	else
	{
		Serial.println("Warning! LSM303DLHC accelerometer not found.");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void getNextSample(float* AccelerationX, float* AccelerationY, float* AccelerationZ)
{
	sensors_event_t event;
	accel.getEvent(&event);

	// Parsing of this data is offloaded to the cloud
	*AccelerationX = event.acceleration.x;
	*AccelerationY = event.acceleration.y;
	*AccelerationZ = event.acceleration.z;
}

