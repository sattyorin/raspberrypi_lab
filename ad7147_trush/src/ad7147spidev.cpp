#include "../include/ad7147/ad7147spidev.h"
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <chrono>
#include <unistd.h>
#include <string.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <byteswap.h>
#include <arpa/inet.h>
#include <array>
#include <bitset>
#include "../include/ad7147/AD7147regmap.h"
#include "../include/ad7147/console_rev.h"
#include <tuple>
#include <unordered_map>
#include <cmath>
using namespace std;
#define A7 ad7147spidev_multi

A7::A7(const char* spidev, int start_bit, int count):
  fd_spi(open(spidev,O_RDWR)),fd_gpio(open("/dev/gpiomem",O_RDWR)), spi_mode(SPI_CPOL|SPI_CPHA),spi_speed(100000/*Hz of SPI*/),
  gpio_start_bit(start_bit),gpio_pincount(count)
{
  unordered_map<string,int> configuration_results {
    {"set mode",ioctl(fd_spi,SPI_IOC_WR_MODE,&spi_mode)},
    {"set max speed", ioctl(fd_spi,SPI_IOC_WR_MAX_SPEED_HZ,&spi_speed)}
  };
  for(auto &[op,ret]: configuration_results){
    if(ret<0) printf(("failed to "+op+"; (%d) %s\n").c_str(),ret,strerror(ret));
    else printf("%s ok\n",op.c_str());
  };
  //set gpio mask
  gpio_mask=0;
  for(int i=0;i<count;i++){
    int val=start_bit+i;
    gpio_mask|=1<<val;
  };
  gpio=(uint32_t*)::mmap(0,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd_gpio,0);
  gpio[7]=gpio_mask;
};
void A7::select(int which){
  int val=which+gpio_start_bit;
  int pos_bitmask=1<<val;
  gpio[10]=pos_bitmask;
};
void A7::unselect(int which){
  int val=which+gpio_start_bit;
  int pos_bitmask=1<<val;
  gpio[7]=pos_bitmask;
};
int A7::read(int which, uint16_t addr, void* buffer, size_t size){
  if(size%sizeof(uint16_t)!=0) return -1;
  spi_command_word read_word(true,addr);
  spi_ioc_transfer transfer_object[2];
  memset(&transfer_object,0,sizeof(transfer_object));
  transfer_object[0].tx_buf=(unsigned long)&read_word;
  transfer_object[0].rx_buf=0;
  transfer_object[0].len=2;
  transfer_object[0].delay_usecs=0;
  transfer_object[0].speed_hz=spi_speed;
  transfer_object[0].bits_per_word=8;
  transfer_object[0].cs_change=false;
  transfer_object[1].tx_buf=0;
  transfer_object[1].rx_buf=(unsigned long)buffer;
  transfer_object[1].len=size;
  transfer_object[1].delay_usecs=0;
  transfer_object[1].speed_hz=spi_speed;
  transfer_object[1].bits_per_word=8;
  transfer_object[1].cs_change=false;
  select(which);
  auto result= ioctl(fd_spi,SPI_IOC_MESSAGE(2),transfer_object);
  unselect(which);
  return result;
};
int A7::read(int which, uint16_t addr){
  uint16_t read_result=0;
  select(which);
  int send_result=read(which,addr,&read_result,sizeof(read_result));
  unselect(which);
  if(send_result<0) return send_result;
  else return read_result;
};
int A7::write(int which, uint16_t addr, void* buffer, size_t size){
  spi_command_word write_word(false,addr);
  spi_ioc_transfer transfer_object;
  memset(&transfer_object,0,sizeof(transfer_object));
  auto transfer_size=size+sizeof(write_word);
  uint8_t* write_buffer=(uint8_t*)malloc(transfer_size);
  printf("tx size: %lu\n",transfer_size);

  memcpy(write_buffer,&write_word,sizeof(write_word));
  memcpy(write_buffer+sizeof(write_word),buffer,size);
  speak(write_buffer,transfer_size);
  select(which);
  int result=::write(fd_spi,write_buffer,transfer_size);
  unselect(which);
  free(write_buffer);
  return result;
};
A7::~A7(){
  ::munmap(gpio,4096);
  ::close(fd_gpio);
  ::close(fd_spi);
};
int speak(const void* ptr,size_t sz){
  const uint8_t *y=reinterpret_cast<const uint8_t*>(ptr);
 // int tmp_data = *((int*)ptr);
  int tmp_data = 0;
  for(size_t i=0;i<sz;i++)
    {
     //printf("%02X ",int(y[i]));
     tmp_data += pow(16,2-2*i)*(int)y[i];
    }
   // printf("tmp_data = %d\n", tmp_data);
  // printf("\n");
    return tmp_data;
};
