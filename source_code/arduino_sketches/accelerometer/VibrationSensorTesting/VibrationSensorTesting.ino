/*
  1. We are only measurign up to 100 Hz at the moment.
  2. Therefore, we have to capture 200 acceleration samples every second.
  3. We perform an FFT every 2 seconds (400 points)
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <arduinoFFT.h>

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321); // create accelerometer object
arduinoFFT FFT = arduinoFFT(); // create FFT object

const uint16_t samples = 512;
double samplingFrequency = 200;

double vReal[samples];
double vImag[samples];

double magnitude;

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02

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
  for (uint16_t i = 0; i < samples; i++) {
    // Get a new sensor event
    sensors_event_t event;
    accel.getEvent(&event);
    magnitude = sqrt(pow(event.acceleration.x, 2) + pow(event.acceleration.y, 2) + pow(event.acceleration.z, 2));
    Serial.println(i);
//    Serial.println(magnitude);
    vReal[i] = int8_t(magnitude);
  }
  Serial.println("Data:");
  PrintVector(vReal, samples, SCL_TIME);
  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
  Serial.println("Weighed data:");
  PrintVector(vReal, samples, SCL_TIME);
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
  Serial.println("Computed Real values:");
  PrintVector(vReal, samples, SCL_INDEX);
  Serial.println("Computed Imaginary values:");
  PrintVector(vImag, samples, SCL_INDEX);
  FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
  Serial.println("Computed magnitudes:");
  PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);
  double x = FFT.MajorPeak(vReal, samples, samplingFrequency);
  Serial.println(x, 6);
  while(1); /* Run Once */
  // delay(2000); /* Repeat after delay */
}


void PrintVector(double *vData, uint8_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    double abscissa;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
  break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFrequency);
  break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / samples);
  break;
    }
    Serial.print(abscissa, 6);
    Serial.print(" ");
    Serial.print(vData[i], 4);
    Serial.println();
  }
  Serial.println();
}
