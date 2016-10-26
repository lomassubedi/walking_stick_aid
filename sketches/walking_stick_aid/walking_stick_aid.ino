const unsigned char uSonicDitchHoleTrig = 2;
const unsigned char uSonicDitchHoleEcho = 3;
const unsigned char uSonicObstTrig = 4;
const unsigned char uSonicObstEcho = 11;

const unsigned char buzz = 6;
const unsigned char PIR = 9;
const unsigned char LED_indi = 10; 
const unsigned char LDR = 0;

#include "NewPing.h"
#include "NewTone.h"

#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define RX_RF 7
#define TX_RF 8

#define DEBUG                 

#define TONE_WIDTH_HUMAN_OBST         50 // in ms
#define TONE_WIDTH_OBST               50
#define TONE_WIDTH_NIGHT              150
#define TONE_WIDTH_REMOT              250
#define TONE_WIDTH_DITCH              70

#define BUZZ_TIME                     2000
#define LDR_NIGHT_VAL                 20

NewPing sonarObst(uSonicObstTrig, uSonicObstEcho, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonarDitch(uSonicDitchHoleTrig, uSonicDitchHoleEcho, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup() {
  // put your setup code here, to run once:
  pinMode(uSonicDitchHoleTrig, OUTPUT);
  pinMode(uSonicDitchHoleEcho, INPUT);
  pinMode(uSonicObstTrig, OUTPUT);
  pinMode(uSonicObstEcho, INPUT);
  pinMode(PIR, INPUT);

  pinMode(buzz, OUTPUT); 
  Serial.begin(9600);
}

void loop() {
  float distObst = 0.0;
  float distDitch = 0.0;
  unsigned ldrValue = 0;
  bool pirStatus = 0;

  // ----------- Read Sensors -----------------------    
  distObst = sonarObst.ping_cm();     // Get distance of obstacle
  delay(10);
  distDitch = sonarDitch.ping_cm();   // Get distance of ditch hole
  pirStatus = digitalRead(PIR);       // Get human presence status
  ldrValue = analogRead(LDR);         // Get darkness status
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
  
  if(distObst < 50 ) {
    if(pirStatus == 1) {                  // If true Obstacle + Human 
      NewTone(buzz, 1, BUZZ_TIME);          // Tone in both true  
    } else {                              // False : Only obstacle No Human
      if(distDitch > 20) {                // If True : Obsatcle + Ditch
        NewTone(buzz, 20, BUZZ_TIME);        // Tone for Obsatcle + Ditch
      } else {                            // False : Only obstacle No Ditch
        NewTone(buzz, 10, BUZZ_TIME);        // Tone for distObst < 50 only          
    }      
    }
  } else {
    if(distDitch >= 20) {                  // If true Only ditch 
      NewTone(buzz, 3, BUZZ_TIME);    
    } else {
      //noNewTone(buzz);   
    }
  } 

  /*
  if(ldrValue < LDR_NIGHT_VAL) {
    newTone(buzz, 30, 5000);
  }*/
  
  delay(100);
}
