#include "WiFi.h"
#include <Wire.h>
#include <n13-48.h>

#define MLX90615_I2C_ADDR    0x5B
#define MLX90615_REG_TEMP_AMBIENT 0x26
#define MLX90615_REG_TEMP_OBJECT  0x27


WiFiServer wifiServer(8080);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  wifiServer.begin();
  
  Serial.println("Hello!");
}

void loop() {
  if(WiFi.status() != WL_CONNECTED) {
    initWiFi();
  } else {
    Serial.println("Waiting...");
  }

  WiFiClient client = wifiServer.available();
 
  if (client) {
    Serial.println("\nClient connected");
   if (client.connected()) {
        while (client.available()>0) {
        char c = client.read();
        Serial.write(c);
      }
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/plain");
      client.println("Connection: close");
      client.println("Server: ESP32 homemade raw");
      client.println();   
      
      client.println("hello, nutipraks!");
      client.println("temperature is ");
      client.print("ambient ");
      client.print(get_ambient_temp());
      client.println("C");
      client.print("object ");
      client.print(get_object_temp());
      client.println("C");
    }
    client.stop();
    Serial.println("\nClient disconnected");
  }
  delay(1000);
}

void initWiFi() {
  Serial.println("My MAC is " + WiFi.macAddress());
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(200);
  }
  Serial.println(WiFi.localIP());
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
