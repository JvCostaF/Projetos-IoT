#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>

// Definindo o pino do DHT22
#define DHTPIN D5     
#define DHTTYPE DHT22   // Tipo do sensor

// Inicializando o sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Inicializando o sensor BMP280
Adafruit_BMP280 bmp; // I2C

void setup() {
  Serial.begin(9600);
  
  // Iniciando o BMP280
  if (!bmp.begin(0x76)) {  // Endereço padrão I2C 0x76 para BMP280
    Serial.println(F("Falha ao encontrar o sensor BMP280!"));
    while (1);
  }

  // Configuração do BMP280
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Modo normal
                  Adafruit_BMP280::SAMPLING_X2,     // Sobreamostragem de temperatura
                  Adafruit_BMP280::SAMPLING_X16,    // Sobreamostragem de pressão
                  Adafruit_BMP280::FILTER_X16,      // Filtro
                  Adafruit_BMP280::STANDBY_MS_500); // Tempo de espera

  // Iniciando o sensor DHT
  dht.begin();
}

void loop() {
  // Leitura do BMP280
  float temperatura_bmp = bmp.readTemperature();
  float pressao = bmp.readPressure() / 100.0F;  // Convertendo para hPa
  float altitude = bmp.readAltitude(1013.25);   // Definindo 1013.25 hPa como pressão ao nível do mar

  // Leitura do DHT22
  float temperatura_dht = dht.readTemperature();
  float humidade = dht.readHumidity();

  // Checando se as leituras são válidas
  if (isnan(temperatura_dht) || isnan(humidade)) {
    Serial.println(F("Falha ao ler do DHT22!"));
  } else {
    // Exibir leituras do BMP280
    Serial.println(F("Dados do sensor BMP280:"));
    Serial.print(F("Temperatura (BMP280): "));
    Serial.print(temperatura_bmp);
    Serial.println(F(" °C"));
    
    Serial.print(F("Pressão: "));
    Serial.print(pressao);
    Serial.println(F(" hPa"));
    
    Serial.print(F("Altitude: "));
    Serial.print(altitude);
    Serial.println(F(" m"));

    // Exibir leituras do DHT22
    Serial.println(F("Dados do sensor DHT22:"));
    Serial.print(F("Temperatura (DHT22): "));
    Serial.print(temperatura_dht);
    Serial.println(F(" °C"));
    
    Serial.print(F("Humidade: "));
    Serial.print(humidade);
    Serial.println(F(" %"));
  }

  // Aguarde 2 segundos antes da próxima leitura
  delay(2000);
}
