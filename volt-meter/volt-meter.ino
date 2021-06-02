#include <SPI.h>
#include <driver/adc.h>

const int battPin1 = 34;
const int battPin2 = 35;
const int potPin = 32;

void setup()
{
  Serial.begin(115200);

  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC1_CHANNEL_7,ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_11);

  delay(100);
}

void loop()
{
  int val1 = adc1_get_raw(ADC1_CHANNEL_6);
  int val2 = adc1_get_raw(ADC1_CHANNEL_7);
  int val3 = adc1_get_raw(ADC1_CHANNEL_4);
  float v1 = val1 * 0.0028144;
  float v2 = val2 * 0.0017489;
  float v3 = ((val3-304) * 0.0008)+0.392;
  
  Serial.print("A: ");
  Serial.print(val3);
  Serial.print(" V:");
  Serial.println(v3);

  delay(1000);
}





// 1 2672 2.16 7.52 // 7.52 / 2672 = 0.0028144
// 2 2150 1.79 3.76 // 3.76 / 2150 = 0.0017489

// 1 1735 1.44 5.05 // 5.05 / 1735 = 0.0029107

// 1 2805      7.86
// 2 2254      3.93
