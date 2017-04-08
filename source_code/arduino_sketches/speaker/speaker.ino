int speaker_pin = 6;

int num_tones = 10;
int tones[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440, 880};

int ldr_tone = 0;

void setup() {
    for (int i=0; i< num_tones; i++) {
        tone(speaker_pin, tones[i]);
        delay(500);
    }
    noTone(speaker_pin);
}

void loop() {
}
