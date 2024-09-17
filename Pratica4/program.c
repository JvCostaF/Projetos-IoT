#include <Arduino.h>
#include <TM1637Display.h>


const int trigPin = D5;
const int echoPin = D6;


const int clkPin = D2;
const int dioPin = D3;


float duracao, distancia;


TM1637Display display(clkPin, dioPin);


void setup() {
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);


 display.setBrightness(0x0f);
 display.clear();


 Serial.begin(115200);
 Serial.println("Sensor Ultrasonico HC-SR04");
}
void loop() {


 digitalWrite(trigPin, LOW);
 delayMicroseconds(2);
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPin, LOW);


 duracao = pulseIn(echoPin, HIGH);


 distancia = duracao * 0.034 / 2;


 int displayValue = int(distancia);


 display.showNumberDec(displayValue, false);


 delay(1000);


}

