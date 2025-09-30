#pragma once

// размер буфера точек для графика
#ifndef GRAPH_BUFFER_LEN
#define GRAPH_BUFFER_LEN 100


extern const int L;           // длина буфера (GRAPH_BUFFER_LEN)
extern int TShead;            // индекс головы (след. запись)
extern int TStail;            // индекс хвоста (старшая запись)
extern float TStorage[];      // массив значений температуры
extern int HScale;            // горизонтальный масштаб (px per sample)
extern int Scale;             // вертикальный масштаб multiplier
extern int LLL;               // для отладки/ длина svg (если нужен)

// значения датчиков и PID (используются в /sensors и /pids)
extern float sensor1, sensor2, sensor3, sensor4, sensorCentral;
extern float pid1, pid2, pid3, pid4;

extern bool configDirty;
extern unsigned long configDirtySince;
extern const unsigned long CONFIG_SAVE_DEBOUNCE_MS;
#endif