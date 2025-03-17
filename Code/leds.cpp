#include "leds.h"
#include <Arduino.h>

const int ledPins[4] = {8, 9, 10, 11};

void setupLEDs() {
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
}

void turnOnLED(int index) {
  if (index >= 0 && index < 4) {
    digitalWrite(ledPins[index], HIGH);
  }
}

void turnOffLED(int index) {
  if (index >= 0 && index < 4) {
    digitalWrite(ledPins[index], LOW);
  }
}