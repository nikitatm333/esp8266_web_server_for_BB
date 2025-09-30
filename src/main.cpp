#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>

#include "app.h"
#include "web_routes.h"
#include "serial_parser.h"
#include "config.h"  


void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println();
  Serial.println("ESP-01 UART viewer starting...");

  // монтируем и читаем конфиг
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
  }
  if (!loadConfig(g_config)) {
    Serial.println("Load config failed, using defaults");
    // g_config уже заполнится дефолтами в loadConfig
  }

  // запустить точку доступа с сохранённым SSID/Password
  WiFi.mode(WIFI_AP);
  WiFi.softAP(g_config.ssid.c_str(), g_config.pass.c_str());
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP running. Connect to "); Serial.print(g_config.ssid); Serial.print(" -> http://"); Serial.println(ip);

  // restore last setpoint into runtime variable
  currentSetpoint = g_config.setpoint;
  lastCmd = g_config.lastCmd;

  // clear log buffer
  for (int i = 0; i < LOG_LINES; i++) logLines[i] = "";
  logHead = 0; logCount = 0;

  setupRoutes();
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  handleSerial();

  // проверяем — пора ли сохранить конфиг на LittleFS?
  if (configDirty) {
    unsigned long now = millis();
    if ((unsigned long)(now - configDirtySince) >= CONFIG_SAVE_DEBOUNCE_MS) {
      if (saveConfig(g_config)) {
        Serial.println("Config saved (debounced).");
      } else {
        Serial.println("Config save FAILED!");
      }
      configDirty = false;
    }
  }
}

