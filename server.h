#pragma once
#include <string.h>
#include <SoftwareSerial.h>

SoftwareSerial arduinoMega(10, 11);
//#define arduinoMega Serial2
char buffer[100];
String command;  //Переменная для определения команды

#define USE_SERIAL Serial

void sendMessage(String const& message) {
  arduinoMega.println(message);
}

void setupServer() {
  Serial.println("tut");
  arduinoMega.begin(4800);
  Serial.println("out");
}

template<typename FnLight, typename FnPump, typename FnHum, typename FnTemp, typename FnTime>
void loopServer(FnLight && lig, FnPump && pum, FnHum && hum, FnTemp && tem, FnTime && time) {
  if (!arduinoMega.available()) {
    return;
  }

  command = arduinoMega.readStringUntil('\n');
  USE_SERIAL.println(command);
  char buf[256];
  command.toCharArray(buf, command.length());
  char* pos = strtok(buf, " "); //Расспознавание команды
  String accepted_cmd;

  auto get_arg = [&]() {
    pos = strtok(NULL, " ");
    if (pos) {
      USE_SERIAL.print(atoi(pos));
      USE_SERIAL.print(" ");
      return atoi(pos);
    } else {
      return -1;
    }
  };

  USE_SERIAL.print("Accepted command: ");
  if (pos) {
    accepted_cmd = pos;
    USE_SERIAL.print(accepted_cmd);
  }

  int arg1 = -1;
  int arg2 = -1;
  int arg3 = -1;
  int arg4 = -1;
  int arg5 = -1;
  int arg6 = -1;

  USE_SERIAL.print(" ");

  auto readCommand = [&] {
    arg1 = get_arg();
    arg2 = get_arg();
    arg3 = get_arg();
    arg4 = get_arg();
    arg5 = get_arg();
    arg6 = get_arg();
  };

  USE_SERIAL.println();

  if (accepted_cmd == "set_light_interval") {  //Свет
    USE_SERIAL.println("command lig");
    readCommand();
    lig(arg1, arg2);
    sendMessage("set_light_interval accepted");
  } else if (accepted_cmd == "set_pump_interval") { //Насос
    USE_SERIAL.println("command pum");
    readCommand();
    pum(arg1, arg2);
    sendMessage("set_pump_interval accepted");
  } else if (accepted_cmd == "set_humidity") { //Влажность
    USE_SERIAL.println("command hum");
    readCommand();
    hum(arg1);
    sendMessage("set_humidity accepted");
  } else if (accepted_cmd == "set_temperature") { //Температура
    USE_SERIAL.println("command tem");
    readCommand();
    tem(arg1);
    sendMessage("set_temperature accepted");
  } else if (accepted_cmd == "set_time") {
    USE_SERIAL.println("set_time accepted");
    readCommand();
    if (arg4 != -1 && arg5 != -1) {
      time(arg4 * 60 + arg5);
    } else {
      USE_SERIAL.println("fail to set time");
      sendMessage("fail to set time");
    }
  } else {
    USE_SERIAL.println("undefined command");
    if (accepted_cmd.length() > 3) {
      sendMessage("none");
    }
  }
}
