#define SOLENOID A0
#define FLYWHEELS A3

#define REVSWITCH A4
#define TRIGGER A5

#define CYCLEMODE A2

int fireMode = 0;
// 0: semiAuto, 1:3Burst, 2:Auto

void setup() {
  pinMode(SOLENOID,OUTPUT);
  pinMode(FLYWHEELS,OUTPUT);
  pinMode(REVSWITCH, INPUT_PULLUP);
  pinMode(TRIGGER, INPUT_PULLUP);
  digitalWrite(FLYWHEELS, LOW);
  Serial.begin(115200);

}

bool hasCycled = false;

bool hasFired = false;

void fireBurst(int burst){
  for(int i = 0; i < burst; i++){
    digitalWrite(SOLENOID, HIGH);
    delay(35);
    digitalWrite(SOLENOID, LOW);
    delay(75);
  }
}

void loop() {
  if(digitalRead(REVSWITCH)){
    digitalWrite(FLYWHEELS, HIGH);
    bool triggerState = analogRead(TRIGGER) > 1000;
    //Serial.println(analogRead(TRIGGER));
    if(fireMode == 0){
      if(triggerState && !hasFired){
        fireBurst(1);
        hasFired = true;
      }
      if(!triggerState){
        hasFired = false;
      }
    }
    if(fireMode == 1){
      if(triggerState && !hasFired){
        fireBurst(3);
        hasFired = true;
      }
      if(!triggerState){
        hasFired = false;
      }
    }
    if(fireMode == 2){
      if(triggerState){
        fireBurst(1);
      }
    }
  }
  else{
    digitalWrite(FLYWHEELS, LOW);
  }
  
  if(analogRead(A2)>1000 && !hasCycled){
    fireMode += 1;
    fireMode = fireMode%3;
    hasCycled = true;
    Serial.println(fireMode);
  }
  else if(analogRead(A2)< 300){
    hasCycled = false;
  }
}
