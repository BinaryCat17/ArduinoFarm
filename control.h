# pragma once
#include "Time.h"
#include "DS1307RTC.h"
#include "server.h"
#include "Wire.h"

unsigned const lightPin = 53;
unsigned const pumpPin = 52;

unsigned beginLightTime = 7.5 * 60;
unsigned endLightTime = 22.5 * 60;

unsigned wastedPumpTime = 0;
bool pumpState = false;
unsigned previousTime = 0;

unsigned workPumpTime = 30;
unsigned relaxPumpTime = 30;

void setLight(int v1, int v2) {
  beginLightTime = v1;
  endLightTime = v2;
  USE_SERIAL.print("new light interval: ");
  USE_SERIAL.print(beginLightTime);
  USE_SERIAL.print(" ");
  USE_SERIAL.println(endLightTime);
}

void setPump(int v1, int v2) {
  workPumpTime = v1;
  relaxPumpTime = v2;
  wastedPumpTime = 0;
  pumpState = false;
  USE_SERIAL.print("new pump interval: ");
  USE_SERIAL.print(workPumpTime);
  USE_SERIAL.print(" ");
  USE_SERIAL.println(relaxPumpTime);
}

void setWorldTime(int minutes) {
  USE_SERIAL.print("New time is: ");
  USE_SERIAL.print(minutes / 60);
  USE_SERIAL.print(" ");
  USE_SERIAL.println(minutes % 60);
  setTime(minutes / 60, minutes % 60, 0, 1, 1, 2020);
  RTC.set(now());
}

void setupControl() {
  setSyncProvider(RTC.get);
  setWorldTime(0);

  pinMode(lightPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(lightPin, HIGH);
  digitalWrite(pumpPin, HIGH);

  if (timeStatus() != timeSet) {
    USE_SERIAL.println("Time error");
  } else {
    USE_SERIAL.println("Time set");
  }
}

void loopControl() {
  USE_SERIAL.print("Time is: ");
  USE_SERIAL.print(hour());
  USE_SERIAL.print(" ");
  USE_SERIAL.print(minute());
  USE_SERIAL.print(" ");
  USE_SERIAL.print(second());
  USE_SERIAL.println();


  int minutes = hour() * 60 + minute();
  wastedPumpTime += minutes - previousTime;
  if (pumpState && wastedPumpTime >= workPumpTime) {
    pumpState = false;
    wastedPumpTime = 0;
  }

  if (!pumpState && wastedPumpTime >= relaxPumpTime) {
    pumpState = true;
    wastedPumpTime = 0;
  }

  digitalWrite(pumpPin, pumpState);

  if (pumpState) {
    sendMessage("updated_pump on");
    USE_SERIAL.println("updated_pump on");
  } else {
    sendMessage("updated_pump off");
    USE_SERIAL.println("updated_pump off");
  }

  if (minutes >= beginLightTime && minutes <= endLightTime) {
    digitalWrite(lightPin, HIGH);
    sendMessage("updated_light on");
    USE_SERIAL.println("updated_light on");
  } else {
    digitalWrite(lightPin, LOW);
    sendMessage("updated_light off");
    USE_SERIAL.println("updated_light off");
  }

  previousTime = minutes;

}
