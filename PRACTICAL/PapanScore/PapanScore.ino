#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
//#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CLK_PIN1   D3 // or SCK
#define DATA_PIN1  D1 // or MOSI
#define CS_PIN1   D2 // or SS

#define CLK_PIN2   D5 // or SCK
#define DATA_PIN2  D7 // or MOSI
#define CS_PIN2    D8 // or SS

MD_Parola P1 = MD_Parola(HARDWARE_TYPE, DATA_PIN1, CLK_PIN1, CS_PIN1, MAX_DEVICES);
MD_Parola P2 = MD_Parola(HARDWARE_TYPE, DATA_PIN2, CLK_PIN2, CS_PIN2, MAX_DEVICES);

const char* ssid     = "BUANA-SAWAH";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "qwerty54321";     // The password of the Wi-Fi network
ESP8266WebServer server(80);

void setup(void)
{
  Serial.begin(115200);
  //  WiFi.mode(WIFI_STA);// Connect to the network
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");
  P1.begin();
  P2.begin();
  P1.print("OK CEK");
  P2.print("OK CEK");
  delay(5000);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }
  String ip = WiFi.localIP().toString();
  P1.print(ip.substring(ip.length()-4, ip.length()));
  
  Serial.println(WiFi.localIP());
  bool success = LittleFS.begin();

  server.serveStatic("/bootstrap.min.css.map", LittleFS, "/bootstrap.min.css.map");
  server.serveStatic("/bootstrap.min.css", LittleFS, "/bootstrap.min.css");
  server.serveStatic("/jquery-3.3.1.slim.min.js", LittleFS, "/jquery-3.3.1.slim.min.js");
  server.serveStatic("/bootstrap.min.js.map", LittleFS, "/bootstrap.min.js.map");
  server.serveStatic("/bootstrap.min.js", LittleFS, "/bootstrap.min.js");
  server.serveStatic("/axios.min.map", LittleFS, "/axios.min.map");
  server.serveStatic("/axios.min.js", LittleFS, "/axios.min.js");
  server.serveStatic("/", LittleFS, "/index.html");
  server.on("/updatescore", HTTP_POST, updateScore);
  server.on("/resetscore", HTTP_POST, resetScore);
  server.on("/get", HTTP_GET, getOldScore);
  server.enableCORS(true);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println(success);
  //  delay(1000);
  //  resetScore();
  delay(1000);
  //getOldScore();
  //resetScore();
}

String twoDigit(int point) {
  return point < 10 ? "0" + String(point) : String(point);
}

void updateScore() {
  Serial.println(server.arg("plain"));
  DynamicJsonDocument doc(256);
  deserializeJson(doc, server.arg("plain"));
  String buf;
  File file = LittleFS.open("/data.json", "w");
  int byteWrite = file.print(server.arg("plain"));
  if (byteWrite > 0) {
    showScore(doc);
    Serial.println("Score Write");
  }

  serializeJson(doc, buf);
  file.close();
  Serial.println(buf);
  server.send(200, "application/json", buf);
}

void showScore(JsonDocument& doc) {
  String p1 = "";
  String p2 = "";
  if (doc["t1"]["serve"] == true)
  {
    p1 = " >" + String(doc["t1"]["name"]) + ": " + twoDigit(doc["t1"]["point"]);
  } else {
    p1 = " -" + String(doc["t1"]["name"])  + ": " + twoDigit(doc["t1"]["point"]);
  }

  if (doc["t2"]["serve"] == true)
  {
    p2 = " >" + String(doc["t2"]["name"])  + ": " + twoDigit(doc["t2"]["point"]);
  } else {
    p2 = " -" + String(doc["t2"]["name"]) + ": " + twoDigit(doc["t2"]["point"]);
  }

  P1.print(p1);
  P2.print(p2);
}
void getOldScore() {
  File file = LittleFS.open("/data.json", "r");
  String buf;
  if (!file) {
    File fileW = LittleFS.open("/data.json", "w");
    DynamicJsonDocument doc(256);
    doc["t1"]["name"] = "-";
    doc["t1"]["serve"] = false;
    doc["t1"]["point"] = 0;
    doc["t2"]["name"] = "-";
    doc["t2"]["serve"] = false;
    doc["t2"]["point"] = 0;
    serializeJson(doc, buf);
    Serial.println(buf);
    int byteWrite = fileW.print(buf);
    if (byteWrite > 0) {
      Serial.println("Score Write");
      showScore(doc);
    }
    fileW.close();
    server.send(200, "application/json", buf);
  } else {
    Serial.println("SCORE EXISTING");
    DynamicJsonDocument doc(256);
    deserializeJson(doc, file);
    delay(1000);
    serializeJson(doc, buf);
    Serial.println(buf);
    showScore(doc);
  }
  file.close();
  server.send(200, "application/json", buf);
}
void resetScore() {
  Serial.println(server.arg("plain"));
  DynamicJsonDocument doc(256);
  deserializeJson(doc, server.arg("plain"));
  File file = LittleFS.open("/data.json", "w");
  int byteWrite = file.print(server.arg("plain"));
  if (byteWrite > 0) {
    Serial.println("Score Write");
    showScore(doc);
  }
  file.close();
  server.send(200, "application/json", server.arg("plain"));
}

void handleNotFound()
{
  if (server.method() == HTTP_OPTIONS)
  {
    server.sendHeader("Access-Control-Max-Age", "10000");
    server.sendHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "*");
    server.send(204);
  }
  else
  {
    server.send(404, "text/plain", "");
  }
}
void loop(void)
{
  server.handleClient();
}
