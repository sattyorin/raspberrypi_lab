#include <ros/ros.h>
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
#include <cmath>
#include <limits>
#include <bitset>
#include "wiringPi.h"
#include "wiringPiSPI.h"
#include "../include/ad7147/AD7147regmap.h"
#include <tuple>
#include <unordered_map>
#include <std_msgs/Int32.h>
using namespace std;

int tmp_data = 0;

void speak(const void* ptr,size_t sz)
{
  tmp_data = 0;
  const uint8_t *y=reinterpret_cast<const uint8_t*>(ptr);
  for(int i=0;i<sz;i++)
  {
    // printf("%02X ",int(y[i]));
    tmp_data += pow(16,2-2*i)*(int)y[i];
  }
  //printf("%d\n", tmp_data);
};

template<typename X> void speak(const X& val)
{
  speak(&val,sizeof(X));
};

void speak16(uint16_t x) 
{
  speak(x);
};

struct spidev
{
  int fd;
  uint8_t mode;
  uint32_t speed;
  spidev(const char* path):fd(open(path,O_RDWR)),mode(SPI_CPOL|SPI_CPHA),speed(100000/*Hz of SPI*/)
  {
    unordered_map<string,int> configuration_results
    {
      {"set mode",ioctl(fd,SPI_IOC_WR_MODE,&mode)},
      {"set max speed", ioctl(fd,SPI_IOC_WR_MAX_SPEED_HZ,&speed)}
    };
    for(auto &[op,ret]: configuration_results)
    {
      if(ret<0) printf(("failed to "+op+"; (%d) %s\n").c_str(),ret,strerror(ret));
      else printf("%s ok\n",op.c_str());
    };
  };
  
  int read(uint16_t addr,void *buffer, size_t size){
    if(size%sizeof(uint16_t)!=0) return -1;
    spi_command_word read_word(true,addr);
    spi_ioc_transfer transfer_object[2];
    memset(&transfer_object,0,sizeof(transfer_object));
    transfer_object[0].tx_buf=(unsigned long)&read_word;
    transfer_object[0].rx_buf=0;
    transfer_object[0].len=2;
    transfer_object[0].delay_usecs=0;
    transfer_object[0].speed_hz=speed;
    transfer_object[0].bits_per_word=8;
    transfer_object[0].cs_change=false;
    transfer_object[1].tx_buf=0;
    transfer_object[1].rx_buf=(unsigned long)buffer;
    transfer_object[1].len=size;
    transfer_object[1].delay_usecs=0;
    transfer_object[1].speed_hz=speed;
    transfer_object[1].bits_per_word=8;
    transfer_object[1].cs_change=false;
    return ioctl(fd,SPI_IOC_MESSAGE(2),transfer_object);
    
  };
  int read(uint16_t addr){ 
    uint16_t read_result=0;
    int send_result=read(addr,&read_result,sizeof(read_result));
    if(send_result<0) return send_result;
    else return read_result;
  };
  int write(uint16_t addr, void *buffer, size_t size){
    spi_command_word write_word(false,addr);
    spi_ioc_transfer transfer_object;
    memset(&transfer_object,0,sizeof(transfer_object));
    auto transfer_size=size+sizeof(write_word);
    uint8_t* write_buffer=(uint8_t*)malloc(transfer_size);
    printf("tx size: %lu\n",transfer_size);
    
    memcpy(write_buffer,&write_word,sizeof(write_word));
    memcpy(write_buffer+sizeof(write_word),buffer,size);
    speak(write_buffer,transfer_size);
    int result=::write(fd,write_buffer,transfer_size);
    free(write_buffer);
    return result;
  };
  ~spidev(){close(fd);}
};


void initialize(spidev& spi){ 
  for(int i=0;i<12;i++){
    auto base_address=i*8+STAGE0_CONNECTION;
    uint16_t config[] {
      // <C-F> 0x80 for magic values definitions
      0x7FFD, 0x1FFF, 0x30, 0x2620 ,0x1600,0x1600,0x1600,0x1600
    };
    auto config_size=sizeof(config);
    for(size_t i=0;i<config_size;i++){
      config[i]=htons(config[i]);
    };
    printf("writing register %d, base address %X\n",i,base_address);
    spi.write(base_address,&config,sizeof(config));
  };
  for(int i=0x80;i<=0xDF;i++){
    speak16(spi.read(i));
  };
  // READ P.40 for MAGIC NUMBERS
  uint8_t initialization_bytes[] {0x82,0xb2,0x0,0x0,0x32,0x30,0x04,0x19,0x08,0x32,0x0,0x0,0x1,0x0};
  spi.write(0,initialization_bytes,sizeof(initialization_bytes));
  uint16_t start_word=0xFF0F;
  spi.write(1,&start_word,2);
};

int main(int argc, char **argv){
  ros::init(argc, argv, "read_AD7147");
  ROS_INFO("[read_AD7147] init to read_AD7147");

  ros::NodeHandle n;
  ros::Publisher pub_AD7147_val = n.advertise<std_msgs::Int32>("AD7147_val", 20);
  std_msgs::Int32 msg;

  spidev dev("/dev/spidev0.0");
  auto x=dev.read(0x17);
  if(x>=0) speak16(x);
  else 
    printf("fail %d: %s\n",x,strerror(x));
  printf("OK. test register write value\n");
  
  ros::Rate loop_rate(10);
  initialize(dev);
  while(ros::ok()){
    // printf("V: ");
    speak16(dev.read(0xB));
    dev.read(8);
    dev.read(9);
    dev.read(10);
    msg.data = tmp_data;
    // printf("%d\n", msg.data);
    pub_AD7147_val.publish(msg);
    ros::spinOnce();
    loop_rate.sleep();
  };

  return 0;
};


