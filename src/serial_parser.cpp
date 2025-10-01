// serial_parser.cpp
#include "serial_parser.h"
#include "app.h"
#include "globals.h"
#include "config.h"
#include <ESP8266WiFi.h>
#include <math.h>    // fabsf
#include <stdio.h>   // sscanf
#include <string.h>  // strlen


// Разделение строки на токены по пробельным символам
int splitTokens(const String &line, String tokens[], int maxTokens) {
  int tcount = 0;
  String cur = "";
  for (unsigned int i = 0; i < line.length(); ++i) {
    char c = line.charAt(i);
    if (c == ' ' || c == '\t') {
      if (cur.length() > 0) {
        if (tcount < maxTokens) tokens[tcount++] = cur;
        cur = "";
      }
    } else {
      cur += c;
    }
  }
  if (cur.length() > 0 && tcount < maxTokens) tokens[tcount++] = cur;
  return tcount;
}

// Проверка, выглядит ли токен как число (целая/дробная, с +/-)
bool looksNumeric(const String &tok) {
  bool hasDigit = false;
  bool hasOther = false;
  for (unsigned int i = 0; i < tok.length(); ++i) {
    char c = tok.charAt(i);
    if ((c >= '0' && c <= '9')) hasDigit = true;
    else if (c == '.' || c == '-' || c == '+') { /* допустимые символы */ }
    else hasOther = true;
  }
  return hasDigit && !hasOther;
}

// Надёжный парсер UART-строк от STM32.
// Обрабатывает:
//  - общие числовые строки (sensor1..4, central, PID)
//  - команды RESET (в отдельном токене)
//  - команды уставки T= (регистр не важен) — обновляет currentSetpoint и помечает g_config->dirty
void handleSerial() {
  static String uartLine = "";
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      String line = uartLine;
      line.trim();
      uartLine = "";
      if (line.length() > 0) {
        pushLogLine(String("[STM32] ") + line);

        // ------------------------------
        // 1) Проверка на команду RESET (как отдельный токен)
        // ------------------------------
        {
          const int MAXCHK = 16;
          String chk[MAXCHK];
          int cc = splitTokens(line, chk, MAXCHK);
          bool foundReset = false;
          for (int i = 0; i < cc; ++i) {
            String tok = chk[i];
            tok.toUpperCase();
            if (tok == "RESET") {
              foundReset = true;
              break;
            }
          }
          if (foundReset) {
            Serial.println("[CFG] RESET command received from STM32 - resetting SSID/password");
            pushLogLine("[CFG] RESET command received - resetting SSID/password");

            // обновляем конфиг
            g_config.ssid = String("BB_RESET");
            g_config.pass = String("");

            // обновляем lastCmd для отображения
            g_config.lastCmd = String("STM32: RESET");

            // сохраним конфиг (persist)
            if (saveConfig(g_config)) {
              pushLogLine("[CFG] Config saved: SSID=BB_RESET");
              Serial.println("[CFG] Config saved: SSID=BB_RESET");
            } else {
              pushLogLine("[CFG] Config save FAILED!");
              Serial.println("[CFG] Config save FAILED!");
            }

            // переключим SoftAP на новые параметры
            WiFi.softAPdisconnect(true);
            WiFi.softAP(g_config.ssid.c_str(), g_config.pass.c_str());
            IPAddress ip = WiFi.softAPIP();
            String ips = ip.toString();
            pushLogLine(String("[CFG] SoftAP changed to ") + g_config.ssid + " -> " + ips);
            Serial.print("[CFG] SoftAP changed to ");
            Serial.print(g_config.ssid);
            Serial.print(" -> ");
            Serial.println(ips);

            // Обновим runtime-переменные 
            lastCmd = g_config.lastCmd;
            currentSetpoint = g_config.setpoint;
          }
        }

        // ------------------------------
        // 2) Разбор чисел — датчики и PID
        // ------------------------------
        const int MAXTOK = 64;
        String toks[MAXTOK];
        int tcount = splitTokens(line, toks, MAXTOK);
        String numtoks[MAXTOK];
        int nnum = 0;
        for (int i = 0; i < tcount; ++i) {
          if (looksNumeric(toks[i])) {
            if (nnum < MAXTOK) numtoks[nnum++] = toks[i];
          }
        }

        // Обновляем centralTemp (строка) для обратной совместимости
        if (nnum >= 5) {
          centralTemp = numtoks[4];
        } else {
          if (tcount >= 5 && looksNumeric(toks[4])) {
            centralTemp = toks[4];
          }
        }

        // Вычисляем числовой centralVal. Если есть два подряд (индексы 4 и 5), усредним их.
        bool haveCentralNumeric = false;
        float centralVal = 0.0f;
        if (nnum >= 5) {
          centralVal = numtoks[4].toFloat();
          haveCentralNumeric = true;
          if (nnum >= 6) {
            float c2 = numtoks[5].toFloat();
            centralVal = (centralVal + c2) / 2.0f;
            haveCentralNumeric = true;
          }
        } else {
          // fallback: пробуем превратить centralTemp в число
          if (centralTemp.length() > 0 && looksNumeric(centralTemp)) {
            centralVal = centralTemp.toFloat();
            haveCentralNumeric = true;
          }
        }

        if (haveCentralNumeric) {
          // обновляем числовой центральный датчик для /sensors
          sensorCentral = centralVal;

          // записываем в кольцевой буфер для графика
          if (L > 0) {
            TStorage[TShead] = centralVal;
            TShead = (TShead + 1) % L;
            if (TShead == TStail) {
              // сдвигаем хвост если перезаписываем старые данные
              TStail = (TStail + 1) % L;
            }
          }
        }

        // Обновляем sensor1..sensor4
        if (nnum >= 4) {
          sensor1 = (nnum > 0) ? numtoks[0].toFloat() : sensor1;
          sensor2 = (nnum > 1) ? numtoks[1].toFloat() : sensor2;
          sensor3 = (nnum > 2) ? numtoks[2].toFloat() : sensor3;
          sensor4 = (nnum > 3) ? numtoks[3].toFloat() : sensor4;
        }

        // PID: определяем стартовый индекс PID в зависимости от количества чисел
        int pidStart = -1;
        if (nnum >= 10) pidStart = 6; // есть два central, PID идут с индекса 6
        else if (nnum >= 9) pidStart = 5; // один central, PID с 5
        // иначе pidStart остаётся -1

        if (pidStart != -1 && nnum >= pidStart + 4) {
          pid1 = numtoks[pidStart + 0].toFloat();
          pid2 = numtoks[pidStart + 1].toFloat();
          pid3 = numtoks[pidStart + 2].toFloat();
          pid4 = numtoks[pidStart + 3].toFloat();
        }

        // ------------------------------
        // 3) Обработка уставки "T=" (надёжно, нечувствительно к регистру)
        // ------------------------------
        {
          String up = line;
          up.toUpperCase();
          int idx = up.indexOf("T=");
          if (idx >= 0) {
            // найдём число после "T=" в оригинальной строке (сохраняем регистр/знаки)
            int origPos = idx; // позиция 'T' относительно строки up == позиции в line
            // безопасно получить C-string и сдвинуть на символы после '='
            const char *orig = line.c_str();
            const char *p = orig + origPos + 2; // после "T="

            // убрать начальные пробелы
            while (*p == ' ' || *p == '\t') p++;

            // Попробуем sscanf - это проще и надёжнее для float
            float vv = 0.0f;
            bool parsed = false;
            if (sscanf(p, "%f", &vv) == 1) {
              parsed = true;
            } else {
              // fallback: ручной парсинг токена (берём первые подряд символы, похожие на число)
              String sub = String(p);
              sub.trim();
              String valtok = "";
              for (unsigned int k = 0; k < sub.length(); ++k) {
                char cc = sub.charAt(k);
                if ((cc >= '0' && cc <= '9') || cc == '.' || cc == '-' || cc == '+') {
                  valtok += cc;
                } else break;
              }
              if (valtok.length() > 0) {
                vv = valtok.toFloat();
                parsed = true;
              }
            }

            if (parsed) {
              // обновляем runtime-переменные и помечаем конфиг "грязным" для отложенного сохранения
              lastCmd = String("STM32: ") + line;
              currentSetpoint = vv;

              const float EPS = 0.05f;
              if (fabsf(g_config.setpoint - currentSetpoint) > EPS || g_config.lastCmd != lastCmd) {
                g_config.setpoint = currentSetpoint;
                g_config.lastCmd = lastCmd;
                configDirty = true;
                configDirtySince = millis();
                pushLogLine(String("[CFG] Marked dirty from STM32: setpoint=") + String(currentSetpoint, 1));
                Serial.printf("[CFG] Marked dirty from STM32: setpoint=%.2f\n", currentSetpoint);
              }
            } else {
              // не удалось распарсить число после T=
              pushLogLine(String("[CFG] Failed to parse T= value from: ") + line);
              Serial.print("[CFG] Failed to parse T= value from: ");
              Serial.println(line);
            }
          } // if idx >= 0
        }

        // закончена обработка строки
      }
    } else {
      // накапливаем символы до перевода строки
      uartLine += c;
      if (uartLine.length() > 1024) {
        // обрежем до 1024 последних символов, чтобы не расходовать память
        uartLine = uartLine.substring(uartLine.length() - 1024);
      }
    }
  }
}


