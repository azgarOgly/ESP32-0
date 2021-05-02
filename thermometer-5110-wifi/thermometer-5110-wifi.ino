#include <SPI.h>
#include <Wire.h>
#include "time.h"
#include <WiFi.h>
#include <n13-48.h>

// #include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

/* Declare LCD object for SPI
  Adafruit_PCD8544(CLK,DIN,D/C,CE,RST); */
#define PIN_CLK 18
#define PIN_DIN 23
#define PIN_DC 19
#define PIN_CE 5
#define PIN_RST 14
#define PIN_LIGHT 25

#define MLX90615_I2C_ADDR    0x5B
#define MLX90615_REG_TEMP_AMBIENT 0x26
#define MLX90615_REG_TEMP_OBJECT  0x27

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600*2;
const int   daylightOffset_sec = 3600;

#define STATUS_CONNECTING 1
#define STATUS_DISCONNECTED 0
#define STATUS_CONNECTED 3
long wifi_connect_start = 0;
int wifi_status = STATUS_DISCONNECTED;

Adafruit_PCD8544 display = Adafruit_PCD8544(PIN_CLK, PIN_DIN, PIN_DC, PIN_CE, PIN_RST);
int contrastValue = 60; // Default Contrast Value
const int adcPin = 34;
int adcValue = 0;

void setup()
{
  Serial.begin(115200);

  Wire.begin();
  disconnectWiFi();

  pinMode(PIN_LIGHT, OUTPUT);
  ledcSetup(0, 8000, 8);
  ledcAttachPin(PIN_LIGHT, 0);
  ledcWrite(0, 1);
  
  display.begin();
  display.setContrast(contrastValue);

  display.clearDisplay();
  display.display();
  delay(100);
}

void loop()
{
  long now = millis();
  char wifiString [50];

  if(WiFi.status() != WL_CONNECTED) {
    initWiFi();
    sprintf(wifiString, "wait %04d", (now - wifi_connect_start)/100);
  } else {
    wifi_status = STATUS_CONNECTED;
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    sprintf(wifiString, "%s(%d)", WiFi.SSID(), WiFi.RSSI());
  }

  long m = millis();
  char banana [6] = "|\\-/5";
  char c = (m/500)%2 ? ' ' : ':';
  // char buffer [50];
  // sprintf(buffer, "%c%c%010d %c", banana[(m/250)%4], c, m, (m/332)&0xFF);
    
  display.clearDisplay();
  display.display();

  display.setCursor(0, 0);
  display.println(wifiString);

  display.setTextSize(2);
  display.setCursor(0, 8);
  display.print("A:");
  display.print(get_ambient_temp());
  display.setCursor(0, 24);
  display.print("0:");
  display.print(get_object_temp());

  display.setTextSize(1);
  display.setCursor(0, 40);
  display.println(getLocalTime());
  display.display();
  delay(120);
}

void initWiFi() {
  if (wifi_status != STATUS_CONNECTING) {
    wifi_connect_start = millis();
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_pass);
    wifi_status = STATUS_CONNECTING;
  }
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

void disconnectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

String getLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return "timeoops.";
  }
  char time[50]; //50 chars should be enough
  strftime(time, sizeof(time), "%d.%m %H:%M:%S", &timeinfo);
  return time;
}
