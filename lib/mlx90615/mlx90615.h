#ifndef AZ_MLX90615
#define AZ_MLX90615

class mlx90615 {

	public:
	  uint32_t get_id();
	  float get_ambient_temp();
	  float get_object_temp();

	private:
	  float raw_to_temp(uint16_t raw);
	  uint16_t read_word16(uint8_t reg);

};

#endif