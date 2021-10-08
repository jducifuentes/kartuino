#include <Arduino.h>
#ifndef CONFIG_H
#define CONFIG_H



// PINES

#define PIN_subir 11
#define PIN_bajar 12
#define PIN_N 13
#define PIN_motor_subir 9
#define PIN_motor_bajar 10
#define PIN_display_N 8 




#define SUBE HIGH
#define BAJA LOW

#define DELAY_SUBIR_MARCHA 100
#define DELAY_BAJAR_MARCHA 100
#define REFRESCO 600

#define maxRPM  12500
#define maxRPMbajar  7000




#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTDEC(x) Serial.print(x, DEC)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTDEC(x)
  #define DEBUG_PRINTLN(x) 
#endif


#endif
