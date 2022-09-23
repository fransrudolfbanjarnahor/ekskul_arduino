#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

ESP8266WebServer server(80);  // Create a webserver object that listens for HTTP request on port 80

void handleRoot();  // function prototypes for HTTP handlers
void handleLogin();
void handleNotFound();

const char* ssid = "BUANA-SAWAH";      // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "qwerty54321";  // The password of the Wi-Fi network

const char form_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<body>

<h2>HTML Forms</h2>

<form action="/dapatkan_text" method="POST">
  <label for="text">Kirim Text:</label><br>
  <input type="text" id="text" name="text" value="Kirim Text">
  <input type="text" id="kecepatanscroll" name="kecepatanscroll" value="25">
  <input type="submit" value="Submit">
</form> 

</body>
</html>
)rawliteral";

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

uint8_t scrollSpeed = 50;  // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 2000;


#define BUF_SIZE 255
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "Hello! Enter new message?" };
bool newMessageAvailable = true;

String ip = "";
void setup(void) {
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

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

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  P.begin();
  //P.print(WiFi.localIP().toString());
  ip = WiFi.localIP().toString();
  P.print(ip.substring(ip.length() - 4, ip.length()));

  server.on("/", HTTP_GET, handleRoot);
  server.on("/dapatkan_text", HTTP_POST, handlePost);
  server.onNotFound(handleNotFound);

  server.begin();  // Actually start the server
  Serial.println("HTTP server started");
  delay(5000);
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
}

void loop(void) {
  server.handleClient();  // Listen for HTTP requests from clients
  if (P.displayAnimate()) {
    if (newMessageAvailable) {
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;
    }
    P.displayReset();
  }
}

void handleRoot() {
  server.send(200, "text/html", form_html);
}


void handlePost() {
  if (server.arg("text") != NULL) {
    Serial.println(server.arg("kecepatanscroll"));
    server.arg("text").toCharArray(newMessage, BUF_SIZE);
    scrollSpeed = server.arg("kecepatanscroll").toInt();
    P.displayText(newMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
    newMessageAvailable = true;

    server.send(200, "text/html", "Text berhasil diterima " + server.arg("text"));
  } else {
    server.send(401, "text/plain", "Tidak berhasil mendapatkan text");
  }
}
void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}