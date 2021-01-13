#include "sensors.h"
#include "server.h"
#include "control.h"

template<unsigned interval, typename Fn>
void timer(Fn && f) {
  unsigned time = millis();
  static unsigned lastTime = 0;

  if (time - lastTime > interval) {
    lastTime = time;
    f();
  }
}

void setup() {
  USE_SERIAL.begin(9600);
   Serial.println("start");
  setupServer();
  
    Serial.println("afterServer");

  setupControl();
  Serial.println("after control");

  setupSensors();
  
    Serial.println("Sensors");

  

  USE_SERIAL.println("Arduino initialized!");
}

void loop() {
  timer<100>([] {
    loopServer(setLight, setPump, setHumidity, setTemperature, setWorldTime);
  });
  
  timer<5000>([] {
    loopControl();
    loopSensors();
  });
}
