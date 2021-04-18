#include <WiFi.h>
#include "time.h"
#include <HTTPClient.h>
#include <Wire.h>
#include <n13-48.h>

const char* ntpServer = "pool.ntp.org";

const long  gmtOffset_sec = 3600*2;
const int   daylightOffset_sec = 3600;

#define MLX90615_I2C_ADDR    0x5B
#define MLX90615_REG_ID_LOW     0x1E
#define MLX90615_REG_ID_HIGH    0x1F
#define MLX90615_REG_TEMP_AMBIENT 0x26
#define MLX90615_REG_TEMP_OBJECT  0x27

HTTPClient http;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  disconnectWiFi();
  Serial.println("helo");
}

void loop() {
  if(WiFi.status() != WL_CONNECTED) {
    initWiFi();
  }
  Serial.println("GET ");

  // String url = "https://docs.google.com/forms/u/0/d/e/1FAIpQLScltzwnFLHARu-1SZJpXh885mHwaFlaOv8PFLk-WlA7cTPfag/formResponse?entry.1274515786=";
  // entry.1274515786: 123
  // entry.1568784093: 321
  // String url = "https://forms.gle/kbniMY5LbFwpjg8v9";
  String url = "https://docs.google.com/forms/u/0/d/e/1FAIpQLScltzwnFLHARu-1SZJpXh885mHwaFlaOv8PFLk-WlA7cTPfag/formResponse";

  http.begin(url + "?entry.1274515786=" + get_ambient_temp() + "&entry.1568784093=" + get_object_temp());
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

  printLocalTime();
  delay(30000);
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

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

}

void disconnectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

float get_ambient_temp() {
  float temp;
  temp = read_word16(MLX90615_REG_TEMP_AMBIENT) * 0.02 - 273.15;
  return temp;
}

float get_object_temp() {
  float temp;
  temp = read_word16(MLX90615_REG_TEMP_OBJECT) * 0.02 - 273.15; 
  return temp;
}

uint16_t read_word16(uint8_t reg) {
  uint16_t data;
  Wire.beginTransmission(MLX90615_I2C_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(MLX90615_I2C_ADDR, (uint8_t)3);
  data = Wire.read();       // read low byte
  data |= Wire.read() << 8; // read high byte
  Wire.read(); // read and discard PEC (packet error code)
  return data;
}
