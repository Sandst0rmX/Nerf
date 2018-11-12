 /* Sweep
 by BARRAGAN <http://barraganstudio.com> 
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/ 

#define revSwitch 8
#define flywheels 6

#include <Servo.h> 

Servo fan;  // create servo object to control a servo 
 
void setup() 
{ 
  Serial.begin(115200);
  fan.attach(9); //TODO: try out some different pins
  delay(2000);
  Serial.println("attached");
  fan.write(770); 
  delay(3000);
  Serial.println("FanInitComplete");
  pinMode(revSwitch, INPUT_PULLUP);
  pinMode(flywheels, OUTPUT);
} 
 
void loop() 
{
  bool revving = !digitalRead(revSwitch);
 if(revving){
   fan.write(2100);
   digitalWrite(flywheels,HIGH);
   Serial.print("FanON");
  }           
  else{
    fan.write(780); 
    digitalWrite(flywheels,LOW);
    Serial.print("FanOFF");
  }
} 
