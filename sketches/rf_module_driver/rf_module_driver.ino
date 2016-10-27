#include <RH_ASK.h>
RH_ASK driver;

const uint8_t led = 16;
const uint8_t interruptPin = 2;
volatile uint8_t flag_trigger_char = 0;
void setup()
{
    pinMode(led, OUTPUT);  
    attachInterrupt(digitalPinToInterrupt(interruptPin), triggerChar, LOW);  
    Serial.begin(9600);   // Debugging only
    if (!driver.init()) {
      Serial.println("init failed");
      //tone(led, 5, 10000);        // Initialization failed indication 
    }         
}

void loop()
{    
    const char *msg = "ON";
    const char *msgOFF = "OFF";
    
    if(flag_trigger_char) {
      flag_trigger_char = 0;      
      driver.send((uint8_t *)msg, strlen(msg));    
      driver.waitPacketSent();
      for(uint8_t i = 0; i < 2; i++) {
        digitalWrite(led, HIGH);
        delay(100);          
        digitalWrite(led, LOW);
        delay(100);
      }     
    }
    else {
      driver.send((uint8_t *)msgOFF, strlen(msgOFF));    
      driver.waitPacketSent();                 
      digitalWrite(led, LOW);
    }
    Serial.println("Can you hear me ?, I am still awake !");    
    delay(100);
}

void triggerChar() {
  flag_trigger_char = 1;
}

