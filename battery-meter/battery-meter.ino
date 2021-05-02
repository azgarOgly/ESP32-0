#include <SPI.h>
#include <driver/adc.h>
#include <Adafruit_PCD8544.h>

/* Declare LCD object for SPI
  Adafruit_PCD8544(CLK,DIN,D/C,CE,RST); */
#define PIN_CLK 18
#define PIN_DIN 23
#define PIN_DC 19
#define PIN_CE 5
#define PIN_RST 14
#define PIN_LIGHT 25

Adafruit_PCD8544 display = Adafruit_PCD8544(PIN_CLK, PIN_DIN, PIN_DC, PIN_CE, PIN_RST);
int contrastValue = 60; // Default Contrast Value

const int battPin1 = 34;
const int battPin2 = 35;

void setup()
{
  pinMode(PIN_LIGHT, OUTPUT);
  ledcSetup(0, 8000, 8);
  ledcAttachPin(PIN_LIGHT, 0);
  ledcWrite(0, 1);
  
  display.begin();
  display.setContrast(contrastValue);

  display.clearDisplay();
  display.display();

  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC1_CHANNEL_7,ADC_ATTEN_DB_11);

  delay(100);
}

void loop()
{
  int val1 = adc1_get_raw(ADC1_CHANNEL_6);
  int val2 = adc1_get_raw(ADC1_CHANNEL_7);
  float v1 = val1 * 0.0028144;
  float v2 = val2 * 0.0017489;
  
  display.clearDisplay();
  display.display();

  display.setTextSize(2);
  display.setCursor(0, 8);
  display.print("1:");
  display.print(v1-v2);
  display.setCursor(0, 24);
  display.print("2:");
  display.print(v2);

  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print("1 ");
  display.print(val1);
  display.print(" 2 ");
  display.print(val2);

  display.display();
  delay(120);
}

// 1 2672 2.16 7.52 // 7.52 / 2672 = 0.0028144
// 2 2150 1.79 3.76 // 3.76 / 2150 = 0.0017489

// 1 1735 1.44 5.05 // 5.05 / 1735 = 0.0029107

// 1 2805      7.86
// 2 2254      3.93
