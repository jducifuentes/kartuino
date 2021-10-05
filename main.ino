#include <Servo.h>
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

#define PIN_subir 11
#define PIN_bajar 12
#define PIN_N 13
#define PIN_motor_subir 9
#define PIN_motor_bajar 10
#define PIN_display_N 8 

#define SUBE HIGH
#define BAJA LOW
#define DELAY_SUBIR_MARCHA 200
#define DELAY_BAJAR_MARCHA 200

#define REFRESCO 600

uint8_t leva_subir = 0;
uint8_t leva_bajar = 0;
boolean neutral = 0;
boolean status_leva_subir = 1;
boolean status_leva_bajar = 1;
byte marcha = 0;
byte marcha_ant = 1;

uint16_t maxRPM = 12500;
uint16_t rpm = 0;
uint16_t maxRPMbajar = 7000;
int tmpRPM = 5000; //variable temporal, este valor se debe leer de una entrada


uint32_t tiempo_marcha_subir=millis(); //variables para el control del tiempo de  pulso de activacion del actuador
uint32_t tiempo_marcha_bajar=millis();

uint32_t mymillis=0; //contador de tiempo  para el refresco de la visualizacion de datos

///**** temporal display 7 segmentos
#include <TM1638plus.h>
const int strobe = 5;
const int clock = 6;
const int data = 7;

bool high_freq = false; //default false,, If using a high freq CPU > ~100 MHZ set to true. 
//Constructor object (GPIO STB , GPIO CLOCK , GPIO DIO, use high freq MCU default false)
TM1638plus tm(strobe, clock ,data, high_freq);
///****

// *** ** ** * *** ** ** **** * ** *  Chapuzas del puto arduino


///////////////
// Calibration:
///////////////

const byte PulsesPerRevolution = 1;  // Set how many pulses there are on each revolution. Default: 2.


// If the period between pulses is too high, or even if the pulses stopped, then we would get stuck showing the
// last value instead of a 0. Because of this we are going to set a limit for the maximum period allowed.
// If the period is above this value, the RPM will show as 0.
// The higher the set value, the longer lag/delay will have to sense that pulses stopped, but it will allow readings
// at very low RPM.
// Setting a low value is going to allow the detection of stop situations faster, but it will prevent having low RPM readings.
// The unit is in microseconds.
const unsigned long ZeroTimeout = 100000;  // For high response time, a good value would be 100000.
                                           // For reading very low RPM, a good value would be 300000.


// Calibration for smoothing RPM:
const byte numReadings = 2;  // Number of samples for smoothing. The higher, the more smoothing, but it's going to
                             // react slower to changes. 1 = no smoothing. Default: 2.


/////////////
// Variables:
/////////////

volatile unsigned long LastTimeWeMeasured;  // Stores the last time we measured a pulse so we can calculate the period.
volatile unsigned long PeriodBetweenPulses = ZeroTimeout+1000;  // Stores the period between pulses in microseconds.
                       // It has a big number so it doesn't start with 0 which would be interpreted as a high frequency.
volatile unsigned long PeriodAverage = ZeroTimeout+1000;  // Stores the period between pulses in microseconds in total, if we are taking multiple pulses.
                       // It has a big number so it doesn't start with 0 which would be interpreted as a high frequency.
unsigned long FrequencyRaw;  // Calculated frequency, based on the period. This has a lot of extra decimals without the decimal point.
unsigned long FrequencyReal;  // Frequency without decimals.
unsigned long RPM;  // Raw RPM without any processing.
unsigned int PulseCounter = 1;  // Counts the amount of pulse readings we took so we can average multiple pulses before calculating the period.

unsigned long PeriodSum; // Stores the summation of all the periods to do the average.

unsigned long LastTimeCycleMeasure = LastTimeWeMeasured;  // Stores the last time we measure a pulse in that cycle.
                                    // We need a variable with a value that is not going to be affected by the interrupt
                                    // because we are going to do math and functions that are going to mess up if the values
                                    // changes in the middle of the cycle.
unsigned long CurrentMicros = micros();  // Stores the micros in that cycle.
                                         // We need a variable with a value that is not going to be affected by the interrupt
                                         // because we are going to do math and functions that are going to mess up if the values
                                         // changes in the middle of the cycle.

// We get the RPM by measuring the time between 2 or more pulses so the following will set how many pulses to
// take before calculating the RPM. 1 would be the minimum giving a result every pulse, which would feel very responsive
// even at very low speeds but also is going to be less accurate at higher speeds.
// With a value around 10 you will get a very accurate result at high speeds, but readings at lower speeds are going to be
// farther from eachother making it less "real time" at those speeds.
// There's a function that will set the value depending on the speed so this is done automatically.
unsigned int AmountOfReadings = 1;

unsigned int ZeroDebouncingExtra;  // Stores the extra value added to the ZeroTimeout to debounce it.
                                   // The ZeroTimeout needs debouncing so when the value is close to the threshold it
                                   // doesn't jump from 0 to the value. This extra value changes the threshold a little
                                   // when we show a 0.

// Variables for smoothing tachometer:
unsigned long readings[numReadings];  // The input.
unsigned long readIndex;  // The index of the current reading.
unsigned long total;  // The running total.
unsigned long average;  // The RPM value after applying the smoothing.



unsigned int sensorValue = 0;  // variable to store the value coming from the sensor

/// **** *** ** * ** * ** * ** * chapuzas del puto arduino 


void setup()
{
myservo.attach(4);


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

// ****

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

void compruebaRPM(){



 LastTimeCycleMeasure = LastTimeWeMeasured;  // Store the LastTimeWeMeasured in a variable.
 CurrentMicros = micros();  // Store the micros() in a variable.

  // CurrentMicros should always be higher than LastTimeWeMeasured, but in rare occasions that's not true.
  // I'm not sure why this happens, but my solution is to compare both and if CurrentMicros is lower than
  // LastTimeCycleMeasure I set it as the CurrentMicros.
  // The need of fixing this is that we later use this information to see if pulses stopped.
  if(CurrentMicros < LastTimeCycleMeasure)
  {
    LastTimeCycleMeasure = CurrentMicros;
  }

  // Calculate the frequency:
  FrequencyRaw = 10000000000 / PeriodAverage;  // Calculate the frequency using the period between pulses.

  
  // Detect if pulses stopped or frequency is too low, so we can show 0 Frequency:
  if(PeriodBetweenPulses > ZeroTimeout - ZeroDebouncingExtra || CurrentMicros - LastTimeCycleMeasure > ZeroTimeout - ZeroDebouncingExtra)
  {  // If the pulses are too far apart that we reached the timeout for zero:
    FrequencyRaw = 0;  // Set frequency as 0.
    ZeroDebouncingExtra = 2000;  // Change the threshold a little so it doesn't bounce.
  }
  else
  {
    ZeroDebouncingExtra = 0;  // Reset the threshold to the normal value so it doesn't bounce.
  }

  FrequencyReal = FrequencyRaw / 10000;  // Get frequency without decimals.
                                          // This is not used to calculate RPM but we remove the decimals just in case
                                          // you want to print it.

  // Calculate the RPM:
  RPM = FrequencyRaw / PulsesPerRevolution * 60;  // Frequency divided by amount of pulses per revolution multiply by
                                                  // 60 seconds to get minutes.
  RPM = RPM / 10000;  // Remove the decimals.


  // Smoothing RPM:
  total = total - readings[readIndex];  // Advance to the next position in the array.
  readings[readIndex] = RPM;  // Takes the value that we are going to smooth.
  total = total + readings[readIndex];  // Add the reading to the total.
  readIndex = readIndex + 1;  // Advance to the next position in the array.

  if (readIndex >= numReadings)  // If we're at the end of the array:
  {
    readIndex = 0;  // Reset array index.
  }
  
  // Calculate the average:
  average = total / numReadings;  // The average value it's the smoothed result.

  rpm=average;

}
 


void(* resetFunc) (void) = 0; //declare reset function @ address 0

boolean compruebaMarcha(){

  //TODO control de cuantas marchas se piden y cuantas se activan. Leer sensores hall de entrada de marcha


    if(neutral==1){
        digitalWrite(PIN_display_N,HIGH);
        resetFunc();  //call reset
        
    }
    
    // //ñapa para desactivar los actuadores y no meter un delay
    //     if (millis()-tiempo_marcha_subir>DELAY_SUBIR_MARCHA){
    //               //digitalWrite(PIN_motor_subir,LOW);
    //               actuaMotor(SUBE,LOW);
    //               tiempo_marcha_subir=millis();
    //             }

    //     if (millis()-tiempo_marcha_bajar>DELAY_BAJAR_MARCHA){
    //               //digitalWrite(PIN_motor_bajar,LOW);
    //               actuaMotor(BAJA,LOW);
    //               tiempo_marcha_bajar=millis();
    //             }


    if((rpm<maxRPM) && (marcha>=1 && marcha<=6)) {
        digitalWrite(PIN_display_N,LOW);
        return true;
    }
    else {
    return false;
    }

    
 }
 void actuaMotor(boolean direccion, boolean activa){

  //  if (direccion == SUBE){
  //    digitalWrite(PIN_motor_subir,activa);
     
  //  }else if (direccion==BAJA){
  //    digitalWrite(PIN_motor_bajar,activa);
  //  }
  
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
 
void displayDatos(){
       
    if (mymillis-millis() >=REFRESCO){

      mymillis=millis();
   
      if (marcha!=marcha_ant){
        for (byte i=0;i<8;i++){tm.setLED(i,0);}
      marcha_ant=marcha;
      }
      tm.setLED(marcha-1, 1);
      tm.displayIntNum(rpm, true);
   
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

if (rpm<maxRPMbajar){
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




void Pulse_Event()  // The interrupt runs this to calculate the period between pulses:
{

  PeriodBetweenPulses = micros() - LastTimeWeMeasured;  // Current "micros" minus the old "micros" when the last pulse happens.
                                                        // This will result with the period (microseconds) between both pulses.
                                                        // The way is made, the overflow of the "micros" is not going to cause any issue.

  LastTimeWeMeasured = micros();  // Stores the current micros so the next time we have a pulse we would have something to compare with.





  if(PulseCounter >= AmountOfReadings)  // If counter for amount of readings reach the set limit:
  {
    PeriodAverage = PeriodSum / AmountOfReadings;  // Calculate the final period dividing the sum of all readings by the
                                                   // amount of readings to get the average.
    PulseCounter = 1;  // Reset the counter to start over. The reset value is 1 because its the minimum setting allowed (1 reading).
    PeriodSum = PeriodBetweenPulses;  // Reset PeriodSum to start a new averaging operation.


    // Change the amount of readings depending on the period between pulses.
    // To be very responsive, ideally we should read every pulse. The problem is that at higher speeds the period gets
    // too low decreasing the accuracy. To get more accurate readings at higher speeds we should get multiple pulses and
    // average the period, but if we do that at lower speeds then we would have readings too far apart (laggy or sluggish).
    // To have both advantages at different speeds, we will change the amount of readings depending on the period between pulses.
    // Remap period to the amount of readings:
    int RemapedAmountOfReadings = map(PeriodBetweenPulses, 40000, 5000, 1, 10);  // Remap the period range to the reading range.
    // 1st value is what are we going to remap. In this case is the PeriodBetweenPulses.
    // 2nd value is the period value when we are going to have only 1 reading. The higher it is, the lower RPM has to be to reach 1 reading.
    // 3rd value is the period value when we are going to have 10 readings. The higher it is, the lower RPM has to be to reach 10 readings.
    // 4th and 5th values are the amount of readings range.
    RemapedAmountOfReadings = constrain(RemapedAmountOfReadings, 1, 10);  // Constrain the value so it doesn't go below or above the limits.
    AmountOfReadings = RemapedAmountOfReadings;  // Set amount of readings as the remaped value.
  }
  else
  {
    PulseCounter++;  // Increase the counter for amount of readings by 1.
    PeriodSum = PeriodSum + PeriodBetweenPulses;  // Add the periods so later we can average.
  }

}  // End of Pulse_Event.
