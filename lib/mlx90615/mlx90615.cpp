#include <Wire.h>
#include "mlx90615.h"

#define MLX90615_I2C_ADDR    (uint16_t)0x5B
#define MLX90615_REG_ID_LOW     0x1E
#define MLX90615_REG_ID_HIGH    0x1F
#define MLX90615_REG_TEMP_AMBIENT 0x26
#define MLX90615_REG_TEMP_OBJECT  0x27


  uint32_t mlx90615::get_id() {
    uint32_t id;
    id = read_word16(MLX90615_REG_ID_LOW);
    id |= (uint32_t)read_word16(MLX90615_REG_ID_HIGH) << 16;
    return id;
  }

  float mlx90615::get_ambient_temp() {
    return raw_to_temp(read_word16(MLX90615_REG_TEMP_AMBIENT));
  }

  float mlx90615::get_object_temp() {
    return raw_to_temp(read_word16(MLX90615_REG_TEMP_OBJECT));
  }

  float mlx90615::raw_to_temp(uint16_t raw) {
    return raw * 0.02 - 273.15;
  }

  uint16_t mlx90615::read_word16(uint8_t reg) {
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
 
