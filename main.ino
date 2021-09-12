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


#define PIN_subir 3
#define PIN_bajar 2
#define PIN_motor_subir 9
#define PIN_motor_bajar 8
#define PIN_N 1
#define PIN_display_N 10

#define DIR_ARRIBA 1
#define DIR_ABAJO 0
#define DELAY_SUBIR_MARCHA 80
#define DELAY_BAJAR_MARCHA 80



byte leva_subir = 0;
byte leva_bajar = 0;
byte neutral = 0;
byte status_leva_subir = 0;
byte status_leva_bajar = 0;
byte marcha = 0;

int maxRPM = 12500;
int tmpRPM = 5000; //variable temporal, este valor se debe leer de una entrada

void setup()
{
    Serial.begin(115200);

    pinMode(PIN_bajar,INPUT);
    pinMode(PIN_subir,INPUT);
    pinMode(PIN_N,INPUT);
    pinMode(PIN_motor_bajar,OUTPUT);
    pinMode(PIN_motor_subir,OUTPUT);

    
    while(digitalRead(PIN_N)){ 
        Serial.println("Esperando Neutral.....");
        delay(1000);
        digitalWrite(PIN_display_N,HIGH);


    } 
    Serial.println("Empezando la fiesta....");
    marcha = 1;


}

void loop(){
        
    leerEntradas();
    compruebaRPM();

    if (compruebaMarcha()){
        activarMarcha();
    }
    
    displayDatos();

    //delay(10);
}


void leerEntradas()
{

    leva_subir = digitalRead(PIN_subir);
    leva_bajar = digitalRead(PIN_bajar);
    neutral = digitalRead(PIN_N);

    // DEBUG_PRINT("  Leva sube:  "); DEBUG_PRINTDEC(leva_subir);
    // DEBUG_PRINT("  Leva baja:  "); DEBUG_PRINTDEC(leva_bajar);
    // DEBUG_PRINT("  status_leva subir:  "); DEBUG_PRINTDEC(status_leva_subir);
    // DEBUG_PRINT("  status_leva bajar:  "); DEBUG_PRINTDEC(status_leva_bajar);
    // DEBUG_PRINTLN(" ");
}

void compruebaRPM(){

 }
void(* resetFunc) (void) = 0; //declare reset function @ address 0

boolean compruebaMarcha(){
    if(neutral==1){
        digitalWrite(PIN_display_N,HIGH);
        resetFunc();  //call reset
    }
    
    if((tmpRPM<maxRPM) && (marcha>=1 && marcha<=6)) {
        digitalWrite(PIN_display_N,LOW);
        return true;
    }
    else {
    return false;
    }

    
 }
 
void displayDatos(){
    //DEBUG_PRINTLN(marcha);
    delay(200);
 }

void activarMotor(byte direccion){
    if (direccion == 1){
        digitalWrite(PIN_motor_subir,HIGH);
        delay(DELAY_SUBIR_MARCHA);
        digitalWrite(PIN_motor_subir,LOW);

    }
    else if  (direccion == 0){
        digitalWrite(PIN_motor_bajar,HIGH);
        delay(DELAY_BAJAR_MARCHA);
        digitalWrite(PIN_motor_bajar,LOW);
    }

}

void activarMarcha(){
//TODO caso de la primera

    if(status_leva_subir!=leva_subir){
            if(marcha>=1 && marcha<6 && leva_subir==1){
                activarMotor(DIR_ARRIBA);
                marcha++;
                DEBUG_PRINTLN("   Sube   ");
            }
    status_leva_subir=leva_subir;
    }

    if(status_leva_bajar!=leva_bajar){
        if (marcha<=6 && marcha>=2 && leva_bajar==1){
                    activarMotor(DIR_ABAJO);
                    marcha--;
                    DEBUG_PRINTLN("   Baja   ");
                }
    status_leva_bajar=leva_bajar;
    }
}

