#include <Servo.h>
#include <Arduino.h>
#include "rpm.h"
#include "display.h"
#include "config.h"

Servo myservo;
//#define DEBUG 


#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTDEC(x) Serial.print(x, DEC)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTDEC(x)
  #define DEBUG_PRINTLN(x) 
#endif






uint8_t leva_subir = 0;
uint8_t leva_bajar = 0;
boolean neutral = 0;
boolean status_leva_subir = 1;
boolean status_leva_bajar = 1;
byte marcha = 0;
byte marcha_ant = 1;


uint16_t RPMS = 0;



uint32_t tiempo_marcha_subir=millis(); //variables para el control del tiempo de  pulso de activacion del actuador
uint32_t tiempo_marcha_bajar=millis();

void setup()
{
  myservo.attach(4);
  setupDisplay();

    Serial.begin(9600);

    pinMode(PIN_bajar,INPUT);
    pinMode(PIN_subir,INPUT);
    pinMode(PIN_N,INPUT);
    pinMode(PIN_motor_bajar,OUTPUT);
    pinMode(PIN_motor_subir,OUTPUT);
    pinMode(PIN_display_N,OUTPUT);

    
    while(digitalRead(PIN_N)){ 
        Serial.println("Esperando Neutral.....");
        delay(1000);
        digitalWrite(PIN_display_N,HIGH);
    } 

    Serial.println("Empezando la fiesta....");
    marcha = 1;
    attachInterrupt(digitalPinToInterrupt(2), Pulse_Event, RISING);  // Enable interruption pin 2 when going from LOW to HIGH.
}

void loop(){
   
    leerEntradas();
    compruebaRPM();

    if (compruebaMarcha()){
        activarMarcha();
    }
     displayDatos();

}

void leerEntradas()
{

    leva_subir = digitalRead(PIN_subir);
    leva_bajar = digitalRead(PIN_bajar);

    neutral = digitalRead(PIN_N);
}


 void(* resetFunc) (void) = 0; //declare reset function @ address 0

boolean compruebaMarcha(){

  //TODO control de cuantas marchas se piden y cuantas se activan. Leer sensores hall de entrada de marcha


    if(neutral==1){
        digitalWrite(PIN_display_N,HIGH);
        resetFunc();  //call reset
        
    }
    

    if((RPMS<maxRPM) && (marcha>=1 && marcha<=6)) {
        digitalWrite(PIN_display_N,LOW);
        return true;
    }
    else {
    return false;
    }

    
 }
 void actuaMotor(boolean direccion, boolean activa){

   if (direccion == SUBE and activa == HIGH){
        myservo.write(180);
        digitalWrite(PIN_motor_subir,HIGH);
 
   }else if (direccion == BAJA and activa == HIGH){
     myservo.write(0);
      digitalWrite(PIN_motor_bajar,HIGH);
   }
 if (activa == LOW){
      myservo.write(90);
      digitalWrite(PIN_motor_bajar,LOW);
      digitalWrite(PIN_motor_subir,LOW);
  }

 }
 
void activarMarcha(){
//TODO caso de la primera

    if(status_leva_subir!=leva_subir){
            if(marcha>=1 && marcha<6 && leva_subir==1){
                actuaMotor(SUBE,HIGH);
                delay(DELAY_SUBIR_MARCHA);
                actuaMotor(SUBE,LOW);
                marcha++;
                //DEBUG_PRINTLN("   Sube   ");
            }
    status_leva_subir=leva_subir;
    }

if (RPMS<maxRPMbajar){
    if(status_leva_bajar!=leva_bajar){
        if (marcha<=6 && marcha>=2 && leva_bajar==1){
                    actuaMotor(BAJA,HIGH);
                    delay(DELAY_BAJAR_MARCHA);
                    actuaMotor(BAJA,LOW);
                    marcha--;
                    //DEBUG_PRINTLN("   Baja   ");
                }
    status_leva_bajar=leva_bajar;
    }
}

}
