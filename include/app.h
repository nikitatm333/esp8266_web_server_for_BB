#pragma once
#include <ESP8266WebServer.h>
#include <Arduino.h>

// web server
extern ESP8266WebServer server;

// logging buffer
const int LOG_LINES = 120;
extern String logLines[];
extern int logHead;
extern int logCount;

// application state (видим на вебе)
extern float currentSetpoint;
extern String lastCmd;
extern String centralTemp;

void pushLogLine(const String &ln);
String buildLogText();

extern String apSsid;
extern String apPass;

// helpers для загрузки/сохранения (реализованы в settings.cpp)
void loadApSettings();
bool saveApSettings(const String &ssid, const String &pass);