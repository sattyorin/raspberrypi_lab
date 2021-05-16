#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <unistd.h> //write
#include <string.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unordered_map>
#include <memory>
#include <arpa/inet.h>
#include <cmath>

#include "AD7147RegMap.h"

#include <typeinfo>

#define NUMBER_OF_REGISTERS 12
#define AD7147QUANTITY 2

class AD7147SPIDev
{
	private:
		int fd_spi, fd_gpio;
	 	uint8_t spi_mode;
	 	uint32_t spi_speed;
	 	uint32_t gpio_mask;
	 	int gpio_start_bit;
	 	int gpio_pincount;
	 	uint32_t *gpio;
		#pragma pack(push,1) //define struct align
		struct spi_command_word //p32
		{
			// x0 : MSB15, x1 : MSB14 ... rw_flag : MSB10, reg_hi : MSB9-MSB8, reg_lo : MSB7-MSB0
			// R/bar{W} : Read->True, Write->False
			uint8_t reg_hi:2, rw_flag:1, x4:1, x3:1, x2:1, x1:1, x0:1; //bit field
			uint8_t reg_lo;
			inline spi_command_word(bool rw, uint16_t register_address): rw_flag(rw)
			{
				x0=x1=x2=1;
				x3=x4=0;
				uint8_t* bptr=(uint8_t*)&register_address;
				reg_hi=bptr[1];
				reg_lo=bptr[0];
			};
		};
		#pragma pack(pop)

	public:
		AD7147SPIDev(const char* spidev, int start_bit, int count);
		void initialize();
		int write(uint16_t addr, void* buffer, size_t size);
		void speak(const void* ptr, size_t sz);
		template<typename X> void speak(const X& val);
		// int getCVal(uint16_t addr);
		void getCVal(uint16_t (&data)[NUMBER_OF_REGISTERS]);
		void select(int which);
		void unselect(int which);
		int read(uint16_t addr, void* buffer, size_t size);
		uint16_t read(uint16_t addr);
		~AD7147SPIDev();
};

class AD7147SPIDevMulti : public AD7147SPIDev
{
	// private:
	// 	int fd_spi, fd_gpio;
	// 	uint8_t spi_mode;
	// 	uint32_t spi_speed;
	// 	uint32_t gpio_mask;
	// 	int gpio_start_bit;
	// 	int gpio_pincount;
	// 	uint32_t *gpio;

	public:
		using AD7147SPIDev::AD7147SPIDev;
		// AD7147SPIDevMulti(const char* filepath, int start_bit, int count);
		//  void select(int which);
		//  void unselect(int which);
		//  int read(int which, uint16_t addr, void* buffer, size_t size);
		//  int read(int which, uint16_t addr);
		//  int write(int which, uint16_t addr, void* buffer, size_t size);
		// ~AD7147SPIDevMulti();
};
