#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

// Assign a unique ID to this sensor at the same time
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

float magnitude;

void displaySensorDetails(void)
{
  sensor_t sensor;
  accel.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup(void)
{
#ifndef ESP8266
  while (!Serial);     // will pause until serial console opens
#endif
  Serial.begin(9600);

  // Initialise the sensor
  if (!accel.begin())
  {
    // There was a problem detecting the ADXL345 ... check your connections
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while (1);
  }

  // Display some basic information on this sensor
  //  displaySensorDetails();
}

void loop(void)
{
  // Get a new sensor event
  sensors_event_t event;
  accel.getEvent(&event);

  // Display the results (acceleration is measured in m/s^2)
  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  "); Serial.println("m/s^2 ");

  magnitude = sqrt(pow(event.acceleration.x, 2) + pow(event.acceleration.y, 2) + pow(event.acceleration.z, 2));

  Serial.println(magnitude);

  // Delay before the next sample
  delay(500);
}
