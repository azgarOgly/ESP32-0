#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "FS.h"
#include "SPIFFS.h"
#include "n13-48.h"
#include "SPIFFSServer.h"


// allows you to set the realm of authentication Default:"Login Required"
const char* www_realm = "Custom Auth Realm";
// the Content of the HTML response in case of Unautherized Access Default:empty
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

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass); // comes from n13-48.h
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin("esp32")) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");

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
}
 
void loop(void) {
  server.handleClient();
}

void serverRouting() {

  server.on("/upload", HTTP_GET, []() {                 // if the client requests the upload page
    if (!handleFileRead("/upload.html"))                // send it if it exists
      server.send(200, "text/html", PAGE_UPLOAD); // otherwise, respond with a 404 (Not Found) error
  });

  server.on("/upload", HTTP_POST,                       // if the client posts to the upload page
    [](){ server.send(200); },                          // Send status 200 (OK) to tell the client we are ready to receive
    handleFileUpload                                    // Receive and save the file
  );

  server.on("/info", handleInfo);
  server.on("/list", handleList);
  server.on("/format", handleFormat);

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
