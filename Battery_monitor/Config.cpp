#include "Config.h"

void Config::begin(fs::FS &fs) {
    File file = fs.open("/config.cfg");
    if(!file){
        status = "Failed to open file for reading";
        return;
    }

    char key[16] = "0123456789ABCDE"; key[15] = 0; 
    char value[64] = "0123456789ABCDE"; value[63] = 0;
    
    resetIndexes();
    boolean keyFound = false;
    
    while(file.available()) {
      char c = file.read();
      if (c == '\n' || c == '\r') {
        if (keyFound) {
          copyString(value);
          useProperty(key, value);
        }   
        resetIndexes();
        keyFound = false;
      } else if (c == '=') {
        if (headIndex >= 0) {
          keyFound = true;
          copyString(key);
        } else {
          key[0] = 0;
        }
        resetIndexes();
      } else {
        if (c != ' ' && c != '\t') {
          if (headIndex < 0) {
            headIndex = index;
          }
          tailIndex = index;
        }
        buffer[index++] = c;
      }
    }

    file.close();
}

void Config::useProperty(char key[], char value[]) {
  if (strcmp("SSID", key) == 0) {
    for(int i=0; i<16; i++) { SSID[i] = value[i]; }
  } else if (strcmp("password", key) == 0) {
    for(int i=0; i<16; i++) { password[i] = value[i]; }
  } else if (strcmp("serviceUrl", key) == 0) {
    for(int i=0; i<64; i++) { serviceUrl[i] = value[i]; }
  }
}

void Config::copyString(char target[]) {
  if (headIndex < 0) {
    headIndex = 0;
    if (tailIndex == 0) {
      target[0] = 0;
      return;  
    }
  }
  for (int i=0; i<=tailIndex-headIndex; i++) {
    target[i] = buffer[headIndex+i];
  }
  target[tailIndex-headIndex+1]=0;
}

char* Config::getSSID() {
  return SSID;
}
char* Config::getPassword() {
  return password;  
}
char* Config::getServiceUrl() {
  return serviceUrl;  
}

String Config::getStatus() {
  return status;
}

void Config::resetIndexes() {
  index = 0;
  headIndex = -1;
  tailIndex = 0;
}
