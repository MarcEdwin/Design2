/*
  1. We are only measurign up to 100 Hz at the moment.
  2. Therefore, we have to capture 200 acceleration samples every second.
  3. We perform an FFT every 2 seconds (400 points)
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321); // create accelerometer object

double magnitude;

void setup() {
  while (!Serial);     // pause until serial console opens
  Serial.begin(115200);

  // Initialise the sensor
  if (!accel.begin()) {
    Serial.println("No LSM303 detected ... check your wiring");
    while (1);
  }
}

void loop() {
  // Get a new sensor event
  sensors_event_t event;
  accel.getEvent(&event);

  // Get the XYZ readings
  Serial.print(event.acceleration.x); Serial.print("  "); Serial.print(event.acceleration.y); Serial.print("  "); Serial.println(event.acceleration.z);

  // Calculate the magnitude
  //magnitude = sqrt(pow(event.acceleration.x, 2) + pow(event.acceleration.y, 2) + pow(event.acceleration.z, 2));
  //Serial.println(magnitude);

  delay(25);
}

