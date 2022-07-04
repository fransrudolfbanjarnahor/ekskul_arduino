#include <IRremote.hpp>

const int IR_RECEIVE_PIN = 12;

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
}

void loop() {
  if (IrReceiver.decode()) {
//    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    Serial.println(IrReceiver.decodedIRData.command);
    delay(1000);
    IrReceiver.resume();
  }
}
