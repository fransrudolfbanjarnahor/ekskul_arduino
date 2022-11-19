/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill-in your Template ID (only if using Blynk.Cloud) */
//#define BLYNK_TEMPLATE_ID   "YourTemplateID"

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
#define BLYNK_TEMPLATE_ID "TMPLm-HoHS5y"
#define BLYNK_DEVICE_NAME "ESP8266BlynkScrollingText"
#define BLYNK_AUTH_TOKEN "cm7WHsOwUQrUPCpGashqGeLiTIEu5LMk"
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).

#define LED D8

#define CLK_PIN1 D3  // or SCK
#define CS_PIN1 D2
#define DATA_PIN1 D1  // or MOSI

// or MOSI
#define MAX_DEVICES 4
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW  //edit this as per your LED matrix hardware type
// MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

//MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN1, CLK_PIN1, CS_PIN1, MAX_DEVICES);

uint8_t scrollSpeed = 25;  // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 2000;

#define BUF_SIZE 255
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "Hello! Enter new message?" };
bool newMessageAvailable = true;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "BUANA-SAWAH";
char pass[] = "qwerty54321";

BLYNK_WRITE(V0) {
  String message = param.asString();
  message.toCharArray(newMessage, BUF_SIZE);
  newMessageAvailable = true;
  //P.print(pinValue);
  // digitalWrite(LED, pinValue);
}

BLYNK_WRITE(V1) {
  scrollSpeed = param.asInt();
  Serial.println(scrollSpeed);
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
  //P.print(pinValue);
  // digitalWrite(LED, pinValue);
}

BLYNK_WRITE(V2) {
  int lr = param.asInt();
  Serial.println(lr);
  if (lr==0) {
    scrollEffect = PA_SCROLL_RIGHT;
    scrollAlign = PA_RIGHT;
  }
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
  //P.print(pinValue);
  // digitalWrite(LED, pinValue);
}
void setup() {
  // Debug console
  Serial.begin(9600);
  P.begin();
  P.print("HELLO");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
}

void loop() {
  Blynk.run();
  if (P.displayAnimate()) {
    if (newMessageAvailable) {
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;
    }
    P.displayReset();
  }
}
