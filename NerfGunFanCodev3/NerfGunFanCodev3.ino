 /* Sweep
 by BARRAGAN <http://barraganstudio.com> 
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/ 

#include <Servo.h> 

Servo fan;  // create servo object to control a servo 

long long prevTime = 0;
 
void setup() 
{ 
  Serial.begin(115200);
  fan.attach(9); //TODO: try out some different pins
  delay(2000);
  Serial.println("attached");
  fan.write(770); 
  delay(3000);
  Serial.println("FanInitComplete");
  pinMode(A0, INPUT);
} 
 
void loop() 
{
 int fanStatus = analogRead(A0);
 if(fanStatus >= 1010){
   fan.write(2100);
   Serial.print("FanON");
   Serial.println(fanStatus);
  }           
  else{
    fan.write(780); 
    Serial.print("FanOFF");
    Serial.println(fanStatus);
  }
  //Serial.println(fanStatus);
} 


