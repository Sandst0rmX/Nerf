//Defines pins for Solenoid and Flywheel MOSFETs 
#define SOLENOID A0
#define FLYWHEELS A3

//Defines pins for microswitches behind rev trigger and main trigger
#define REVSWITCH A4
#define TRIGGER A5

//Defines pin for touch sensor that controls fire mode
#define CYCLEMODE A2

int fireMode = 0;
// 0: semiAuto, 1:3Burst, 2:Auto

void setup() {
  pinMode(SOLENOID,OUTPUT);
  pinMode(FLYWHEELS,OUTPUT);
  pinMode(REVSWITCH, INPUT_PULLUP);
  pinMode(TRIGGER, INPUT_PULLUP);
  digitalWrite(FLYWHEELS, LOW);//Start flywheel MOSFET at LOW so the pin value isn't floating
  Serial.begin(115200);

}

//Boolean used to make sure that fireMode only changes once every time the touch sensor is pressed
bool hasCycled = false;

//Boolean used to make sure that only one dart, or only three darts are fired for every pull of the trigger
//Depends upon which fireMode the blaster is in
bool hasFired = false;

//Method that fires a burst of darts of size "burst"
void fireBurst(int burst){
  for(int i = 0; i < burst; i++){
    digitalWrite(SOLENOID, HIGH);//Extends solenoid for 35 milliseconds to fire dart
    delay(32);//38 for 2s solenoiding,32 for 3s solenoiding
    digitalWrite(SOLENOID, LOW);//Retracts Solenoid for 75 milliseconds to let the next dart emerge from the magazine
    delay(75);
  }
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

void loop() {
  
  if(digitalRead(REVSWITCH)){ //Checks to see if the rev trigger is held 
    digitalWrite(FLYWHEELS, HIGH);//If the rev trigger is held activate the flywheels
    
    updateTriggerVals(analogRead(TRIGGER));
    bool triggerState = meanTriggerVal() > 950; //Checks whether the trigger has been pulled or not and assigns this to a boolean
    
    Serial.println(triggerState);
    if(fireMode == 0){//Checks if the blaster is currently in semi auto mode
      if(triggerState && !hasFired){//Checks if the trigger is currently pressed and the blaster has not been fired for this trigger pull
        /*Serial.print("fired ");
        Serial.print(analogRead(TRIGGER));
        Serial.print(" hasFired ");
        Serial.println(hasFired);
        Serial.println();*/
        fireBurst(1);//Fires a burst of 1 dart (a single shot)
        hasFired = true;//Sets the hasFired flag to true so the blaster fires only once

      }
      if(!triggerState){//Checks if the trigger is released 
        /*Serial.print("reset");
        Serial.println(analogRead(TRIGGER));*/
        hasFired = false;//Resets hasFired to false to the blaster can fire again
      }
    }
    if(fireMode == 1){//Checks if the blaster is currently in three shot burst mode
      if(triggerState && !hasFired){//Checks if the trigger is currently pressed and the blaster has not been fired for this trigger pull
        fireBurst(3);//Fires a burst of 3 darts
        hasFired = true;//Sets the hasFired flag to true so the blaster fires only one three shot burst
      }
      if(!triggerState){//Checks if the trigger is released 
        hasFired = false;//Resets hasFired to false to the blaster can fire again
      }
    }
    if(fireMode == 2){//Checks if the blaster is currently in full auto mode
      if(triggerState){//Checks to see if the trigger is held

        //Fires a single shot
        //No boolean to make sure trigger is released between shots
        fireBurst(1);
      }
    }
  }
  else{
    digitalWrite(FLYWHEELS, LOW);//If the rev trigger is not pressed don't spin up the flywheels
  }
  
  if(analogRead(A2)>1000 && !hasCycled){//Checks to see if the touch sensor is pressed and the blaster has not changed fire mode yet
    //Cycles the fireMode between 0,1, and 2 in increments of 1
    fireMode += 1;
    fireMode = fireMode%3;
    
    hasCycled = true;//Sets the hasCycled flag to true so the blaster will only cycle once for this activation of the touch sensor.
    //Serial.println(fireMode);//Print the fireMode to the serial monitor
  }
  else if(analogRead(A2)< 300){//Checks if the touch sensor is not being pressed
    hasCycled = false;//Sets the hasCycled flag to false to allow the blaster to cycle the fireMode again.
  }
}
