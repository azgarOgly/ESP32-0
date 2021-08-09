#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "n13-48.h"

#define PIN_L0 27 // output pins per channel
#define PIN_L1 26
#define PIN_L2 25
#define PIN_C0 33 // pins for encoder
#define PIN_C1 32
#define PIN_CB 14
#define PIN_LED_ON 16 // status LED yellow
#define PIN_LED_READY 4 // status LED green

WebServer server(80);
int l0 = 0; // duty cycles by channel
int l1 = 0;
int l2 = 0;
int value = 0; // value of encoder
int channel = 0; // channel to control with encoder
int lastState = 3; // encoder last state
int counter = 0; // interrupt counter

void handleRoot() {
  char temp[2000];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

// TODO move to separate file
  snprintf(temp, 2000,

           "<html>\
  <head>\
    <title>ESP32 L&Ouml;MP server</title>\
    <style>\
      body { background-color: #A0B0A0; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
      <script>\
  function up() {\
    var element0 = document.getElementById('L0input');\
    var element1 = document.getElementById('L1input');\
    var element2 = document.getElementById('L2input');\
    var value0 = element0.value;\
    var value1 = element1.value;\
    var value2 = element2.value;\
    const Http = new XMLHttpRequest();\
    const url='data?L0=' + value0 + '&L1=' + value1  + '&L2=' + value2;\
    Http.open('GET', url);\
    Http.send();\
\
    Http.onreadystatechange = (e) => {\
      console.log(Http.responseText)\
    } \
  }\
  </script>\
  </head>\
  <body>\
    <h1>ESP32 L&Ouml;MP server</h1>\
    <input id='L0input' type='range' min='0' max='255' value='%d' onChange='up()'><br>\
    <input id='L1input' type='range' min='0' max='255' value='%d' onChange='up()'><br>\
    <input id='L2input' type='range' min='0' max='255' value='%d' onChange='up()'><br>\
  </body>\
</html>",
           l0, l1, l2);
  server.send(200, "text/html", temp);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void handleData() { // receive GET request with new values per channel
  String message = "Mmkay ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
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

  server.on("/", handleRoot);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.on("/data", handleData);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  digitalWrite(PIN_LED_READY, HIGH); // green on
}

void loop(void) {
  digitalWrite(PIN_LED_READY, LOW);
  server.handleClient(); // seems to be non blocking call
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
