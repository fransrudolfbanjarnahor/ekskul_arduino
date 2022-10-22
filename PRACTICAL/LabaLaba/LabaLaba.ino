#include <Servo.h>
Servo s1;
Servo s2;
Servo s3;
Servo s4;

int pos = 0;    // variable to store the servo position
int DELAY = 1000;
void setup() {
  s1.attach(2);
  s2.attach(3);
  s3.attach(4);
  s4.attach(5);
  s1.write(0);
  s2.write(0);
  s3.write(0);
  s4.write(0);
  delay(DELAY);
  s1.write(90);
  s2.write(90);
  s3.write(90);
  s4.write(90);
//  delay(DELAY);
//  s1.write(180);
//  s2.write(180);
//  s3.write(180);
//  s4.write(180);
  
}

void loop() {
  // goes from 180 degrees to 0 degrees
    
//    delay(1000);
//    s11.write(0);
//    delay(1000);
//    s11.write(90);
//    delay(1000);
//    s11.write(0);
//    delay(1000);
//    s11.write(90);
  
}
