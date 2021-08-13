#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "FS.h"
#include "SPIFFS.h"
#include "n13-48.h"
#include "SPIFFSServer.h"

#define PIN_L0 27 // output pins per channel
#define PIN_L1 26
#define PIN_L2 25
#define PIN_C0 33 // pins for encoder
#define PIN_C1 32
#define PIN_CB 14
#define PIN_LED_ON 16 // status LED yellow
#define PIN_LED_READY 4 // status LED green

const char* www_realm = "Custom Auth Realm";
String authFailResponse = "Authentication Failed";
const char* www_username = "admin";
const char* www_password = "esp32";

char PAGE_UPLOAD[] PROGMEM = R"(
<form method="post" enctype="multipart/form-data">
    <input type="file" name="name">
    <input class="button" type="submit" value="Upload">
</form>
)";

WebServer server(80);

int l0 = 0; // duty cycles by channel
int l1 = 0;
int l2 = 0;
int value = 0; // value of encoder
int channel = 0; // channel to control with encoder
int lastState = 3; // encoder last state
int counter = 0; // interrupt counter


void handleData() { // receive GET request with new values per channel
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i) == "L0") {
      l0 = server.arg(i).toInt();
      ledcWrite(0, l0);
      Serial.printf("Setting L0 to %d\n", l0);
    }
    if(server.argName(i) == "L1") {
      l1 = server.arg(i).toInt();
      ledcWrite(1, l1);
      Serial.printf("Setting L1 to %d\n", l1);
    }
    if(server.argName(i) == "L2") {
      l2 = server.arg(i).toInt();
      ledcWrite(2, l2);
      Serial.printf("Setting L2 to %d\n", l2);
    }
  }
  char message[32]; 
  sprintf(message, "L0=%d\nL1=%d\nL2=%d\n", l0, l1, l2);
  server.send(200, "text/plain", message);
}

void setup(void) {
  pinMode(PIN_LED_ON, OUTPUT);
  digitalWrite(PIN_LED_ON, HIGH); // switch on status LEDs
  pinMode(PIN_LED_READY, OUTPUT);

  Serial.begin(115200);

  pinMode(PIN_L0, OUTPUT); // one pin per channel
  ledcSetup(0, 8000, 8);
  ledcAttachPin(PIN_L0, 0);
  ledcWrite(0, l0);
  pinMode(PIN_L1, OUTPUT);
  ledcSetup(1, 8000, 8);
  ledcAttachPin(PIN_L1, 1);
  ledcWrite(1, l1);
  pinMode(PIN_L2, OUTPUT);
  ledcSetup(2, 8000, 8);
  ledcAttachPin(PIN_L2, 2);
  ledcWrite(2, l2);

  pinMode(PIN_C0, INPUT_PULLUP); // pins for encoder
  pinMode(PIN_C1, INPUT_PULLUP);
  pinMode(PIN_CB, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIN_C0), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_C1), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_CB), handleButton, FALLING); // on press

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass); // comes from n13-48.h
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(PIN_LED_READY, HIGH); // blink green
    delay(50);
    digitalWrite(PIN_LED_READY, LOW);
    delay(450);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(wifi_ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  Serial.print(F("Inizializing FS..."));
  if (SPIFFS.begin(true)){
    Serial.println(F("done."));
  } else {
    Serial.println(F("fail."));
  }

  Serial.println("Set routing for http server!");
  serverRouting();
  server.begin();
  Serial.println("HTTP server started");

  digitalWrite(PIN_LED_READY, HIGH); // green on
}

void serverRouting() {

  server.on("/admin/upload", HTTP_GET, []() {                 // if the client requests the upload page
    if (!handleFileRead("/upload.html"))                // send it if it exists
      server.send(200, "text/html", PAGE_UPLOAD);       // otherwise, serve predefined form
  });

  server.on("/admin/upload", HTTP_POST,                       // if the client posts to the upload page
    [](){ server.send(200); },                          // Send status 200 (OK) to tell the client we are ready to receive
    handleFileUpload                                    // Receive and save the file
  );

  server.on("/admin/info", handleInfo);
  server.on("/admin/list", handleList);
  server.on("/admin/format", handleFormat);

  server.on("/data", handleData);

  server.onNotFound([]() {                           // If the client requests any URI
    Serial.println(F("On not found"));
    if (!handleFileRead(server.uri())){                  // send it if it exists
        handleNotFound(); // otherwise, respond with a 404 (Not Found) error
    }
  });

  Serial.println(F("Set cache!"));
  // Serve a file with no cache so every tile It's downloaded
  server.serveStatic("/configuration.json", SPIFFS, "/configuration.json","no-cache, no-store, must-revalidate"); // what?
  // Server all other page with long cache so browser chaching they
  server.serveStatic("/", SPIFFS, "/","max-age=31536000");
}

void loop(void) {
  digitalWrite(PIN_LED_READY, LOW);
  server.handleClient();
  digitalWrite(PIN_LED_READY, HIGH);
  delay(2);
}

void handleEncoder() {
  int c0 = digitalRead(PIN_C0);
  int c1 = digitalRead(PIN_C1);

  int state = c0 << 1 | c1; 
  if (lastState == 1 && state == 3) {
    value--;
  } else if (lastState == 2 && state == 3) {
    value++;
  }
  lastState = state;
  counter++;

  if (value > 255) {
    value = 255;
  } else if (value < 0) {
    value = 0;
  }
  if (channel == 0) {
    l0 = value;
  } else if (channel == 1) {
    l1 = value;
  } else if (channel == 2) {
    l2 = value;
  }
}

void handleButton() {
  int cb = digitalRead(PIN_CB);
  if (cb == 0) {
    channel++;
  }
  if (channel > 2) {
    channel = 0;
  }
  counter++;

  // TODO remove
  // XXX why is this called when encoder is rotated? WTF?
  Serial.printf("Value: %d, Counter: %d, channel:%d\n", value, counter, channel);
  Serial.printf("L0: %d, L1: %d, L2:%d\n", l0, l1, l2);
}
