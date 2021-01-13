#pragma once
#include "DHT.h"
#include "server.h"

unsigned const coolerPin = 8;
unsigned const steamPin = 9;
unsigned const dhtPort = 5;

unsigned maxTmp = 20;
unsigned minHumidity = 60;

void setTemperature(int v) {
  USE_SERIAL.print("new temperature: ");
  USE_SERIAL.println(v);
  maxTmp = v;
}

void setHumidity(int v) {
  USE_SERIAL.print("new humidity: ");
  USE_SERIAL.println(v);
  minHumidity = v;
}

DHT dht(dhtPort, DHT11);

void setupSensors() {
  dht.begin();        //Подключение DHT-датчика
  pinMode(steamPin, OUTPUT);
  pinMode(coolerPin, OUTPUT);
}

void loopSensors() {
  static float lastHumidity = dht.readHumidity();
  static float lastTemperature = dht.readTemperature();

  float vlg = dht.readHumidity();
  sendMessage("updated_humidity " + String(vlg));

  float tmp = dht.readTemperature();
  sendMessage("updated_temperature " + String(tmp));

  // выводим температуру (t) и влажность (h) на монитор порта
  USE_SERIAL.print("Влажность: ");
  USE_SERIAL.print(vlg);
  USE_SERIAL.println("%");
  USE_SERIAL.print("Температура: ");
  USE_SERIAL.print(tmp);
  USE_SERIAL.println("C");

  if (tmp > maxTmp) {
    digitalWrite(coolerPin, HIGH);
    sendMessage("updated_cooler on");
  } else {
    digitalWrite(coolerPin, LOW);
    sendMessage("updated_cooler off");
  }

  if (vlg < minHumidity) {
    digitalWrite(steamPin, HIGH);
    sendMessage("updated_steam on");
  } else {
    digitalWrite(steamPin, LOW);
    sendMessage("updated_steam off");
  }
}
