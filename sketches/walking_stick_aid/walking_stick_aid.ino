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
#include "RH_ASK.h"
#include "digitalSmooth.h"

#define MAX_DISTANCE        200   // Max distance assumed to be accepeable for ultrasonic sensor
#define filterSamples       15    // Filter sample for digital smoothing should  be an odd number, no smaller than 3
#define DEBUG                     // defining DEBUG enables the Serial console printing
#define BUZZ_TIME           2000  // Buzzer ON time ms

#define LDR_NIGHT_VAL       500    // Measured ADC value through LDR for normal night light
#define DIST_OBST           30    // Assumed distane of Obstalce on its presence (in cm)
#define DIST_DTH_HOLE       20    // More than 30cm is assumed to be presence of ditch hole

#define TONE_FREQ_REMOTE      1      
#define TONE_FREQ_DITCH       5
#define TONE_FREQ_OBST        10
#define TONE_FREQ_HUMAN       50
#define TONE_FREQ_OBST_HUMN   100
#define TONE_FREQ_DITCH_OBST  500
#define TONE_FREQ_NIGHT       1000

#define TICK_COUNT            12000UL

RH_ASK rexRF;      // Create ASK RF module object
NewPing sonarObst(uSonicObstTrig, uSonicObstEcho, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonarDitch(uSonicDitchHoleTrig, uSonicDitchHoleEcho, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

unsigned int dataDitchHole[filterSamples]; 
unsigned int dataObastacle[filterSamples];
unsigned int ldrData[filterSamples];
volatile unsigned long tickCount = 0;

void setup() {  
  Serial.begin(9600);

  if (!rexRF.init())
    Serial.println("RF Receiver init failed....");
  delay(5000);         
    
  pinMode(PIR, INPUT);
  pinMode(buzz, OUTPUT);   
  pinMode(LED_RF, OUTPUT);
  pinMode(LED_INDI, OUTPUT);
  NewTone(buzz, 20, 5000);
  
  NewPing::timer_ms(50, timerCallback); // Create a Timer2 interrupt that calls timerCallback every 500 milliseconds.
}

void loop() {
  unsigned int distObst = 0;
  unsigned int distDitch = 0;
  unsigned int ldrValue = 0;
  bool pirStatus = 0, flag_night_first_hit = 0;

  uint8_t buf[12];
  uint8_t buflen = sizeof(buf);

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
    
  if(obstacle(distObst)) {                                   // TRUE : Obstacle present
    if(pirStatus) {                                          // TRUE : Obstacle is Human
      NewTone(buzz, TONE_FREQ_OBST_HUMN, BUZZ_TIME);
    } else {
      if(ditch(distDitch)) {                                 // If True : Obsatcle + Ditch
        NewTone(buzz, TONE_FREQ_DITCH_OBST, BUZZ_TIME);      // Tone for Obsatcle + Ditch
      } else {                                               // False : Only obstacle No Ditch
        NewTone(buzz, TONE_FREQ_OBST, BUZZ_TIME);            // Tone for distObst < 50 only          
      }      
    }    
  }
  
  if(ditch(distDitch)) {                                     // TRUE : Only ditch detected
    NewTone(buzz, TONE_FREQ_DITCH, BUZZ_TIME);
  }
  
  // ----------- Enable alarm periodically on darkness -------//
  if((ldrValue <= LDR_NIGHT_VAL) && (tickCount >= TICK_COUNT)){        
    tickCount = 0;
    NewTone(buzz, TONE_FREQ_NIGHT, (2 * BUZZ_TIME));           
  }

  //-- Deal with information comming from remote -----//
  if (rexRF.recv(buf, &buflen)){               // Non-blocking 
      int i;
      // Message with a good checksum received, dump it.
      #ifdef DEBUG
        Serial.print("Message from Remote: ");
        Serial.println((char*)buf);         
      #endif
  } 
  if(buf[0] == 'O' && buf[1] == 'N') {
    digitalWrite(LED_RF, HIGH);
    NewTone(buzz, TONE_FREQ_REMOTE, (2 * BUZZ_TIME));
  } else{
    digitalWrite(LED_RF, LOW);               
  }     
}

void timerCallback() {
  digitalWrite(LED_INDI, !digitalRead(LED_INDI)); // Toggle the LED.
  tickCount++;
  if(tickCount > TICK_COUNT) {
    tickCount = TICK_COUNT;
  }
}

bool obstacle(unsigned int distance) {
  if((distance <=  DIST_OBST) && (distance > 0)) {
    return 1;
  } else {
    return 0;
  }
}

bool ditch(unsigned int distance) {
  if((distance >  DIST_DTH_HOLE) && (distance > 0)) {
    return 1;
  } else {
    return 0;
  }
}

