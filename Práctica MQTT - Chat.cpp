#include <WiFi.h>
#include <PubSubClient.h>

// Configuración WiFi y MQTT
const char* ssid = "";               // Nombre de la red WiFi
const char* password = "";           // Contraseña del WiFi
const char* name = ":";            // Nombre/emisor del mensaje

const char* mqtt_server = "";        // Dirección del broker MQTT
const int mqtt_port = 1883;          // Puerto estándar para MQTT
const char* client_id = "";          // ID único del cliente MQTT

const char* subscribe_topic = "";    // Tópico al que se suscribe
const char* publish_topic = "";      // Tópico al que publica

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);

  // Conexión al WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando al WiFi...");
    delay(500);
  }
  Serial.println("Conectado al WiFi");

  // Conexión al broker MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Conectando al broker MQTT...");
    client.connect(client_id);
    delay(1000);
  }
  Serial.println("Conectado al broker MQTT");

  client.subscribe(subscribe_topic);
}

// Función que se ejecuta al recibir un mensaje en el tópico suscripto
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println("Mensaje recibido: " + msg);
}

void loop() {
  client.loop();

  // Si llega un mensaje por el monitor serial, lo publica por MQTT
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg = name + msg;
    client.publish(publish_topic, msg.c_str());
    Serial.println("Mensaje enviado: " + msg);
  }
}