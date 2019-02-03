#include <Servo.h> 
#include <FastLED.h>

#define revSwitchPin 8
#define trigSwitchPin A0
#define cyclerPin 11

#define flywheelsPin 6
#define solenoidPin 3
#define fanPin 9

#define LED_PIN 12
#define NUM_LEDS 4

CRGB leds[NUM_LEDS];

Servo fan;  // create servo object to control the fan
 
void setup() 
{ 
  pinMode(revSwitchPin, INPUT_PULLUP);
  pinMode(trigSwitchPin, INPUT_PULLUP);
  pinMode(flywheelsPin, OUTPUT);
  pinMode(cyclerPin,INPUT);

  digitalWrite(flywheelsPin, LOW);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  
  Serial.begin(115200);
  fan.attach(fanPin);
  delay(1000);
  Serial.println("attached");
  fan.write(770); 
  delay(1000);
  Serial.println("FanInitComplete");
  

} 

int fireMode = 0; //semi auto, 1-3 burst, full auto

const int numModes = 2;

//Boolean used to make sure that fireMode only changes once every time the touch sensor is pressed
bool hasCycled = false;

//Boolean used to make sure that only one dart, or only three darts are fired for every pull of the trigger
//Depends upon which fireMode the blaster is in
bool hasFired = false;

void fire(int burstLength){
  digitalWrite(solenoidPin, HIGH);
  delay(burstLength);
  digitalWrite(solenoidPin,LOW);
  Serial.println("fired");
}

float triggerVals[] = {0,0,0,0,0};

void updateTriggerVals(int val){
  for(int i = 0; i < 4; i++){
    triggerVals[i] = triggerVals[i+1];
  }
  triggerVals[4] = val;
}

int meanTriggerVal(){
  int total = 0;
  for(int i = 0; i < 5; i++){
    total += triggerVals[i];
  }
  return total/5;
}

void loop() 
{
   bool revving = !digitalRead(revSwitchPin);
   updateTriggerVals(analogRead(trigSwitchPin));
   bool triggerState = meanTriggerVal() < 50; //Checks whether the trigger has been pulled or not and assigns this to a boolean
   //Serial.println(revving);
   if(revving){//If rev trigger is held...
       fan.write(2100);//activate ducted fan
       digitalWrite(flywheelsPin,HIGH);//Spin up the flywheels by activating the MOSFETs. Two MOSFETs on the same pin
       if(fireMode == 0){//If the fireMode is single shot...
            if(triggerState && !hasFired){// and the trigger is held and you haven't fired before on this trigger pull...
                fire(20);//Fire a single shot
                hasFired = true;//Set the hasFired flag to true
            }
            if(!triggerState){//Checks if the trigger is released 
                hasFired = false;//Resets hasFired to false to the blaster can fire again
            }
        }
        /*if(fireMode == 1){//If the fireMode is 3-burst...
            if(triggerState && !hasFired){// and the trigger is held and you haven't fired before on this trigger pull...
                fire(120);//Fire a burst of 3 rounds
                hasFired = true;//Set the hasFired flag to true
            }
            if(!triggerState){//Checks if the trigger is released 
                hasFired = false;//Resets hasFired to false to the blaster can fire again
            }
        }*/
        if(fireMode == 1){//If the fireMode is full auto...
            if(triggerState){//and the trigger is held...
               digitalWrite(solenoidPin, HIGH);//lower the solenoid gate.
               Serial.println("firing");
            }
            else{//if the trigger is not held...
              digitalWrite(solenoidPin, LOW);//raise the solenoid gate.
            }
        }
    }           
    else{//If rev trigger is not held...
        fan.write(780);//turn off fan.
        digitalWrite(flywheelsPin,LOW);//turn off flywheels.
    }
  
    if(digitalRead(cyclerPin) && !hasCycled){//Checks to see if the touch sensor is pressed and the blaster has not changed fire mode yet
        //Cycles the fireMode between 0,1, and 2 in increments of 1
        fireMode += 1;
        fireMode = fireMode%numModes;

        for(int i = 0; i < NUM_LEDS; i++){
          leds[i] = CRGB(0, 0, 0);
        }
        for(int i = 0; i < (fireMode+1)*2; i++){
          leds[i] = CRGB(0, 100, 0);
        }
        FastLED.show();
        
        hasCycled = true;//Sets the hasCycled flag to true so the blaster will only cycle once for this activation of the touch sensor.
        Serial.println(fireMode);//Print the fireMode to the serial monitor
    }
    else if(!digitalRead(cyclerPin)){//Checks if the touch sensor is not being pressed
        hasCycled = false;//Sets the hasCycled flag to false to allow the blaster to cycle the fireMode again.
    }
} 
