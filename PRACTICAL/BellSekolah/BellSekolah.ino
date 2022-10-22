#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <SPI.h>
//#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// #if !defined(UBRR1H)
// #include <SoftwareSerial.h>
// SoftwareSerial mySerial(D7, D8);  // RX, TX
// #endif
// RX, TX
SoftwareSerial mySerial(D7, D8);  // RX, TX

const char* ssid = "ARDUINO-AP";      // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "987654321";  // The password of the Wi-Fi network

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
#define PIN_BUSY D1
const long utcOffsetInSeconds = 25200;
char daysOfTheWeek[7][12] = { "MINGGU", "SENIN", "SELASA", "RABU", "KAMIS", "JUMAT", "SABTU" };
#define NTP_OFFSET 60 * 60      // In seconds
#define NTP_INTERVAL 60 * 1000  // In miliseconds
// Define NTP Client to get time
bool dfPlayerStatus = false;
ESP8266WebServer server(80);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");


int tempMinute = 0;
void setup() {
  pinMode(PIN_BUSY, INPUT);
  //PinMode(D1, OUTPUT)
  mySerial.begin(9600);
  Serial.begin(115200);
  LittleFS.begin();
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

  server.serveStatic("/bootstrap.min.css.map", LittleFS, "/bootstrap.min.css.map");
  server.serveStatic("/bootstrap.min.css", LittleFS, "/bootstrap.min.css");
  server.serveStatic("/jquery-3.3.1.slim.min.js", LittleFS, "/jquery-3.3.1.slim.min.js");
  server.serveStatic("/bootstrap.min.js.map", LittleFS, "/bootstrap.min.js.map");
  server.serveStatic("/bootstrap.min.js", LittleFS, "/bootstrap.min.js");
  server.serveStatic("/axios.min.map", LittleFS, "/axios.min.map");
  server.serveStatic("/axios.min.js", LittleFS, "/axios.min.js");
  server.serveStatic("/", LittleFS, "/index.html");

  server.on("/info", HTTP_GET, getInfo);
  server.on("/jadwal", HTTP_GET, getJadwal);
  server.on("/play", HTTP_POST, play);
  server.on("/add", HTTP_POST, updateJadwal);
  server.enableCORS(true);
  server.onNotFound(handleNotFound);
  server.begin();

  timeClient.begin();
  timeClient.setTimeOffset(25200);
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  delay(1000);
  if (!myDFPlayer.begin(mySerial, true, true)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    dfPlayerStatus = false;
    while (true) {
      delay(0);  // Code to compatible with ESP8266 watch dog.
    }
  } else {
    dfPlayerStatus = true;
  }
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  Serial.println(F("DFPlayer Mini online."));
  Serial.println(myDFPlayer.readFolderCounts());
  Serial.println(myDFPlayer.readFileCounts());  //read all file counts in SD card
  Serial.println(WiFi.localIP());
  myDFPlayer.volume(10);
  //readJadwal();
  delay(1000);  //Set volume value. From 0 to 30
  // myDFPlayer.play(1);     //Play the first mp3
}

int ctr = 0;
int isPlay = 0;
void loop() {
  server.handleClient();
  timeClient.update();
  isPlay = digitalRead(PIN_BUSY);
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());

  if (tempMinute != timeClient.getMinutes()) {
    tempMinute = timeClient.getMinutes();
    cekWaktu();
  } else {
    tempMinute = timeClient.getMinutes();
  }
  Serial.print(tempMinute);
  // //Serial.println(timeClient.getFormattedTime());
  // if (timeClient.getMinutes() % 2 == 0 && isPlay) {
  //   myDFPlayer.play(1);
  // }
  // if (timeClient.getMinutes() % 2 == 1 && isPlay) {
  //   myDFPlayer.play(2);
  // }
  // if (timeClient.getSeconds() == 0) {
  //   ctr++;
  //   if (ctr % 2 == 0) {
  //     myDFPlayer.play(1);
  //   } else {
  //     myDFPlayer.play(2);
  //   }
  // }
  // if (myDFPlayer.available()) {
  //   printDetail(myDFPlayer.readType(), myDFPlayer.read());  //Print the detail message from DFPlayer to handle different errors and states.
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

void getInfo() {
  DynamicJsonDocument doc(256);
  String buf;
  doc["info"]["ip"] = WiFi.localIP().toString();
  doc["info"]["dpplayer"] = (dfPlayerStatus ? "DF Player Connected" : "DF Player Failed");
  doc["info"]["file_count"] = myDFPlayer.readFileCounts();
  serializeJson(doc, buf);
  server.send(200, "application/json", buf);
}

// void readJadwal() {
//   File file = LittleFS.open("/jadwal.json", "r");
//   String buf;
//   if (!file) {
//     Serial.println("Jadwal  KOSONG");
//   } else {
//     DynamicJsonDocument jadwal(2048);
//     Serial.println("EXISTING JADWAL");
//     deserializeJson(jadwal, file);
//     serializeJson(jadwal, buf);

//     // arrayJadwal = jadwal.as<JsonArray>();
//     // for (JsonVariant v : arrayJadwal) {
//     //   JsonObject jadwal = v.as<JsonObject>();
//     //   Serial.println(String(jadwal["hari"]));
//     // }
//   }
//   file.close();
// }

void cekWaktu() {

  File file = LittleFS.open("/jadwal.json", "r");
  String buf;
  if (!file) {
    Serial.println("Jadwal  KOSONG");
  } else {
    DynamicJsonDocument jadwal(1024);
    deserializeJson(jadwal, file);
    JsonArray arrayJadwal = jadwal.as<JsonArray>();
    for (JsonVariant v : arrayJadwal) {
      JsonObject obj = v.as<JsonObject>();
      Serial.println(obj["hari"].as<String>());
      Serial.println(obj["jam"].as<String>());
      Serial.println(obj["menit"].as<String>());

      if (obj["hari"] == daysOfTheWeek[timeClient.getDay()] && obj["jam"].as<int8_t>() == timeClient.getHours() && obj["menit"].as<int8_t>() == timeClient.getMinutes()) {
        if (isPlay) {
          myDFPlayer.play(obj["audio"].as<int8_t>());
        }
        Serial.println("HARI SAMA JAM SAMA MENIS SAMA");
      }
    }
    serializeJson(jadwal, buf);
  }
  file.close();

  // Serial.println(buf);
}

void getJadwal() {
  File file = LittleFS.open("/jadwal.json", "r");
  String buf;
  if (!file) {
    Serial.println("Jadwal  KOSONG");
    File fileW = LittleFS.open("/jadwal.json", "w");
    DynamicJsonDocument doc(1024);
    doc["data"] = "[]";
    serializeJson(doc, buf);
    Serial.println(buf);
    int byteWrite = fileW.print(buf);
    if (byteWrite > 0) {
      Serial.println("Jadwal Tersimpan");
    } else {
      Serial.println("Jadwal Tidak Tersimpan");
    }
    fileW.close();
    server.send(200, "application/json", buf);
  } else {
    Serial.println("EXISTING JADWAL");
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, file);
    delay(1000);
    serializeJson(doc, buf);
    Serial.println(buf);
  }
  file.close();
  server.send(200, "application/json", buf);
}

void play() {
  Serial.println(server.arg("plain"));
  DynamicJsonDocument doc(256);
  deserializeJson(doc, server.arg("plain"));
  Serial.println(String(doc["index"]));
  myDFPlayer.play(doc["index"]);
  String buf;
  server.send(200, "application/json", buf);
}

void updateJadwal() {
  //Serial.println(server.arg("plain"));
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, server.arg("plain"));
  String buf;
  File file = LittleFS.open("/jadwal.json", "w");
  int byteWrite = file.print(server.arg("plain"));
  if (byteWrite > 0) {
    Serial.println("Jadwal Tersimpan");
  } else {
    Serial.println("Jadwal Tidak Tersimpan");
  }
  serializeJson(doc, buf);
  file.close();
  Serial.println(buf);
  server.send(200, "application/json", buf);
}

void handleNotFound() {
  if (server.method() == HTTP_OPTIONS) {
    server.sendHeader("Access-Control-Max-Age", "10000");
    server.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "*");
    server.send(204);
  } else {
    server.send(404, "text/plain", "");
  }
}
