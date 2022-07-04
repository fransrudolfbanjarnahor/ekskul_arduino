#include <IRremote.hpp>
typedef struct KeyAndPin {
  int key;
  int pin;
};

const int IR_RECEIVE_PIN = 12;
KeyAndPin butArray[] = {
  {128, 2},
  {64, 3},
  {192, 4},
  {32, 5},
  {160, 6},
  {96, 7},
  {224, 8},
};

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
  
  for (int i = 2; i <= 8; i++) {
    pinMode(i, OUTPUT);
  }

  for (int i = 2; i <= 8; i++) {
    digitalWrite(i, HIGH);
    delay(250);
  }
  delay(2000);
  for (int i = 2; i <= 8; i++) {
    digitalWrite(i, LOW);
  }

}

void loop() {
  if (IrReceiver.decode()) {
    Serial.println(sizeof(butArray));
    Serial.println(sizeof(butArray[0]));
    for (int i = 0; i <= sizeof(butArray) / sizeof(butArray[0]); i++) {
      digitalWrite(butArray[i].pin, LOW);
      if (IrReceiver.decodedIRData.command == butArray[i].key) {
        Serial.println(butArray[i].pin);
        digitalWrite(butArray[i].pin, HIGH);
      }
    }
    delay(1000);
    IrReceiver.resume();
  }

}
