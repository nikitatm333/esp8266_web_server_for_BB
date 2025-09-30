#pragma once
#include <Arduino.h>

void handleSerial();
bool looksNumeric(const String &tok);
int splitTokens(const String &line, String tokens[], int maxTokens);
