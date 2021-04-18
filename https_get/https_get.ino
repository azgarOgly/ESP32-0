#include "WiFi.h"
#include <HTTPClient.h>
#include <n13-48.h>

HTTPClient http;

void setup() {
  Serial.begin(115200);
  disconnectWiFi();
  Serial.println("helo");
}

void loop() {
  if(WiFi.status() != WL_CONNECTED) {
    initWiFi();
  }
  Serial.println("GET ");

  String url = "https://docs.google.com/forms/u/0/d/e/1FAIpQLScltzwnFLHARu-1SZJpXh885mHwaFlaOv8PFLk-WlA7cTPfag/formResponse?entry.1274515786=24.3";

  http.begin(url);
  int httpCode = http.GET();
 
  if (httpCode > 0) { //Check for the returning code
      String payload = http.getString();
      Serial.println(httpCode);
      // Serial.println(payload);
  } else {
    Serial.print("Error on HTTP request");
    Serial.println(httpCode);
  }

  http.end(); //Free the resources
  
  delay(3000);
}

void initWiFi() {
  Serial.println("My MAC is " + WiFi.macAddress());
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);
  Serial.print("Connecting to WiFi ");
  int count = 1;
  while (WiFi.status() != WL_CONNECTED) {
    if (count++ % 5 == 0) {
      Serial.print(count / 5);
    } else {
      Serial.print('.');
    }
    delay(200);
  }
  Serial.println("Connected.");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
}

void disconnectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}
