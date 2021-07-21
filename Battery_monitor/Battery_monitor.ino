#include <SD.h>
#include <SPI.h>
#include "WiFi.h"
#include <HTTPClient.h>
#include "Config.h"
#include <driver/adc.h>

Config conf;
File logFile;

void setup(){
    Serial.begin(115200);
    delay(1000);

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_DB_11);

    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    logFile = SD.open("/logfile.log", FILE_APPEND);
    logFile.println("\n************************");
    logFile.println("Starting battery monitor");

    logFile.print("SD Card Type: ");
    if(cardType == CARD_MMC) {
        logFile.println("MMC");
    } else if(cardType == CARD_SD) {
        logFile.println("SDSC");
    } else if(cardType == CARD_SDHC) {
        logFile.println("SDHC");
    } else {
        logFile.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    logFile.printf("SD Card Size: %lluMB\n", cardSize);

    String path = "/logfile.log";
    Serial.print("Reading file: "); Serial.print(path);Serial.println();

    File file = SD.open(path);
    if(!file) {
        return;
    }
    
    Serial.print("File conent:\n");
    while(file.available()) {
      Serial.write(file.read());
    }
    file.close();

    conf.begin(SD);
    Serial.printf("SSID: '%s'\n", conf.getSSID());
    Serial.printf("password: '%s'\n", conf.getPassword());
    Serial.printf("serviceUrl: '%s'\n", conf.getServiceUrl());

    logFile.printf("SSID: '%s'\n", conf.getSSID());
    logFile.printf("password: '*********'\n");
    logFile.printf("serviceUrl: '%s'\n", conf.getServiceUrl());

    initWiFi();
    logFile.close();
}

void loop() {
  File dataFile = SD.open("/data.log", FILE_APPEND);

  struct tm timeinfo;
  if(getLocalTime(&timeinfo)){
    dataFile.print(&timeinfo, "%d.%m.%Y %H:%M:%S : ");
    Serial.print(&timeinfo, "%d.%m.%Y %H:%M:%S : ");
  }

  int val1 = adc1_get_raw(ADC1_CHANNEL_6);
  float v1 = (((val1-304) * 0.0008)+0.38)*2;
  
  dataFile.print("A: ");
  dataFile.print(val1);
  dataFile.print(" V:");
  dataFile.println(v1);
  Serial.print(" V:");
  Serial.println(v1);

  dataFile.close();
  delay(30000);
}

void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    logFile.println("Failed to obtain time");
    return;
  }
  logFile.println(&timeinfo, "%d.%m.%Y %H:%M:%S");
  Serial.println(&timeinfo, "%d.%m.%Y %H:%M:%S");
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  // const char* ssid = conf.getSSID().c_str();
  // const char* password = conf.getPassword().c_str();
  WiFi.begin(conf.getSSID(), conf.getPassword());
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(200);
  }
  Serial.println(WiFi.localIP());

  char* ntpServer = "pool.ntp.org";
  long  gmtOffset_sec = 3600*2;
  int   daylightOffset_sec = 3600;
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  logFile.print("Connected to ");
  logFile.print(conf.getSSID());
  logFile.print(", signal strength ");
  logFile.print(WiFi.RSSI());
  logFile.print(", local IP ");
  logFile.print(WiFi.localIP());
}
