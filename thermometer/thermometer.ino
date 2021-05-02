#include <Wire.h>
#include "mlx90615.h"

mlx90615 thermo;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  
  Serial.print("Sensor ID number = ");
  Serial.println(thermo.get_id(), HEX);
}
 
void loop() {
  Serial.print("Ambient = ");
  Serial.print(thermo.get_ambient_temp()); //ambient temperature
  Serial.print(" *C\tObject = ");
  Serial.print(thermo.get_object_temp()); //object temperature
  Serial.println(" *C");
  delay(500);
}
