#define revSwitchPin 8
#define trigSwitchPin A0
#define cyclerPin 4

#define flywheelsPin 6
#define solenoidPin 3
#define fanPin 9

#include <Servo.h> 

Servo fan;  // create servo object to control the fan
 
void setup() 
{ 
  Serial.begin(115200);
  fan.attach(fanPin);
  delay(2000);
  Serial.println("attached");
  fan.write(770); 
  delay(3000);
  Serial.println("FanInitComplete");
  
  pinMode(revSwitchPin, INPUT_PULLUP);
  pinMode(trigSwitchPin, INPUT_PULLUP);
  pinMode(flywheelsPin, OUTPUT);
  pinMode(cyclerPin,INPUT);
} 

int fireMode = 0; //1-3 burst, full auto

const int numModes = 3;

//Boolean used to make sure that fireMode only changes once every time the touch sensor is pressed
bool hasCycled = false;

//Boolean used to make sure that only one dart, or only three darts are fired for every pull of the trigger
//Depends upon which fireMode the blaster is in
bool hasFired = false;

void fire(int burstLength){
  digitalWrite(solenoidPin, HIGH);
  delay(burstLength);
  digitalWrite(solenoidPin,LOW);
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
 
 if(revving){
     fan.write(2100);
     digitalWrite(flywheelsPin,HIGH);
     if(fireMode == 0){
        if(triggerState && !hasFired){
            fire(20);
            hasFired = true;
        }
        if(!triggerState){//Checks if the trigger is released 
            hasFired = false;//Resets hasFired to false to the blaster can fire again
        }
      }
      if(fireMode == 1){
        if(triggerState && !hasFired){
            fire(120);
            hasFired = true;
        }
        if(!triggerState){//Checks if the trigger is released 
            hasFired = false;//Resets hasFired to false to the blaster can fire again
        }
      }
      if(fireMode == 2){//Checks if the blaster is currently in full auto mode
          if(triggerState){//Checks to see if the trigger is held
             digitalWrite(solenoidPin, HIGH);
          }
          else{
            digitalWrite(solenoidPin, LOW);
          }
      }
  }           
  else{
      fan.write(780); 
      digitalWrite(flywheelsPin,LOW);
      //Serial.print("FanOFF");
  }

  if(digitalRead(cyclerPin) && !hasCycled){//Checks to see if the touch sensor is pressed and the blaster has not changed fire mode yet
    //Cycles the fireMode between 0,1, and 2 in increments of 1
    fireMode += 1;
    fireMode = fireMode%numModes;
    
    hasCycled = true;//Sets the hasCycled flag to true so the blaster will only cycle once for this activation of the touch sensor.
    Serial.println(fireMode);//Print the fireMode to the serial monitor
  }
  else if(!digitalRead(cyclerPin)){//Checks if the touch sensor is not being pressed
    hasCycled = false;//Sets the hasCycled flag to false to allow the blaster to cycle the fireMode again.
  }
  
} 
