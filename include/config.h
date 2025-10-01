#pragma once
#include <Arduino.h>

struct Config {
  String ssid;
  String pass;
  float setpoint;
  String lastCmd;
};

extern Config g_config;
extern bool configDirty;
extern unsigned long configDirtySince;
extern const unsigned long CONFIG_SAVE_DEBOUNCE_MS;

bool loadConfig(Config &cfg);
bool saveConfig(const Config &cfg);