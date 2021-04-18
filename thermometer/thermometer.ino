#include <Wire.h>

#define MLX90615_I2C_ADDR    0x5B
#define MLX90615_REG_ID_LOW     0x1E
#define MLX90615_REG_ID_HIGH    0x1F
#define MLX90615_REG_TEMP_AMBIENT 0x26
#define MLX90615_REG_TEMP_OBJECT  0x27

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

void setup() {
  Wire.begin();
  Serial.begin(115200);
  
  Serial.print("Sensor ID number = ");
  Serial.println(get_id(), HEX);

}

/*
Scanning...
I2C device found at address 0x3C
I2C device found at address 0x5B
done
*/
 
void loop() {
  Serial.print("Ambient = ");
  Serial.print(get_ambient_temp()); //ambient temperature
  Serial.print(" *C\tObject = ");
  Serial.print(get_object_temp()); //object temperature
  Serial.println(" *C");
  delay(500);
}

uint32_t get_id() {
  uint32_t id;
  id = read_word16(MLX90615_REG_ID_LOW);
  id |= (uint32_t)read_word16(MLX90615_REG_ID_HIGH) << 16;
  return id;
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
