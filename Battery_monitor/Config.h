#ifndef AZ_Config
#define AZ_Config

#include <SD.h>

class Config {
  public:
    void begin(fs::FS &fs);
    char* getSSID();
    char* getPassword();
    char* getServiceUrl();

    String getStatus();

  private:
    char SSID[16];
    char password[16];
    char serviceUrl[64];

    String status;

    char buffer[128];
    int index;
    int headIndex;
    int tailIndex;

    void resetIndexes();
    void copyString(char target[]);
    void useProperty(char key[], char value[]);
};

#endif
