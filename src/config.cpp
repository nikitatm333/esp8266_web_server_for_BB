#include "config.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

Config g_config;

static const char* CONFIG_FILE = "/config.json";

bool loadConfig(Config &cfg) {
  if (!LittleFS.begin()) {
    // попытка форматировать и смонтировать (редко нужно)
    // LittleFS.format(); // осторожно — стереть FS если нужно
    if (!LittleFS.begin()) return false;
  }

  if (!LittleFS.exists(CONFIG_FILE)) {
    // дефолты
    cfg.ssid = "ACHT_ESP";
    cfg.pass = "";
    cfg.setpoint = 25.0;      
    cfg.lastCmd = "";
    return true;
  }

  File f = LittleFS.open(CONFIG_FILE, "r");
  if (!f) return false;

  DynamicJsonDocument doc(512);
  auto err = deserializeJson(doc, f);
  f.close();
  if (err) return false;

  cfg.ssid = String((const char*)(doc["ssid"] | "ACHT_ESP"));
  cfg.pass = String((const char*)(doc["pass"] | ""));
  cfg.setpoint = (float)(doc["setpoint"] | 25.0);
  cfg.lastCmd = String((const char*)(doc["lastCmd"] | ""));
  return true;
}

bool saveConfig(const Config &cfg) {
  if (!LittleFS.begin()) {
    if (!LittleFS.begin()) return false;
  }
  File f = LittleFS.open(CONFIG_FILE, "w");
  if (!f) return false;

  DynamicJsonDocument doc(512);
  doc["ssid"] = cfg.ssid;
  doc["pass"] = cfg.pass;
  doc["setpoint"] = cfg.setpoint;
  doc["lastCmd"] = cfg.lastCmd;

  if (serializeJson(doc, f) == 0) {
    f.close();
    return false;
  }
  f.close();
  return true;
}
