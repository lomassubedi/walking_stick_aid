const unsigned char uSonicDitchHoleTrig = 2;
const unsigned char uSonicDitchHoleEcho = 3;
const unsigned char uSonicObstTrig = 4;
const unsigned char uSonicObstEcho = 5;
const unsigned char buzz = 6;
const unsigned char PIR = 9;
const unsigned char LED_indi = 10; 
const unsigned char LDR = 0;

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

float getDistance(const unsigned char trigPin, const unsigned char echoPin) {
  unsigned long duration = 0;
  float distance = 0.0;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(7);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.034)/2;
  return distance;
}

void toneCustom(unsigned int tonePulseWidth) {
  for(uint8_t i = 0; i<3; i++) {
      digitalWrite(buzz, HIGH);
      delay(tonePulseWidth);
      digitalWrite(buzz, LOW);
      delay(tonePulseWidth);
    }   
}
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

bool flag_only_obst = 0;
bool flag_human_obst = 0;
bool flag_night = 0;

void loop() {
  float distObst = 0.0;
  float distDitch = 0.0;
  unsigned ldrValue = 0;
  bool pirStatus = 0;
  
//    Serial.print("Distance measured : ");
//    Serial.println(getDistance(uSonicObstTrig, uSonicObstEcho));  

    // ----------- Read Sensors -----------------------
    distObst = getDistance(uSonicObstTrig, uSonicObstEcho);
    distDitch = getDistance(uSonicDitchHoleTrig, uSonicDitchHoleEcho);
    pirStatus = digitalRead(PIR);
    ldrValue = analogRead(LDR);
    // ------------ End : read Sensors ------------------
    
    // ------------- Print sensor values ----------------
    /*
    Serial.print("Obstacle Distance :");
    Serial.print(distObst);
    Serial.print("\tDitch Distance :");
    Serial.print(distDitch);       
    Serial.print("\tLDR Value : ");
    Serial.print(ldrValue);
    Serial.print("\tPIR :");
    Serial.println(pirStatus); 
    */
    // --------------------------------------------------
    
    if(distObst < 50 ) {
      if(pirStatus == 1) {                  // If true Obstacle + Human 
        tone(buzz, 1, BUZZ_TIME);          // Tone in both true  
      } else {                              // False : Only obstacle No Human
        if(distDitch > 20) {                // If True : Obsatcle + Ditch
          tone(buzz, 20, BUZZ_TIME);        // Tone for Obsatcle + Ditch
        } else {                            // False : Only obstacle No Ditch
          tone(buzz, 10, BUZZ_TIME);        // Tone for distObst < 50 only          
      }      
      }
    } else {
      if(distDitch > 20) {                  // If true Only ditch 
        tone(buzz, 3, BUZZ_TIME);    
      } else {
        //noTone(buzz);   
      }
    } 

    /*
    if(ldrValue < LDR_NIGHT_VAL) {
      tone(buzz, 30, 5000);
    }*/
    
  //delay(500);
}
