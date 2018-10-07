#include <Servo.h> 

#define solenoidPin 9

#define fanPin 11

#define flywheelPin 5

Servo fan;
Servo flywheels;

long long prevTime = 0;
 
void setup() 
{ 
  Serial.begin(115200);
  fan.attach(fanPin);
  flywheels.attach(flywheelPin);
  delay(2000);
  Serial.println("attached");
  fan.write(770);
  flywheels.write(770);
  delay(3000);
  Serial.println("FanInitComplete");
  pinMode(solenoidPin, OUTPUT);
} 
 
void loop() 
{
   //fan.write(1335);
   //flywheels.write(1300);
   int revver = analogRead(A0);
   int trigger = analogRead(A1);

   Serial.print("trigger: ");
   Serial.println(trigger);
   Serial.print("revver: ");
   Serial.println(revver);
   
   if(revver >= 1000){
    fan.write(1500);
    flywheels.write(1900);
    
    if(trigger >= 1000){
      digitalWrite(solenoidPin, HIGH);
    } else {
      digitalWrite(solenoidPin, LOW);
    }
   } else {
    fan.write(780);
    flywheels.write(780);
    digitalWrite(solenoidPin, LOW);
   }
} 


