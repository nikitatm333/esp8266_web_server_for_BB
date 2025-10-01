#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>

#include "app.h"
#include "web_routes.h"
#include "serial_parser.h"
#include "config.h"
#include "globals.h"  


void setup() {
  Serial.begin(115200);
  delay(50);
  initGraphStorage();
  Serial.println();
  Serial.println("ESP-01 UART viewer starting...");

  // -------------------------
  // 1) Mount LittleFS
  // -------------------------
  if (!LittleFS.begin()) {
    Serial.println("[CFG] LittleFS mount failed, retrying...");
    // одна попытка повторно (иногда полезно)
    delay(50);
    if (!LittleFS.begin()) {
      Serial.println("[CFG] LittleFS mount FAILED");
    } else {
      Serial.println("[CFG] LittleFS mounted (after retry)");
    }
  } else {
    Serial.println("[CFG] LittleFS mounted OK");
  }

  // -------------------------
  // 2) Load config (with defaults fallback)
  // -------------------------
  if (!loadConfig(g_config)) {
    Serial.println("[CFG] Load config failed, using defaults");
    // loadConfig должен заполнить дефолты
  } else {
    Serial.printf("[CFG] Loaded config: ssid=%s setpoint=%.2f lastCmd=%s\n",
                  g_config.ssid.c_str(), g_config.setpoint, g_config.lastCmd.c_str());
  }

  // -------------------------
  // 3) Dump raw config file to Serial (read what's actually on FS)
  // -------------------------
  Serial.println(F("[CFG] --- config file content (read from FS) ---"));
  if (LittleFS.exists("/config.json")) {
    File f = LittleFS.open("/config.json", "r");
    if (!f) {
      Serial.println(F("[CFG] dump: open failed"));
    } else {
      String txt = f.readString();
      f.close();
      Serial.println(txt);
    }
  } else {
    Serial.println(F("[CFG] dump: <no config file>"));
  }
  Serial.println(F("[CFG] --- end config dump ---"));

  // -------------------------
  // 4) Start SoftAP with saved SSID/pass
  // -------------------------
  WiFi.mode(WIFI_AP);
  WiFi.softAP(g_config.ssid.c_str(), g_config.pass.c_str());
  IPAddress ip = WiFi.softAPIP();
  Serial.print("[CFG] AP running. Connect to ");
  Serial.print(g_config.ssid);
  Serial.print(" -> http://");
  Serial.println(ip);

  // -------------------------
  // 5) Restore runtime vars from loaded config
  // -------------------------
  currentSetpoint = g_config.setpoint;
  lastCmd = g_config.lastCmd;

  // логируем текущую runtime-уставку
  Serial.printf("[CFG] Runtime setpoint restored: %.2f (lastCmd: %s)\n",
                currentSetpoint, lastCmd.c_str());
  pushLogLine(String("[CFG] Runtime setpoint restored: ") + String(currentSetpoint, 1));

  // -------------------------
  // 6) Announce setpoint to STM32 on boot (sync)
  // -------------------------
  {
    Serial.printf("[CFG] Announcing saved setpoint to STM32: %.2f\n", currentSetpoint);
    const int BOOT_ANNOUNCE_ATTEMPTS = 6;
    // for (int i = 0; i < BOOT_ANNOUNCE_ATTEMPTS; ++i) {
    //   // отправляем в том же формате, который разбирает STM32
    //   Serial.print("T=");
    //   Serial.print(currentSetpoint, 1);
    //   Serial.print("\n");
    //   Serial.print("VOID_SETUP\n");
    //   // даём STM32 время принять / распарсить / загрузиться
    //   delay(120);
    // }
    pushLogLine(String("[CFG] Announced setpoint to STM32: ") + String(currentSetpoint, 1));
  }

  // -------------------------
  // 7) Clear log buffer (runtime)
  // -------------------------
  for (int i = 0; i < LOG_LINES; i++) logLines[i] = "";
  logHead = 0; logCount = 0;

  // -------------------------
  // 8) Setup web routes and start server
  // -------------------------
  setupRoutes();
  server.begin();
  Serial.println("[HTTP] HTTP server started");
}

void loop() {
  server.handleClient();
  handleSerial();

  // проверяем — пора ли сохранить конфиг на LittleFS
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

