// src/web_routes.cpp
#include "web_routes.h"
#include "app.h"
#include "index.h"
#include "log_page.h"
#include "graph_page.h"
#include "graph.h"
#include "globals.h"
#include <pgmspace.h>
#include "config.h"
#include <ESP8266WiFi.h> // чтобы IPAddress был виден

#include <LittleFS.h>

// --- обработчики страниц ---
void handleRoot() {
  server.send_P(200, "text/html; charset=utf-8", INDEX_PAGE);
}

void handleLogPage() {
  server.send_P(200, "text/html; charset=utf-8", LOG_PAGE);
}

void handleUart() {
  String txt = buildLogText();
  server.send(200, "text/plain; charset=utf-8", txt);
}

void handleUartClear() {
  for (int i = 0; i < LOG_LINES; i++) logLines[i] = "";
  logHead = 0; logCount = 0;
  server.send(200, "text/plain", "OK");
}

void handleGetSetpoint() {
  String s = "{";
  s += "\"setpoint\":" + String(currentSetpoint, 1) + ",";
  String lc = lastCmd;
  lc.replace("\\", "\\\\");
  lc.replace("\"", "\\\"");
  s += "\"lastCmd\":\"" + lc + "\"";
  s += "}";
  server.send(200, "application/json", s);
}

void handleGetTemp() {
  String json = "{";
  String t = centralTemp;
  t.replace("\\", "\\\\");
  t.replace("\"", "\\\"");
  json += "\"temp\":\"" + t + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

// --- Установка температуры (обновление RAM + пометка для отложенной записи) ---
void handleSetTemp() {
  String tempStr;
  if (server.method() == HTTP_POST) {
    if (server.hasArg("temp")) tempStr = server.arg("temp");
    else tempStr = server.arg(0);
  } else {
    if (server.hasArg("temp")) tempStr = server.arg("temp");
  }

  tempStr.trim();
  if (tempStr.length() == 0) {
    server.send(400, "text/plain", "missing temp");
    return;
  }

  float v = tempStr.toFloat();
  if (v < -50.0) v = -50.0;
  if (v > 150.0) v = 150.0;

  char buf[32];
  snprintf(buf, sizeof(buf), "T=%.1f\r\n", v);
  pushLogLine(String("[SENT] ") + String(buf));
  Serial.write((const uint8_t*)buf, strlen(buf));
  Serial.flush();

  // сразу применяем в RAM
  currentSetpoint = v;
  lastCmd = String("ESP: Sent T=") + String(v, 1);

  // обновляем RAM-конфиг (чтобы при следующей записи сохранить актуальное состояние)
  g_config.setpoint = currentSetpoint;
  g_config.lastCmd = lastCmd;

  // НЕ сохраняем немедленно — ставим флаг "грязного" конфига (debounce)
  configDirty = true;
  configDirtySince = millis();

  server.send(200, "text/plain", "OK");
}

// --- получение/установка Wi-Fi конфигов (сохранение немедленно) ---
void handleGetConfig() {
  String s = "{";
  s += "\"ssid\":\"" + String(g_config.ssid) + "\",";
  s += "\"pass\":\"" + String(g_config.pass) + "\"";
  s += "}";
  server.send(200, "application/json", s);
}

void handleGetMac() {
  String mac = WiFi.softAPmacAddress(); // MAC точки доступа (AP)
  String s = "{";
  s += "\"mac\":\"" + mac + "\"";
  s += "}";
  server.send(200, "application/json", s);
}

void handleSetConfig() {
  String ssid, pass;
  if (server.method() == HTTP_POST) {
    if (server.hasArg("ssid")) ssid = server.arg("ssid");
    else ssid = server.arg(0);
    if (server.hasArg("pass")) pass = server.arg("pass");
  } else {
    if (server.hasArg("ssid")) ssid = server.arg("ssid");
    if (server.hasArg("pass")) pass = server.arg("pass");
  }

  if (ssid.length() == 0) {
    server.send(400, "text/plain", "missing ssid");
    return;
  }

  // обновить конфиг и сохранить сразу 
  g_config.ssid = ssid;
  g_config.pass = pass;
  if (!saveConfig(g_config)) {
    server.send(500, "text/plain", "save failed");
    return;
  }

  // отправляем ответ клиенту сразу, прежде чем менять SoftAP.
  // Иначе клиент потеряет соединение и попадёт в .catch().
  server.send(200, "text/plain", "OK");

  // теперь безопасно поменять SoftAP — клиент уже получил ответ
  WiFi.softAPdisconnect(true);
  WiFi.softAP(g_config.ssid.c_str(), g_config.pass.c_str());
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP changed to "); Serial.print(g_config.ssid); Serial.print(" -> "); Serial.println(ip);
}

void handleGraphPage() {
  server.send_P(200, "text/html; charset=utf-8", GRAPH_PAGE);
}

void handleGraphSvg() {
  String svg = drawGraphSvg();
  server.send(200, "image/svg+xml; charset=utf-8", svg);
}


void handleGraphClear() {
  for (int i = 0; i < L; ++i) TStorage[i] = NAN;
  TShead = 0;
  TStail = 0;
  server.send(200, "text/plain", "OK");
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void setupRoutes() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/log", HTTP_GET, handleLogPage);
  server.on("/uart", HTTP_GET, handleUart);
  server.on("/uart_clear", HTTP_POST, handleUartClear);
  server.on("/get_setpoint", HTTP_GET, handleGetSetpoint);
  server.on("/get_temp", HTTP_GET, handleGetTemp);
  server.on("/set_temp", HTTP_POST, handleSetTemp);
  server.on("/set_temp", HTTP_GET, handleSetTemp);
  server.on("/get_config", HTTP_GET, handleGetConfig);
  server.on("/set_config", HTTP_POST, handleSetConfig);
  server.on("/get_mac", HTTP_GET, handleGetMac);
  server.on("/graph", HTTP_GET, handleGraphPage);
  server.on("/graph_svg", HTTP_GET, handleGraphSvg);
  server.on("/graph_clear", HTTP_POST, handleGraphClear);
  server.onNotFound(handleNotFound);

  server.on("/dumpconfig", HTTP_GET, []() {
  String body = "<html><body><pre>";
  if (!LittleFS.begin()) {
    body += "LittleFS not mounted";
  } else if (!LittleFS.exists("/config.json")) {
    body += "&lt;no config file&gt;";
  } else {
    File f = LittleFS.open("/config.json", "r");
    if (!f) body += "open failed";
    else {
      body += f.readString();
      f.close();
    }
  }
  body += "</pre></body></html>";
  server.send(200, "text/html", body);
});
}
