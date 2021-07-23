#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "FS.h"
#include "SPIFFS.h"
#include "n13-48.h"
 
// allows you to set the realm of authentication Default:"Login Required"
const char* www_realm = "Custom Auth Realm";
// the Content of the HTML response in case of Unautherized Access Default:empty
String authFailResponse = "Authentication Failed";
 
const char* www_username = "admin";
const char* www_password = "esp32";
 
WebServer server(80);
File fsUploadFile;              // a File object to temporarily store the received file
 
bool loadFromSPIFFS(String path, String dataType) {
  Serial.print("Requested page -> ");
  Serial.println(path);
  if (SPIFFS.exists(path)){
    File dataFile = SPIFFS.open(path, "r");
    if (!dataFile) {
        handleNotFound();
        return false;
    }

    if (server.streamFile(dataFile, dataType) != dataFile.size()) {
      Serial.println("Sent less data than expected!");
    }else{
      Serial.println("Page served!");
    }

    dataFile.close();
    return true;
  } else {
    handleNotFound();
    return false;
  }
}
 
void serverRouting();

static const char PAGE_UPLOAD[] PROGMEM = R"(
<form method="post" enctype="multipart/form-data">
    <input type="file" name="name">
    <input class="button" type="submit" value="Upload">
</form>
)";

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
 
String getContentType(String filename){
  if(filename.endsWith(F(".htm")))          return F("text/html");
  else if(filename.endsWith(F(".html")))    return F("text/html");
  else if(filename.endsWith(F(".css")))     return F("text/css");
  else if(filename.endsWith(F(".js")))      return F("application/javascript");
  else if(filename.endsWith(F(".json")))    return F("application/json");
  else if(filename.endsWith(F(".png")))     return F("image/png");
  else if(filename.endsWith(F(".gif")))     return F("image/gif");
  else if(filename.endsWith(F(".jpg")))     return F("image/jpeg");
  else if(filename.endsWith(F(".jpeg")))    return F("image/jpeg");
  else if(filename.endsWith(F(".ico")))     return F("image/x-icon");
  else if(filename.endsWith(F(".xml")))     return F("text/xml");
  else if(filename.endsWith(F(".pdf")))     return F("application/x-pdf");
  else if(filename.endsWith(F(".zip")))     return F("application/x-zip");
  else if(filename.endsWith(F(".gz")))      return F("application/x-gzip");
  return F("text/plain");
}
 
bool handleFileRead(String path){
  Serial.print(F("handleFileRead: "));
  Serial.println(path);

  if (path.startsWith("/upload") && !server.authenticate(www_username, www_password)) {
      server.requestAuthentication();
  }
 
  if (path.endsWith("/")) path += F("index.html");          // If a folder is requested, send the index file
  String contentType = getContentType(path);                // Get the MIME type
  String pathWithGz = path + F(".gz");
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){     // If the file exists, either as a compressed archive, or normal
    if(SPIFFS.exists(pathWithGz))                           // If there's a compressed version available
      path += F(".gz");                                     // Use the compressed version
    fs::File file = SPIFFS.open(path, "r");                 // Open the file
    size_t sent = server.streamFile(file, contentType);     // Send it to the client
    file.close();                                           // Close the file again
    Serial.println(String(F("\tSent file: ")) + path + String(F(" of size ")) + sent);
    return true;
  }
  Serial.println(String(F("\tFile Not Found: ")) + path);
  return false;                                             // If the file doesn't exist, return false
}

void handleFileUpload(){ // upload a new file to the SPIFFS
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) { filename = "/"+filename; }
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {                                   // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
      server.sendHeader("Location","/success.html");      // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
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

void handleInfo() { 
  int totalBytes = SPIFFS.totalBytes();
  int usedBytes = SPIFFS.usedBytes();
  String message = "SPIFF Info: ";
  message += "\ntotalBytes: ";
  message += totalBytes;
  message += "\nusedBytes: ";
  message += usedBytes;
  message += "\nfreeBytes: ";
  message += totalBytes - usedBytes;
  message += "\n";
  server.send(200, "text/plain", message);
}

void handleList() { 
  String message = "Listing files: ";
  Serial.printf("Listing directory: %s\r\n", "/");

  File root = SPIFFS.open("/");
  if(!root || !root.isDirectory()){
    Serial.println("ERROR: failed to list directory");
    server.send(500, "text/plain", "failed to list directory");
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()) { // can that happen on SPIFFS?
      message += "\n";
      message += file.name();
      message += ": dir";
    } else {
      message += "\n";
      message += file.name();
      message += ": ";
      message += file.size();
    }
    file = root.openNextFile();
  }
  server.send(200, "text/plain", message);
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
