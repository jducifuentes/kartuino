
#include <Arduino.h>
#include "display.h"
#include <TM1638plus.h>

extern byte marcha;
extern byte marcha_ant;
extern uint16_t RPMS;

uint32_t mymillis=0;
const int strobe = 5;
const int clock = 6;
const int data = 7;

bool high_freq = false; //default false,, If using a high freq CPU > ~100 MHZ set to true. 
//Constructor object (GPIO STB , GPIO CLOCK , GPIO DIO, use high freq MCU default false)
TM1638plus tm(strobe, clock ,data, high_freq);

void displayDatos(){
  
       
    if (mymillis-millis()>=REFRESCO){

      mymillis=millis();
   
      if (marcha!=marcha_ant){
        for (byte i=0;i<8;i++){tm.setLED(i,0);}
      marcha_ant=marcha;
      }
      tm.setLED(marcha-1, 1);
      tm.displayIntNum(RPMS, true);
   
    }
      
 }

 void setupDisplay(){
  // // //  **** temporal display 7 segmentos
pinMode(strobe, OUTPUT);
pinMode(clock, OUTPUT);
pinMode(data, OUTPUT);

  tm.displayBegin();
  delay(500);
 // Test 0 reset test
  tm.setLED(0, 1);
  delay(500);
  tm.reset();
  tm.displayIntNum(marcha, false);
  
  
  
  }
