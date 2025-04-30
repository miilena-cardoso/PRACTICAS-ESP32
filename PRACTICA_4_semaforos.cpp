#include <Arduino.h>

#define LED_ROJO     23
#define LED_AMARILLO 22
#define LED_VERDE    21

#define LED_ROJITO    19
#define LED_AMARILLITO 18
#define LED_VERDECITO    5

SemaphoreHandle_t semaforito;

void semaforito1(void *parameter) 
{
  while (true) 
  {
    if (xSemaphoreTake(semaforito, portMAX_DELAY) == pdTRUE) 
    {

      digitalWrite(LED_ROJO, LOW);
      digitalWrite(LED_AMARILLO, HIGH);
      vTaskDelay(pdMS_TO_TICKS(1000));

      digitalWrite(LED_AMARILLO, LOW);
      digitalWrite(LED_VERDE, HIGH);
      vTaskDelay(pdMS_TO_TICKS(4000));

      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_AMARILLO, HIGH);
      vTaskDelay(pdMS_TO_TICKS(1000));

      digitalWrite(LED_AMARILLO, LOW);
      digitalWrite(LED_ROJO, HIGH);

      xSemaphoreGive(semaforito);
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }
}


void semaforito2(void *parameter) 
{
  while (true) 
  {
    if (xSemaphoreTake(semaforito, portMAX_DELAY) == pdTRUE) 
    {
      digitalWrite(LED_ROJITO, HIGH);
      digitalWrite(LED_AMARILLITO, LOW);
      digitalWrite(LED_VERDECITO, LOW);
      vTaskDelay(pdMS_TO_TICKS(3000));

      digitalWrite(LED_ROJITO, LOW);
      digitalWrite(LED_AMARILLITO, HIGH);
      vTaskDelay(pdMS_TO_TICKS(1000));

      digitalWrite(LED_AMARILLITO, LOW);
      digitalWrite(LED_VERDECITO, HIGH);
      vTaskDelay(pdMS_TO_TICKS(3000));

      digitalWrite(LED_VERDECITO, LOW);
      digitalWrite(LED_ROJITO, HIGH);

      xSemaphoreGive(semaforito);
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }
}

void setup() 
{
  Serial.begin(115200);
  delay(100);

  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);

  pinMode(LED_ROJITO, OUTPUT);
  pinMode(LED_AMARILLITO, OUTPUT);
  pinMode(LED_VERDECITO, OUTPUT);

  digitalWrite(LED_ROJITO, HIGH);
  digitalWrite(LED_ROJO, HIGH);

  semaforito = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(
    semaforito1,
    "Semaforo1",
    4096, 
    NULL, 
    1, 
    NULL, 
    0);

  xTaskCreatePinnedToCore(
  semaforito2, 
  "Semaforo2", 
  4096, 
  NULL, 
  1, 
  NULL, 
  1);
}

void loop() {
  // nada por aquí
}
