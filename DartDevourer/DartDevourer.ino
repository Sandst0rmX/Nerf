#include <FastLED.h>

//feeder Mechanism
#define feederStart 11
#define feederStop 4
#define feeder 3
#define hammerStop 2
#define hammer 5

//pusher Mechanism
#define pusherStop 6
#define pusher 7
#define trigSwitch 8

//flywheel mechanism
#define revSwitch 9
#define flywheels 10  

//Touch Sensor Pin
#define CYCLEMODE A5

//LED Strip
#define LED_PIN 12
#define NUM_LEDS 3
CRGB leds[NUM_LEDS];

int fireMode = 0;
// 0: semiAuto, 1:3Burst, 2:Auto

void setup() {
  //Pusher recieves power when you send a LOW signal. Initializes pusher to HIGH.
  pinMode(pusher, OUTPUT);
  digitalWrite(pusher, HIGH);
  delay(400);
  //Hammer recieves power when you send a HIGH signal. Initializes hammer to LOW.
  pinMode(hammer,OUTPUT);
  digitalWrite(hammer,LOW);
  
  Serial.begin(115200);

  //Initializes all the switches
  pinMode(feederStart, INPUT_PULLUP);
  pinMode(feederStop, INPUT_PULLUP);
  pinMode(hammerStop, INPUT_PULLUP);
  pinMode(pusherStop,INPUT_PULLUP);
  pinMode(trigSwitch, INPUT_PULLUP);
  pinMode(revSwitch, INPUT_PULLUP);

  //Initializes LED strip
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  //Initializes flywheels and feeder. Both recieve power when a HIGH signal is sent.
  pinMode(flywheels, OUTPUT);
  digitalWrite(flywheels,LOW);
  pinMode(feeder,OUTPUT);
  digitalWrite(feeder,LOW);

  //Sets all LED colors to the current fire mode (fire mode is 0 at the start so 1 row of LED's will light up)
  for(int i = 0; i < fireMode+1; i++){
    leds[i] = CRGB(0, 100, 0);
  }
  FastLED.show();

  Serial.println("Init Complete");
  
}


//Called when a dart needs to be loaded
void loadDart(){

  //Checks to see if hammer is still extended before loading a dart
  bool hStop = !digitalRead(hammerStop);
  if(!hStop){
    Serial.println("HAMMER HAS NOT RETURNED TO STARTING POSITION. THERE IS A JAM.");
    return;
  }

  //Checks to see if pusher is still extended before loading a dart
  bool pusherExtended = digitalRead(pusherStop);
  if(pusherExtended){
    Serial.println("THE PUSHER IS EXTENDED. LOADING IS DISABLED UNTIL PUSHER RETRACTS.");
    return;
  }

  long long feedTimerStart = millis();
  bool shouldHammer = false;
  
  while(!digitalRead(feederStart)){
    long long feedTimerCurTime = millis();
    digitalWrite(feeder,HIGH);

    bool fStop = !digitalRead(feederStop);
    if(fStop){
      shouldHammer = true;
      break;
    }
  }

  for(int i = 0; i < 300; i++){
    bool fStop = !digitalRead(feederStop);
    if(fStop){
      shouldHammer = true;
      break;
    }
    delay(1);
  }

  
  digitalWrite(feeder,LOW);

  if(shouldHammer){
      Serial.println("starting the hammer");
      digitalWrite(hammer,HIGH);
      delay(90);//90
      long long hammerTimerStartMs = millis();
      while(digitalRead(hammerStop)){//Returns 0 when the hammer should stop
          digitalWrite(hammer,HIGH);
          long long hammerTimerCurTimeMs = millis();
          if(hammerTimerCurTimeMs - hammerTimerStartMs >= 90){//the hammer spends a maxmimum of 73 millseconds pushing darts. If it spends more time than this it is highly likely that there is a jam.
            break;
          }
      }
      
      digitalWrite(hammer,LOW);
      Serial.println((long)millis()-(long)hammerTimerStartMs);
      Serial.println("ending the hammer");
  }

}

//Boolean used to make sure that fireMode only changes once every time the touch sensor is pressed
bool hasCycled = false;

//Boolean used to make sure that only one dart, or only three darts are fired for every pull of the trigger
//Depends upon which fireMode the blaster is in
bool hasFired = false;

//Called to fire a single dart in semi-auto mode
void fireDart(){

  //Checks to see if hammer is still extended before loading a dart
  bool hStop = !digitalRead(hammerStop);
  if(!hStop){
    Serial.println("HAMMER HAS NOT RETURNED TO STARTING POSITION. THERE IS A JAM.");
    return;
  }
  
  long long fireTimerStartMs = millis();
  digitalWrite(pusher, LOW);
  delay(83);
  while(digitalRead(pusherStop)){//Goes LOW when pusherStop is pressed
    long long curTimeMs = millis();
    if(curTimeMs - fireTimerStartMs > 100){
      digitalWrite(pusher, HIGH);
      return;
    }
    digitalWrite(pusher, LOW);
  }
  digitalWrite(pusher, HIGH);
  Serial.println((long)millis()-(long)fireTimerStartMs);
}

//Called to fire three darts in three burst mode
void threeBurst(){
  bool hStop = !digitalRead(hammerStop);
  if(!hStop){
    Serial.println("HAMMER HAS NOT RETURNED TO STARTING POSITION. THERE IS A JAM.");
    return;
  }

  for(int i = 0; i < 3; i++){
    long long fireTimerStartMs = millis();
    digitalWrite(pusher, LOW);
    delay(83);
    
    while(digitalRead(pusherStop)){//Goes LOW when pusherStop is pressed
      long long curTimeMs = millis();
      if(curTimeMs - fireTimerStartMs > 100){
        digitalWrite(pusher, HIGH);
        return;
      }
      digitalWrite(pusher, LOW);
    }
    Serial.println((long)millis()-(long)fireTimerStartMs);
  }
  digitalWrite(pusher, HIGH);
  
}

//Unjam mode variables
long unJamTimerMs = 0;

bool countingUnJam = false;

bool unJamMode = false;

bool unJamHasTriggered = false;

bool unJamHasFStarted = false;


long prevPrintTimerMs = 0;


long prevLoopTimeMs = 0;

void loop() {
  long curLoopTimeMs = millis();
  //Sets loop time to a constant 15ms
  if(curLoopTimeMs - prevLoopTimeMs > 15){
    prevLoopTimeMs = curLoopTimeMs;
    bool rev = !digitalRead(revSwitch);
    bool trig = !digitalRead(trigSwitch);
    bool pStop = !digitalRead(pusherStop);
    bool fStop = !digitalRead(feederStop);
    bool hStop = !digitalRead(hammerStop);
    bool fStart = !digitalRead(feederStart);
    bool tSensor = analogRead(CYCLEMODE) > 950;
  
    long curPrintTimerMs = millis();

    //Serial prints button info every 1 second
    if(curPrintTimerMs - prevPrintTimerMs > 1000){
      Serial.print("rev: ");
      Serial.print(rev);
      Serial.print(", trig: ");
      Serial.print(trig);
      Serial.print(", pStop: ");
      Serial.print(pStop);
      Serial.print(", fStop: ");
      Serial.print(fStop);
      Serial.print(", hStop: ");
      Serial.print(hStop);
      Serial.print(", fStart: ");
      Serial.println(fStart);
      prevPrintTimerMs = curPrintTimerMs;
    }
    //Checks to see unJam mode has been entered
    if(unJamMode){
      digitalWrite(flywheels, LOW);//Shuts off flywheels
      if(tSensor){
        if(trig && !unJamHasTriggered){
          Serial.println("un-jamming pusher");
          digitalWrite(pusher, LOW);
          delay(30);
          digitalWrite(pusher, HIGH);
          unJamHasTriggered = true;
        } else if(!trig){
          unJamHasTriggered = false;
        }
        if(fStart && !unJamHasFStarted){
          Serial.println("un-jamming hammer");
          digitalWrite(hammer,HIGH);
          delay(30);
          digitalWrite(hammer,LOW);
          unJamHasFStarted = true;
        } else if(!fStart){
          unJamHasFStarted = false;
        }
      } else{
          unJamMode = false;
    
          unJamHasTriggered = false;
    
          unJamHasFStarted = false;
          for(int i = 0; i < 3; i++){
            leds[i] = CRGB(0, 0, 0);
          }
          FastLED.show();
          for(int i = 0; i < fireMode+1; i++){
            leds[i] = CRGB(0, 100, 0);
          }
          FastLED.show();
      }
    } else{
      //If not in unjam mode check to see if rev is pressed.
        if(rev){
            digitalWrite(flywheels, HIGH);
            if(fireMode == 0){
              if(trig && !hasFired){
                  hasFired = true;
                  fireDart();
                  Serial.println("fired");
              } else if(!trig){
                  hasFired = false;
              }
            } else if(fireMode == 1){
              if(trig && !hasFired){
                  hasFired = true;
                  threeBurst();
              } else if(!trig){
                  hasFired = false;
              }
            } else if(fireMode == 2){ //Full-auto mode. Applies power to the pusher until trigger is released. Then continues applying power until pusher trips retraction switch.
                if(!hStop){
                  Serial.println("HAMMER HAS NOT RETURNED TO STARTING POSITION. THERE IS A JAM.");
                  return;
                } else if(trig){
                  digitalWrite(pusher, LOW);
                } else if(!trig){
                  if(pStop){
                    digitalWrite(pusher, HIGH);
                  } else {
                    
                  }
              }
            }
        } else{
          digitalWrite(pusher, HIGH);
          digitalWrite(flywheels, LOW);
        }
        //If the feederswitch is triggered call the loadDart() function.
        if(fStart){
          loadDart();
        }
      
        //Mode Cycler
        if(tSensor && !hasCycled){//Checks to see if the touch sensor is pressed and the blaster has not changed fire mode yet
          //Cycles the fireMode between 0,1, and 2 in increments of 1
          fireMode += 1;
          fireMode = fireMode%3;

          //Resets all LEDs
          for(int i = 0; i < 3; i++){
            leds[i] = CRGB(0, 0, 0);
          }
          //Lights up LEDS according to fire mode
          for(int i = 0; i < fireMode+1; i++){
            leds[i] = CRGB(0, 100, 0);
          }
          FastLED.show();
          
          hasCycled = true;//Sets the hasCycled flag to true so the blaster will only cycle once for this activation of the touch sensor.
          //Serial.println(fireMode);//Print the fireMode to the serial monitor
        } else if(analogRead(CYCLEMODE)< 300){//Checks if the touch sensor is not being pressed
          hasCycled = false;//Sets the hasCycled flag to false to allow the blaster to cycle the fireMode again.
        }
      
        //Enters un-jam mode
        if(tSensor && !countingUnJam){
          unJamTimerMs = millis();
          countingUnJam = true;
        } else if(tSensor && countingUnJam){
          long curTimeMs = millis();
          if(curTimeMs - unJamTimerMs > 6000){
            unJamMode = true;
            for(int i = 0; i < 3; i++){
              leds[i] = CRGB(100, 0, 0);
            }
            FastLED.show();
          }
        }
        else if(!tSensor && countingUnJam){
          countingUnJam = false;
        }
        
        delay(10);
    }
  }
}
