#include "graph.h"
#include "globals.h"
#include "app.h"    // для currentSetpoint
#include <Arduino.h>

static float clampf(float x, float a, float b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}

String drawGraphSvg() {
  const float MIN_SPAN = 6.0f;   // минимальный вертикальный диапазон (°C)
  const float MARGIN = 1.2f;     // запас сверху/снизу

  int graphWidth = max(120, L * HScale);
  int graphHeight = max(40, 50 * Scale);

  // собираем статистику по буферу
  float minv = 1e6f, maxv = -1e6f, sum = 0.0f;
  int cnt = 0;
  int i = TStail;
  while (i != TShead) {
    float v = TStorage[i];
    if (!isnan(v)) {
      if (v < minv) minv = v;
      if (v > maxv) maxv = v;
      sum += v;
      cnt++;
    }
    i = (i + 1) % L;
    if (cnt >= L) break;
  }
  float mean = (cnt > 0) ? (sum / cnt) : 0.0f;

  String out = "";
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"100%\" height=\"auto\" viewBox=\"0 0 ";
  out += String(graphWidth);
  out += " ";
  out += String(graphHeight);
  out += "\" preserveAspectRatio=\"xMidYMid meet\">";

  out += "<rect width=\"" + String(graphWidth) + "\" height=\"" + String(graphHeight) + "\" fill=\"#ffffff\" stroke=\"#000088\" stroke-width=\"1\" />\n";

  if (cnt <= 0) {
    out += "<text x=\"10\" y=\"" + String(graphHeight/2) + "\" font-size=\"14\" fill=\"#000088\">Нет данных для графика</text>";
    out += "</svg>";
    LLL = out.length();
    return out;
  }

  // Центрируем график по среднему (чтобы линия оставалась в центре)
  float mid = mean;

  float observedSpan = (maxv > minv) ? (maxv - minv) : 0.0f;
  float span = max(MIN_SPAN, observedSpan * MARGIN);

  float scaleY = (span > 1e-6f) ? ((float)graphHeight / span) : 1.0f;
  float centerY = (float)graphHeight / 2.0f;

  // подписи верх/низ
  float topValue = mid + span / 2.0f;
  float bottomValue = mid - span / 2.0f;
  out += "<text x=\"5\" y=\"16\" font-size=\"12px\" fill=\"#000088\">" + String(topValue, 2) + " °C</text>\n";
  out += "<text x=\"5\" y=\"" + String(graphHeight - 6) + "\" font-size=\"12px\" fill=\"#000088\">" + String(bottomValue, 2) + " °C</text>\n";

  // строим points для polyline
  String points = "";
  int step = 0;
  i = TStail;
  while (i != TShead) {
    float v = TStorage[i];
    float yf = centerY + (mid - v) * scaleY;
    yf = clampf(yf, 0.0f, (float)graphHeight);
    int xi = step * HScale;
    if (xi > graphWidth) xi = graphWidth;
    points += String(xi) + "," + String((int)round(yf)) + " ";
    step++;
    i = (i + 1) % L;
    if (step >= L) break;
  }

  if (step == 1) {
    // если одна точка — рисуем кружок
    int xi = 0;
    int yi = 0;
    int comma = points.indexOf(',');
    if (comma > 0) {
      xi = points.substring(0, comma).toInt();
      int sp = points.indexOf(' ', comma+1);
      if (sp > comma) yi = points.substring(comma+1, sp).toInt();
      else yi = points.substring(comma+1).toInt();
    }
    out += "<circle cx=\"" + String(xi) + "\" cy=\"" + String(yi) + "\" r=\"2\" fill=\"#000088\" />\n";
  } else {
    out += "<polyline points=\"" + points + "\" fill=\"none\" stroke=\"#000088\" stroke-width=\"1\" stroke-linejoin=\"round\" stroke-linecap=\"round\" />\n";
  }

  // Рисуем горизонтальную линию для setpoint 
  float sp = currentSetpoint;
  float limit = span * 4.0f;
  if (isfinite(sp) && fabs(sp - mid) <= limit) {
    float ysp = centerY + (mid - sp) * scaleY;
    ysp = clampf(ysp, 0.0f, (float)graphHeight);
    out += "<line x1=\"0\" y1=\"" + String((int)round(ysp)) + "\" x2=\"" + String(graphWidth) + "\" y2=\"" + String((int)round(ysp)) + "\" stroke=\"#d9534f\" stroke-width=\"1\" stroke-dasharray=\"4,3\" />\n";
    out += "<text x=\"" + String(graphWidth - 120) + "\" y=\"" + String((int)round(ysp) - 4) + "\" font-size=\"11\" fill=\"#d9534f\">set: " + String(sp,1) + "°C</text>\n";
  }

  // Добавляем надпись current в правый нижний угол
  float currentTemp = TStorage[(TShead - 1 + L) % L]; // последнее значение центрального датчика
  if (isfinite(currentTemp)) {
    out += "<text x=\"" + String(graphWidth - 120) + "\" y=\"" + String(graphHeight - 6) + "\" font-size=\"11\" fill=\"#d9534f\">current: " 
           + String(currentTemp,1) + "°C</text>\n";
  }

  out += "</svg>\n";
  LLL = out.length();
  return out;
}
