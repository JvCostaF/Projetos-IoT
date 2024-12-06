#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Stepper_28BYJ_48.h>

#define PR1_PIN A0
#define BOARD_RESOLUTION 1024
#define DHTPIN D5
#define DHTTYPE DHT22
#define STEPS 100

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "IC-LCI";
const char* password = "aluno.dcc!";
const char* mqttServer = "10.4.44.17";
const int mqttPort = 1883;
int doBlink = 0;
int currStep = 0;
int stepsToReceive = 0;
int sendDHTCounter = 0;

WiFiClient espClient;
PubSubClient client(espClient);
Stepper_28BYJ_48 stepper(D1, D2, D3, D4);

void setup() {
  Serial.begin(9600);
  // conectando ao wifi
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
  // dht
  dht.begin();
  
  // mqtt
  client.setClient(espClient);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(receiveSteps);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    // Reconexão...
    reconnect();
  }
  client.loop();
  sendDHTCounter++;
  int light_val = readPhotoresistor(PR1_PIN);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  // Serial.println(light_val);
  // Serial.println(temperature);
  // Serial.println(humidity);
  if (sendDHTCounter >= 3) {
    sendPayload(light_val, temperature, humidity);
  }
  //Serial.println(success);
  if (temperature > 30) {
    doBlink = 1;
  } else {
    doBlink = 0;
  }
  blink(doBlink);

  updateMotor();
  delay(300);
}

void sendPayload(int light_val, float temperature, float humidity) {
  String payload = buildPayload(light_val, temperature, humidity);
  Serial.println(payload);
  boolean success = client.publish("ufrj/projeto-iot-2024/grupo-d/dht-light", payload.c_str());
  sendDHTCounter = 0;
}

void updateMotor() {

  if (stepsToReceive < 0) {
    currStep += stepsToReceive;
    stepsToReceive = 0;
  } else if (stepsToReceive > 50) {
    stepper.step(50);
    stepsToReceive -= 50;
    currStep += 50;
  } else {
    stepper.step(stepsToReceive);
    currStep += stepsToReceive;
    stepsToReceive = 0;
  }

  Serial.println(currStep);
  Serial.println(stepsToReceive);

}

String buildPayload(int light, float temperature, float humidity) {

  String payload = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) +
                      ", \"light\": " + String(light) + "}";
  return payload;

}

void receiveSteps(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Message arrived on topic: ");
  // Serial.print(topic);
  // Serial.print(". Message: ");
  
  // Convert the payload to a string and print it
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  // Serial.println(message);  // Print the received message

  int stateReceived = message.toInt();
  int differenceInState = stateReceived - (currStep + stepsToReceive);
  stepsToReceive += differenceInState;

  // Serial.println(message);
  
  // You can process the message here (e.g., controlling devices)
}

void blink(int doBlink) {
  if (doBlink) {
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED on (HIGH is the voltage level)
    delay(500);                      // wait for a second
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED off by making the voltage LOW
  }
}

int readPhotoresistor(int pin) {
  int value = analogRead(pin);
  int light_code = 0;
  Serial.println(value);
  if (value < BOARD_RESOLUTION / 3) {
    light_code = 0;
  } else  if (value < (BOARD_RESOLUTION / 3) * 2) {
    light_code = 1;
  } else {
    light_code = 2;
  }
  return light_code;
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Tentando se conectar ao broker MQTT...");
        if (client.connect("projetoIOT")) {
            Serial.println("Conectado ao MQTT!");
            // Inscrever em um tópico, se necessário
            client.subscribe("ufrj/projeto-iot-2024/grupo-d/motor");
        } else {
            Serial.print("Falha ao conectar, estado: ");
            Serial.println(client.state());
            delay(2000); // Aguardar 2 segundos antes de tentar novamente
        }
    }
}