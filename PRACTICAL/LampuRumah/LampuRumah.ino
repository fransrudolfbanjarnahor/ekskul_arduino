#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "SoftwareSerial.h"
#include <LittleFS.h>
#include <SPI.h>

const char* ssid = "BUANA-SAWAH";      // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "qwerty54321";  // The password of the Wi-Fi network
ESP8266WebServer server(80);
#define R D0
#define Y D1
#define G D2
#define UTAMA D4
#define K1 D3

void setup() {

  pinMode(R, OUTPUT);
  pinMode(Y, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(UTAMA, OUTPUT);
  pinMode(K1, OUTPUT);
  digitalWrite(UTAMA, LOW);
  // digitalWrite(R, HIGH);
  // digitalWrite(Y, HIGH);
  // digitalWrite(G, HIGH);
  Serial.begin(115200);
  WiFi.begin(ssid, password);  // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println("...");
  LittleFS.begin();
  WiFi.begin(ssid, password);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {  // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i);
    Serial.print(' ');
  }
  Serial.println(WiFi.localIP());
  server.serveStatic("/axios.min.map", LittleFS, "/axios.min.map");
  server.serveStatic("/axios.min.js", LittleFS, "/axios.min.js");
  server.serveStatic("/", LittleFS, "/index.html");
  server.on("/control", HTTP_POST, control);
  server.enableCORS(true);
  server.onNotFound(handleNotFound);
  server.begin();

  delay(1000);  //Set volume value. From 0 to 30
  // myDFPlayer.play(1);     //Play the first mp3
}

void loop() {
  server.handleClient();
  delay(1000);
}


void control() {
  // digitalWrite(R, LOW);
  // digitalWrite(Y, LOW);
  // digitalWrite(G, LOW);
  // Serial.println(server.arg("plain"));
  if (String(server.arg("plain")) == "depan_h") {

    if (digitalRead(G) == HIGH) {
      Serial.println("LAMPU DEPAN HIJAU HIGH");
      digitalWrite(G, LOW);
    } else {
      Serial.println("LAMPU DEPAN HIJAU LOW");
      digitalWrite(G, HIGH);
    }
  }

  if (String(server.arg("plain")) == "depan_k") {

    if (digitalRead(Y) == HIGH) {
      digitalWrite(Y, LOW);
    } else {
      digitalWrite(Y, HIGH);
    }
  }

  if (String(server.arg("plain")) == "depan_m") {

    if (digitalRead(R) == HIGH) {
      digitalWrite(R, LOW);
    } else {
      digitalWrite(R, HIGH);
    }
  }

  if (String(server.arg("plain")) == "utama") {
    if (digitalRead(UTAMA) == HIGH) {
      Serial.println("UTAMA HIGH");
      digitalWrite(UTAMA, LOW);
    } else {
      Serial.println("UTAMA LOW");
      digitalWrite(UTAMA, HIGH);
    }
  }

  if (String(server.arg("plain")) == "k1") {
    if (digitalRead(K1) == HIGH) {
      Serial.println("UTAMA HIGH");
      digitalWrite(K1, LOW);
    } else {
      Serial.println("UTAMA LOW");
      digitalWrite(K1, HIGH);
    }
  }


  // if (String(server.arg("plain")) == "depan_k") {
  //   Serial.println("LAMPU DEPAN KUNING");
  //   if (Y == HIGH) {
  //     digitalWrite(Y, LOW);
  //   } else {
  //     digitalWrite(Y, HIGH);
  //   }
  // }
  // if (String(server.arg("plain")) == "depan_m") {
  //   Serial.println("LAMPU DEPAN MERAH");
  //   if (R == HIGH) {
  //     digitalWrite(R, LOW);
  //   } else {
  //     digitalWrite(R, HIGH);
  //   }
  // }
  server.send(200, "application/json", server.arg("plain"));
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
