#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "game.h"

void setup() {
  Serial.begin(9600);
  Serial.println("Arduino gestartet!"); // DEBUG
  setupGame();
  Serial.println("setupGame() aufgerufen!"); // DEBUG
  startGame();
  Serial.println("Spiel gestartet");
}

void loop() {
  // wird von FreeRTOS uebernommen

}