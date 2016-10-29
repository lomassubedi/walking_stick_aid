const unsigned char uSonicDitchHoleTrig = 4;
const unsigned char uSonicDitchHoleEcho = 3;
const unsigned char uSonicObstTrig = 6;
const unsigned char uSonicObstEcho = 5;

const unsigned char buzz = 8;
const unsigned char PIR = 2;

const unsigned char LED_RF = 9; 
const unsigned char LED_INDI = 10; 

const unsigned char LDR = 0;

#include "NewPing.h"
#include "NewTone.h"
#include "digitalSmooth.h"

#define MAX_DISTANCE        200   // Max distance assumed to be accepeable for ultrasonic sensor
#define filterSamples       15    // Filter sample for digital smoothing should  be an odd number, no smaller than 3
#define DEBUG                     // defining DEBUG enables the Serial console printing
#define BUZZ_TIME           2000  // Buzzer ON time ms
#define LDR_NIGHT_VAL       20    // Measured ADC value through LDR for normal night light

NewPing sonarObst(uSonicObstTrig, uSonicObstEcho, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonarDitch(uSonicDitchHoleTrig, uSonicDitchHoleEcho, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

unsigned int dataDitchHole[filterSamples]; 
unsigned int dataObastacle[filterSamples];
unsigned int ldrData[filterSamples];

void setup() {  
  Serial.begin(9600);
  
  pinMode(PIR, INPUT);
  pinMode(buzz, OUTPUT);   
  pinMode(LED_RF, OUTPUT);
  pinMode(LED_INDI, OUTPUT);
  NewTone(buzz, 10, 5000);
  
  NewPing::timer_ms(50, timerCallback); // Create a Timer2 interrupt that calls toggleLED in your sketch once every 500 milliseconds.
}

void loop() {
  unsigned int distObst = 0;
  unsigned int distDitch = 0;
  unsigned int ldrValue = 0;
  bool pirStatus = 0;

  // ----------- Read Sensors -----------------------    
  distObst = digitalSmooth(sonarObst.ping_cm(), dataDitchHole);     // Get distance of ditch hole
  distDitch = digitalSmooth(sonarDitch.ping_cm(), dataObastacle);   // Get distance of obstacle
  ldrValue = digitalSmooth(analogRead(LDR), ldrData);              // Get darkness status    
  pirStatus = digitalRead(PIR);                                     // Get human presence status  
  // ------------ End : read Sensors ------------------
  
  // ------------- Print sensor values ----------------
  #ifdef DEBUG
    Serial.print("Obstacle Distance :");
    Serial.print(distObst);
    Serial.print("\tDitch Distance :");
    Serial.print(distDitch);       
    Serial.print("\tLDR Value : ");
    Serial.print(ldrValue);
    Serial.print("\tPIR :");
    Serial.println(pirStatus); 
  #endif    
  // --------------------------------------------------
  
  analogWrite(LED_RF, map(ldrValue, 0, 1023, 255, 0));

}

void timerCallback() {
  digitalWrite(LED_INDI, !digitalRead(LED_INDI)); // Toggle the LED.
}
