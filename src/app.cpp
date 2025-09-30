#include "app.h"

ESP8266WebServer server(80);

String logLines[LOG_LINES];
int logHead = 0;
int logCount = 0;

float currentSetpoint = 25.0;
String lastCmd = "-";
String centralTemp = "-";

void pushLogLine(const String &ln) {
  logLines[logHead] = ln;
  logHead = (logHead + 1) % LOG_LINES;
  if (logCount < LOG_LINES) logCount++;
}

String buildLogText() {
  String out;
  int tail = (logHead - logCount + LOG_LINES) % LOG_LINES;
  for (int i = 0; i < logCount; i++) {
    out += logLines[(tail + i) % LOG_LINES];
    out += "\n";
  }
  return out;
}
