#include <Arduino.h>

// Declaración de recursos compartidos
SemaphoreHandle_t pista[3];  // Semáforos (mutex) para cada pista de aterrizaje

QueueHandle_t solicitudDeAterrizaje;  // Cola para que los aviones pidan permiso
QueueHandle_t permisoDeAterrizaje;   // Cola donde la torre responde con el número de pista

// Función que representa el comportamiento de un avión (CORE 0)
void avioncito(void *parameter) {
  int id = (int)parameter;  // ID del avión
  int pistaAsignada;

  while (true) {
    delay(random(3000, 5000));  // Simula el tiempo de aproximación

    Serial.printf("Avion %d solicita aterrizar\n", id);
    delay(1000);  // Pausa para leer el mensaje

    xQueueSend(solicitudDeAterrizaje, &id, portMAX_DELAY);

    // Espera la autorización y número de pista de la torre
    xQueueReceive(permisoDeAterrizaje, &pistaAsignada, portMAX_DELAY);

    // Intenta tomar la pista asignada
    if (xSemaphoreTake(pista[pistaAsignada], portMAX_DELAY) == pdTRUE) {
      Serial.printf("Avion %d aterriza en la pista %d\n", id, pistaAsignada);
      delay(1500);  // Pausa más larga para simular y leer mejor

      delay(random(3000, 5000));  // Simula el tiempo de aterrizaje

      Serial.printf("Avion %d libera la pista %d\n\n", id, pistaAsignada);
      delay(1000);  // Otra pausa antes de volver a empezar
      xSemaphoreGive(pista[pistaAsignada]);
    }
  }
}

// Función de la torre de control (CORE 1)
void torre(void *parameter) {
  int idAvion;

  while (true) {
    if (xQueueReceive(solicitudDeAterrizaje, &idAvion, portMAX_DELAY)) {
      delay(1000);  // Simula el tiempo de decisión de la torre

      Serial.printf("Torre: autoriza aterrizaje para avion %d\n", idAvion);
      delay(1000);  // Tiempo para que se lea el mensaje

      // Busca una pista libre
      for (int i = 0; i < 3; i++) {
        if (xSemaphoreTake(pista[i], 1) == pdTRUE) {
          xSemaphoreGive(pista[i]);  // Solo chequea si está libre
          xQueueSend(permisoDeAterrizaje, &i, portMAX_DELAY);
          break;
        }
      }

      Serial.println();  // Espacio visual
    }
  }
}

void setup() {
  Serial.begin(9600);

  // Inicializamos mutex para cada pista
  for (int i = 0; i < 3; i++) {
    pista[i] = xSemaphoreCreateMutex();
  }

  // Inicializamos las colas
  solicitudDeAterrizaje = xQueueCreate(5, sizeof(int));
  permisoDeAterrizaje = xQueueCreate(5, sizeof(int));

  // Creamos tareas de los aviones (Core 0)
  for (int i = 0; i < 5; i++) {
    xTaskCreatePinnedToCore(
      avioncito,
      "avion",
      4096,
      (void *)i,
      1,
      NULL,
      0
    );
  }

  // Creamos tarea de la torre (Core 1)
  xTaskCreatePinnedToCore(
    torre,
    "torre",
    4096,
    NULL,
    1,
    NULL,
    1
  );
}

void loop() {
  // Nada que hacer en loop
}
