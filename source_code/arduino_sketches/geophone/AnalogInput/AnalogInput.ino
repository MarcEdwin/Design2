int geophone_pin = A5;    // input pin for the geophone
int velocity_reading = 0;  // store reading from the geophone

int velocity_array[250];

void setup() {

    Serial.begin(9600);
}

void loop() {
    for(int i=0; i<250; i++) {
        // read the value from the sensor
        velocity_reading = analogRead(geophone_pin);
        velocity_array[i] = velocity_reading;
        Serial.println(velocity_array[i]);
        delay(4);
    }
    Serial.println("Array is full!");
}
