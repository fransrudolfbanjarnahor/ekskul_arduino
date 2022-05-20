#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

void handleRoot();              // function prototypes for HTTP handlers
void handleLogin();
void handleNotFound();

const char* ssid     = "BUANA-SAWAH";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "qwerty54321";     // The password of the Wi-Fi network

const char form_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<body>

<h2>HTML Forms</h2>

<form action="/dapatkan_text" method="POST">
  <label for="text">Kirim Text:</label><br>
  <input type="text" id="text" name="text" value="Kirim Text">
  <input type="submit" value="Submit">
</form> 

</body>
</html>
)rawliteral";



void setup(void) {
  Serial.begin(115200);        
  delay(10);
  Serial.println('\n');

  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);        
  server.on("/dapatkan_text", HTTP_POST, handlePost); 
  server.onNotFound(handleNotFound);           

  server.begin();                            // Actually start the server
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();                     // Listen for HTTP requests from clients
}

void handleRoot() {
    server.send(200, "text/html", form_html);
}


void handlePost() {

  if (server.arg("text") != NULL) {
    Serial.println(server.arg("text")); 
    server.send(200, "text/html", "Text berhasil diterima " + server.arg("text") );
  } else {
    server.send(401, "text/plain", "Tidak berhasil mendapatkan text");
  }
}
void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}
