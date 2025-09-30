#include "globals.h"

// настройки/инициализация
const int L = GRAPH_BUFFER_LEN;
int TShead = 0;
int TStail = 0;
float TStorage[GRAPH_BUFFER_LEN] = {0.0f};
int HScale = 3;   // например 2 px per sample (подкорректируй)
int Scale = 4;    // вертикальный множитель
int LLL = 0;

// примеры значений датчиков/PID — фактически у тебя их нужно обновлять в коде
float sensor1 = 0.0f;
float sensor2 = 0.0f;
float sensor3 = 0.0f;
float sensor4 = 0.0f;
float sensorCentral = 0.0f;

float pid1 = 0.0f;
float pid2 = 0.0f;
float pid3 = 0.0f;
float pid4 = 0.0f;

bool configDirty = false;
unsigned long configDirtySince = 0;
const unsigned long CONFIG_SAVE_DEBOUNCE_MS = 5000UL; // время обновления последней температуры
