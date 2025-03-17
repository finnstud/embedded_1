#include "buttons.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>

extern QueueHandle_t buttonQueue;

const int buttonPins[4] = {4, 5, 6, 7};

void buttonTask(void *pvParameters) {
  int lastState[4] = {LOW, LOW, LOW, LOW};

  while (1) {
    for (int i = 0; i < 4; i++) {
      int currentState = digitalRead(buttonPins[i]);
      if (currentState == HIGH && lastState[i] == LOW) {
        int btn = i;
        xQueueSend(buttonQueue, &btn, portMAX_DELAY);
      }
      lastState[i] = currentState;
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void setupButtons() {
  buttonQueue = xQueueCreate(10, sizeof(int));

  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT);
  }

  xTaskCreate(buttonTask, "ButtonTask", 256, NULL, 1, NULL);
}