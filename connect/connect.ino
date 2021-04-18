#include "WiFi.h"
#include <n13-48.h>

WiFiServer wifiServer(8080);

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  wifiServer.begin();
  
  Serial.println("Hello!");
}

void loop() {
  if(WiFi.status() != WL_CONNECTED) {
    initWiFi();
  } else {
    Serial.println("Waiting...");
  }

  WiFiClient client = wifiServer.available();
 
  if (client) {
    Serial.println("\nClient connected");
   if (client.connected()) {
        while (client.available()>0) {
        char c = client.read();
        Serial.write(c);
      }
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/plain");
      client.println("Connection: close");
      client.println();   
      
      client.println("helo \nthis is ESP32 on my desktop speakin. \n/helo");
    }
    client.stop();
    Serial.println("\nClient disconnected");
  }
  delay(1000);
}

void initWiFi() {
  Serial.println("My MAC is " + WiFi.macAddress());
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(200);
  }
  Serial.println(WiFi.localIP());
}
