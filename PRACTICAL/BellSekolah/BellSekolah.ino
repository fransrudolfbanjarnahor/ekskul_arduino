#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <WiFiUdp.h>
#include <NTPClient.h>

// #if !defined(UBRR1H)
// #include <SoftwareSerial.h>
// SoftwareSerial mySerial(D7, D8);  // RX, TX
// #endif
// RX, TX
SoftwareSerial mySerial(D7, D8);  // RX, TX

const char* ssid = "ARDUINO_AP";      // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "987654321";  // The password of the Wi-Fi network

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
#define PIN_BUSY D1
const long utcOffsetInSeconds = 25200;
char daysOfTheWeek[7][12] = { "Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu" };
#define NTP_OFFSET 60 * 60      // In seconds
#define NTP_INTERVAL 60 * 1000  // In miliseconds
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
void setup() {
  // pinMode(PIN_BUSY, INPUT);
  mySerial.begin(9600);
  Serial.begin(115200);

  WiFi.begin(ssid, password);  // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {  // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i);
    Serial.print(' ');
  }
  timeClient.begin();


  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  delay(1000);
  if (!myDFPlayer.begin(mySerial, true, true)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0);  // Code to compatible with ESP8266 watch dog.
    }
  }

  // #if !defined(UBRR1H)
  //   mySerial.begin(9600);
  //   // myDFPlayer.begin(mySerial, true);
  //   if (!myDFPlayer.begin(mySerial, true)) {  //Use softwareSerial to communicate with mp3.
  //     Serial.println(F("Unable to begin:"));
  //     Serial.println(F("1.Please recheck the connection!"));
  //     Serial.println(F("2.Please insert the SD card!"));
  //     while (true) {
  //       delay(0);  // Code to compatible with ESP8266 watch dog.
  //     }
  //   }
  // #else
  //   Serial1.begin(9600);
  //   // myDFPlayer.begin(Serial1, true);
  //    if (!myDFPlayer.begin(Serial1, true)) {  //Use softwareSerial to communicate with mp3.
  //     Serial.println(F("Unable to begin:"));
  //     Serial.println(F("1.Please recheck the connection!"));
  //     Serial.println(F("2.Please insert the SD card!"));
  //     while (true) {
  //       delay(0);  // Code to compatible with ESP8266 watch dog.
  //     }
  //   }
  // #endif
  //   delay(1000);

  // myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(30);  //Set volume value. From 0 to 30
  myDFPlayer.play(1);     //Play the first mp3
}

int ctr = 0;
void loop() {

  timeClient.update();

  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  //Serial.println(timeClient.getFormattedTime());
  if (timeClient.getMinutes() % 2l == 0) {
     myDFPlayer.play(1);
  } else {
     myDFPlayer.play(2);
  }
  // if (timeClient.getSeconds() == 0) {
  //   ctr++;
  //   if (ctr % 2 == 0) {
  //     myDFPlayer.play(1);
  //   } else {
  //     myDFPlayer.play(2);
  //   }
  // }
  delay(1000);
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}