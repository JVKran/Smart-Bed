#include "LoadSensor.hpp"

// HX711 circuit wiring
const int dataOutPin = 2;
const int serialClockPin = 3;

LoadSensor scale(dataOutPin, serialClockPin);

void setup() {
  Serial.begin(9600);
  scale.begin();
}

void loop() {
    if (scale.isReady()) {
        long reading = scale.read();
        Serial.print("HX711 reading: ");
        Serial.println(reading);
    } else {
        Serial.println("HX711 not found.");
    }
    delay(1000);
}