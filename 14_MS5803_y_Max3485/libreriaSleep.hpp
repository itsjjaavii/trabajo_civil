#ifndef LIBRERIA_SLEEP_H
#define LIBRERIA_SLEEP_H

#include "Arduino.h"

#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5          /* Time ESP32 will go to sleep (in seconds) */

void sleepESP(uint64_t time_in_s); //Manda a dormir por esa cantidad de segundos (entera). Se recomienda iniciar condigo con millis, en una uint 65, para saber cuanto demoro la ejecucion y  calcular cuanto es un buen tiempo de sleep, o leer un RTC

#endif