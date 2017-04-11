#include <SPI.h>
#include <SdFat.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <RTClib.h>
#include <string.h>

#define DEBUG

SdFat SD;
File data_file;

RTC_PCF8523 rtc;
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321); // create accelerometer object

// Pin used to access SD card (10 on Feather)
const int chip_select = 10;
const int samples = 250;

int geophone_pin = A5;    // input pin for the geophone
int velocity_reading = 0;  // store reading from the geophone

int velocity_array[samples];
float acceleration_array[samples];
String time_array[samples];
int millis_array[samples];

void setup() {
    
    Serial.begin(9600);
    while(!Serial);

    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    // See if the SD card is present and can be initialized
    if (!SD.begin(chip_select)) {
        Serial.println("Card failed, or not present");
        // Don't do anything more
        return;
    }

    // Initialise the aeccelerometer sensor
    if (!accel.begin()) {
        Serial.println("No LSM303 detected ... check your wiring");
        while (1);
    }

    SD.remove("geophone_data.csv");
    Serial.readString();
}


int seconds_of_data = 5;
int seconds = 0;

int reference_millis = millis();
int current_millis;

void loop() {

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
            
            sensors_event_t event;
            accel.getEvent(&event);
            acceleration_array[i] = sqrt(pow(event.acceleration.x, 2) + pow(event.acceleration.y, 2) + pow(event.acceleration.z, 2));
            
            millis_array[i] = current_millis - reference_millis;
            delay(1000/samples-2); // gives us a sampling period of about 4 ms ~> 250 Hz
        }


        /* If the file is available, write to it. Else, print an error. */        
        data_file = SD.open("geophone_data.csv", FILE_WRITE);
        
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


        #ifdef DEBUG
        if (seconds == (seconds_of_data-1)) {
            /* Re-open the file for reading, then close the file.
            If the file didn't open, print an error.*/
            data_file = SD.open("geophone_data.csv");
            
            if (data_file) {
                Serial.println("geophone_data.csv:");
                // read from the file until there's nothing else in it
                while (data_file.available()) {
                    Serial.write(data_file.read());
                }
                data_file.close();
            } else {
                Serial.println("error opening geophone_data.csv");
            }
        }
        #endif
        
        seconds++;
    }
}
