#include <Servo.h>


const int ServoPin = 10;
const int TriggerPin = 11;
const int EchoPin = 12;

Servo servo;
long duration, cm, inches;

void setup() {
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  Serial.begin(9600);
  servo.attach(ServoPin);
  servo.write(0);
}

void loop() {
  digitalWrite(TriggerPin, LOW);
  delayMicroseconds(5);
  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);

  pinMode(EchoPin, INPUT);
  duration = pulseIn(EchoPin, HIGH);
  cm = (duration / 2) / 29.1;   // Divide by 29.1 or multiply by 0.0343
//  inches = (duration / 2) / 74; // Divide by 74 or multiply by 0.0135
 
//  Serial.print(inches);
//  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  if (cm < 50) {
    servo.write(90);
    delay(2000);
  } else {
    servo.write(0);
    delay(2000);
  }
//  Serial.println();

  delay(250);
}
