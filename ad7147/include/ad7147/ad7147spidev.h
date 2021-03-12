#ifndef __H_AD7147SPIDEV__
#define __H_AD7147SPIDEV__
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <ext/stdio_filebuf.h>
struct ad7147spidev_multi{
  int fd_spi, fd_gpio;
  uint8_t spi_mode;
  uint32_t spi_speed;
  uint32_t gpio_mask;
  int gpio_start_bit;
  int gpio_pincount;
  uint32_t *gpio;
  ad7147spidev_multi(const char* filepath, int start_bit, int count);
  void select(int which);
  void unselect(int which);
  int read(int which, uint16_t addr, void* buffer, size_t size);
  int read(int which, uint16_t addr);
  int write(int which, uint16_t addr, void* buffer, size_t size);
  ~ad7147spidev_multi();
};
int speak(const void* ptr,size_t sz);
#endif
