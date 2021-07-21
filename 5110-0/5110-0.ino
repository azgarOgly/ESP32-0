// 5110 screen example code, straight from web, no changes.
// TODO delete

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

/* Declare LCD object for SPI
 Adafruit_PCD8544(CLK,DIN,D/C,CE,RST); */
#define PIN_CLK 18
#define PIN_DIN 23
#define PIN_DC 19
#define PIN_CE 5
#define PIN_RST 14

Adafruit_PCD8544 display = Adafruit_PCD8544(PIN_CLK, PIN_DIN, PIN_DC, PIN_CE, PIN_RST);
int contrastValue = 60; // Default Contrast Value
const int adcPin = 34;
int adcValue = 0;

void setup()
{
  /* Initialize the Display*/
  display.begin();

  /* Change the contrast using the following API*/
  display.setContrast(contrastValue);

  /* Clear the buffer */
  display.clearDisplay();
  display.display();
  delay(1000);
  
  /* Now let us display some text */
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0,1);
  display.setTextSize(2);
  display.println("|ESP32|");
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(22,20);
  display.println("|Nokia|");
  display.setCursor(22,32);
  display.println("|5110|");
  display.display();
  delay(2000);
}

void loop()
{
  /* You can implement your own display logic here*/  
}
