#ifndef AZ_SPIFFServer
#define AZ_SPIFFServer

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "FS.h"
#include "SPIFFS.h"
#include "n13-48.h"
 
// allows you to set the realm of authentication Default:"Login Required"
extern const char* www_realm;
// the Content of the HTML response in case of Unautherized Access Default:empty
extern String authFailResponse;
 
extern const char* www_username;
extern const char* www_password;

extern char PAGE_UPLOAD[];

extern WebServer server;

extern File fsUploadFile;              // a File object to temporarily store the received file

 
bool loadFromSPIFFS(String path, String dataType);
void serverRouting();
String getContentType(String filename);
bool handleFileRead(String path);
void handleFileUpload();
void handleNotFound();
void handleInfo();
void handleFormat();
void handleList();

#endif
