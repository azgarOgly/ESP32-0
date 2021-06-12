#include <SD.h>
#include <SPI.h>

void setup(){
    Serial.begin(115200);
    delay(1000);

    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    String path = "/config.cfg";
    Serial.printf("Reading file: %s\n", path);

    File file = SD.open(path);
    if(!file){
        return;
    }
    Serial.print("File conent:\n");
    while(file.available()){
      Serial.write(file.read());
    }
    
    file.close();
}

void loop(){}
