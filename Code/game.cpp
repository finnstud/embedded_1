#include "game.h"
#include "leds.h"
#include "buttons.h"
#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include <queue.h>
#include <stdlib.h>

#define MAX_SEQUENCE 10

QueueHandle_t buttonQueue;
TaskHandle_t gameTaskHandle;
bool wartetAufInput = false;

int sequence[MAX_SEQUENCE];
int sequenceSpeed = 500;
int sequenceLength = 1;
int playerIndex = 0;
bool isPlaying = false;

void generateSequence() {
  for (int i = 0; i < MAX_SEQUENCE; i++) {
    sequence[i] = rand() % 4;
  }
}

void playSequence() {

  wartetAufInput = false;

  for (int i = 0; i < sequenceLength; i++) {
     turnOnLED(sequence[i]);
     vTaskDelay(sequenceSpeed / portTICK_PERIOD_MS);
     turnOffLED(sequence[i]);
     vTaskDelay(sequenceSpeed / 2 / portTICK_PERIOD_MS);
  }

  wartetAufInput = true;

}

void successFeedback() {
  Serial.print("success");
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
     turnOnLED(j);
      }
      vTaskDelay(200 / portTICK_PERIOD_MS);
      for (int j = 0; j < 4; j++) {
        turnOffLED(j);
        }
      vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void failureFeedback() {
  Serial.print("failed");
  for (int j = 0; j < 4; j++) {
    turnOnLED(j);
  }
  vTaskDelay(3000 / portTICK_PERIOD_MS);
  for (int j = 0; j < 4; j++) {
    turnOffLED(j);
  }
}

void gameTask(void *pvParameters) {

  generateSequence();

  while(1) {
    if (!isPlaying) {
      vTaskDelay(100 / portTICK_PERIOD_MS);
      continue;
    }

    playSequence();
    playerIndex = 0;

    while (playerIndex < sequenceLength) {
      if (!wartetAufInput) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        continue;
      }

      int buttonPressed;
      if (xQueueReceive(buttonQueue, &buttonPressed, 5000 / portTICK_PERIOD_MS)) {
        if (buttonPressed == sequence[playerIndex]) {
          playerIndex++;
        } else {
          failureFeedback();
          sequenceLength = 1;
          isPlaying = false;
          break;
        }
      } else {
        failureFeedback();
        sequenceLength = 1;
        isPlaying = false;
        break;
      }
    }

    if (playerIndex == sequenceLength) {
      successFeedback();
      sequenceLength++;
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);

  }
}

void startGame() {
  if (!isPlaying) {
    isPlaying = true;
    sequenceLength = 1;
  }
}

void setupGame() {
  setupLEDs();
  setupButtons();
  Serial.begin(9600);

  srand(analogRead(A0));

  Serial.println("Drücke eine Taste, um die Geschwindigkeit zu wählen!");

  int buttonPressed;

  if (digitalRead(4) == HIGH) {
        sequenceSpeed = 700;
        Serial.println("Langsam (700ms) gewählt!");
        digitalWrite(8, HIGH);
    } else if (digitalRead(5) == HIGH) {
        sequenceSpeed = 500;
        Serial.println("Mittel (500ms) gewählt!");
        digitalWrite(9, HIGH);
    } else if (digitalRead(6) == HIGH) {
        sequenceSpeed = 300;
        Serial.println("Schnell (300ms) gewählt!");
        digitalWrite(10, HIGH);
    } else if (digitalRead(7) == HIGH) {
        sequenceSpeed = 150;
        Serial.println("Sehr schnell (150ms) gewählt!");
        digitalWrite(11, HIGH);
    } else {
        sequenceSpeed = 500;
        Serial.println("Keine Auswahl - Standard (500ms)!");
    }
  
  unsigned long startTime = millis();
  while (millis() - startTime < 1000) {
  }

  for (int i = 8; i < 12; i++) {
    digitalWrite(i, LOW);
  }
  
  unsigned long startTime2 = millis();
  while (millis() - startTime2 < 3000) {
  }

  Serial.println("creating game task");

  xTaskCreate(gameTask, "GameTask", 256, NULL, 1, &gameTaskHandle);

  Serial.println("game task created");
}