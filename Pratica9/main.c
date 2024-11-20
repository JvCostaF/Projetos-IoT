#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "IC-LCI";
const char* password = "aluno.dcc!";
const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;

Adafruit_BMP280 bmp;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(500);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando ao Wi-Fi...");
  }
  int status = WiFi.status();
  if (status == WL_CONNECTED) {
    Serial.println("Sucesso!\n Meu endereço IP é: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Falha ao conectar no WiFi");
  }

  client.setClient(espClient);
  client.setServer(mqttServer, mqttPort);
  bmp.begin(0x76); // Endereço I2C padrão do BME280
}

void loop() {
  if (!client.connected()) {
    // Reconexão...
    reconnect();
  }
  client.loop();

  float temp = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0F;
  Serial.println(F("Dados do sensor BMP280:"));
  Serial.print(F("Temperatura (BMP280): "));
  Serial.print(temp);
  Serial.println(F(" °C"));
    
  Serial.print(F("Pressão: "));
  Serial.print(pressure);
  Serial.println(F(" hPa"));
  
  String payload = "{\"temperature\": " + String(temp) + ", \"pressure\": " + String(pressure) + "}";
  boolean success = client.publish("sensor/bme280/projeto-iot-2024", payload.c_str());
  Serial.println(success);
  delay(1000);
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Tentando se conectar ao broker MQTT...");
        if (client.connect("projetoIOT")) {
            Serial.println("Conectado ao MQTT!");
            // Inscrever em um tópico, se necessário
            client.subscribe("sensor/bme280/projeto-iot-2024");
        } else {
            Serial.print("Falha ao conectar, estado: ");
            Serial.println(client.state());
            delay(2000); // Aguardar 2 segundos antes de tentar novamente
        }
    }
}

