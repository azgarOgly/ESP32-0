#include <SPI.h>
#include <Wire.h>

// #include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

/* Declare LCD object for SPI
  Adafruit_PCD8544(CLK,DIN,D/C,CE,RST); */
#define PIN_CLK 18
#define PIN_DIN 23
#define PIN_DC 19
#define PIN_CE 5
#define PIN_RST 14
#define PIN_LIGHT 27

#define MLX90615_I2C_ADDR    0x5B
#define MLX90615_REG_TEMP_AMBIENT 0x26
#define MLX90615_REG_TEMP_OBJECT  0x27

Adafruit_PCD8544 display = Adafruit_PCD8544(PIN_CLK, PIN_DIN, PIN_DC, PIN_CE, PIN_RST);
int contrastValue = 60; // Default Contrast Value
const int adcPin = 34;
int adcValue = 0;

void setup()
{
  Wire.begin();

  pinMode(PIN_LIGHT, OUTPUT);
  ledcSetup(0, 8000, 8);
  ledcAttachPin(PIN_LIGHT, 0);
  ledcWrite(0, 50);
  
  display.begin();
  display.setContrast(contrastValue);

  display.clearDisplay();
  display.display();
  delay(100);
}

void loop()
{
  long m = millis();
  char buffer [50];
  char banana [6] = "|\\-/5";
  char c = (m/500)%2 ? ' ' : ':';
  sprintf(buffer, "%c%c%010d %c", banana[(m/250)%4], c, m, (m/332)&0xFF);
    
  display.clearDisplay();
  display.display();

  display.setTextSize(2);
  display.setCursor(0, 8);
  display.print("A:");
  display.print(get_ambient_temp());
  display.setCursor(0, 24);
  display.print("0:");
  display.print(get_object_temp());

  display.setTextSize(1);
  display.setCursor(0, 40);
  display.println(buffer);
  display.display();
  delay(120);
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
