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
#include <byteswap.h>
#include <arpa/inet.h>
#include <array>
#include <bitset>
#include <cmath>
//#include "wiringPi.h"
//#include "wiringPiSPI.h"
#include "../include/ad7147/AD7147regmap.h"
#include "../include/ad7147/ad7147spidev.h"
#include <tuple>
#include <unordered_map>
#include <std_msgs/Int32MultiArray.h>
using namespace std;

int AD7147QUANTITY = 6;
int tmp = 0;

template<typename X> int speak(const X& val){return speak(&val,sizeof(X)); };
void speak16(uint16_t x) { tmp = speak(x); };

void initialize(int which, ad7147spidev_multi& spi){
  // for(int i=0;i<12;i++){
  //   auto base_address=i*8+STAGE0_CONNECTION;
  //   uint16_t config[] {
  //     // <C-F> 0x80 for magic values definitions
  //     0x7FFD, 0x1FFF, 0,0x2626,0x1600,0x1600,0x1600,0x1600
  //   };
  uint16_t initialization_array[][8]{
     {0xFFFE, 0x1FFF, 0,0x2626,0x1600,0x1600,0x1600,0x1600},
     {0xFFFB, 0x1FFF, 0,0x2626,0x1600,0x1600,0x1600,0x1600},
     {0xFFEF, 0x1FFF, 0,0x2626,0x1600,0x1600,0x1600,0x1600},
     {0xFFBF, 0x1FFF, 0,0x2626,0x1600,0x1600,0x1600,0x1600},
     {0xFEFF, 0x1FFF, 0,0x2626,0x1600,0x1600,0x1600,0x1600},
     {0xFBFF, 0x1FFF, 0,0x2626,0x1600,0x1600,0x1600,0x1600},
     {0xEFFF, 0x1FFF, 0,0x2626,0x1600,0x1600,0x1600,0x1600},
     {0xFFFF, 0x1FFE, 0,0x2626,0x1600,0x1600,0x1600,0x1600},
     {0xFFFF, 0x1FFB, 0,0x2626,0x1600,0x1600,0x1600,0x1600},
     {0xFFFF, 0x1FEF, 0,0x2626,0x1600,0x1600,0x1600,0x1600},
     {0xFFFF, 0x1FBF, 0,0x2626,0x1600,0x1600,0x1600,0x1600},
     {0xFFFF, 0x1EFF, 0,0x2626,0x1600,0x1600,0x1600,0x1600},
  };
  for(int i=0; i<12; i++){
    auto base_address=i*8+STAGE0_CONNECTION;
    auto &config=initialization_array[i];
    //uint16_t config[] {
    //  0x7FFD, 0x1FFF, 0,0x2626,0x1600,0x1600,0x1600,0x1600
    //};
    auto config_size=sizeof(config);
    for(size_t j=0;j<config_size;j++){
      config[j]=htons(config[j]);
    };
    printf("writing register %d, base address %X\n",i,base_address);
    spi.write(which,base_address,&config,sizeof(config));
    };
  for(int i=0x80;i<=0xDF;i++){
    speak16(spi.read(which,i));
  };
  // READ P.40 for MAGIC NUMBERS
  uint8_t initialization_bytes[] {0x82,0xb2,0x0,0x0,0x32,0x30,0x04,0x19,0x08,0x32,0x0,0x0,0x1,0x0};
  spi.write(which,0,initialization_bytes,sizeof(initialization_bytes));
  uint16_t start_word=0xFF0F;
  spi.write(which,1,&start_word,2);
};

int main(int argc, char **argv){
  ros::init(argc, argv, "read_AD7147s");
  ROS_INFO("[read_AD7147s] init to read_AD7147s");

  ros::NodeHandle n;
  ros::Publisher pub_AD7147_vals1 = n.advertise<std_msgs::Int32MultiArray>("AD7147_vals1", 20);
  ros::Publisher pub_AD7147_vals2 = n.advertise<std_msgs::Int32MultiArray>("AD7147_vals2", 20);
  ros::Publisher pub_AD7147_vals3 = n.advertise<std_msgs::Int32MultiArray>("AD7147_vals3", 20);
  ros::Publisher pub_AD7147_vals4 = n.advertise<std_msgs::Int32MultiArray>("AD7147_vals4", 20);
  ros::Publisher pub_AD7147_vals5 = n.advertise<std_msgs::Int32MultiArray>("AD7147_vals5", 20);
  ros::Publisher pub_AD7147_vals6 = n.advertise<std_msgs::Int32MultiArray>("AD7147_vals6", 20);
//  ros::Publisher pub_AD7147_vals7 = n.advertise<std_msgs::Int32MultiArray>("AD7147_vals7", 20);
//  ros::Publisher pub_AD7147_vals8 = n.advertise<std_msgs::Int32MultiArray>("AD7147_vals8", 20);
//  ros::Publisher pub_AD7147_vals9 = n.advertise<std_msgs::Int32MultiArray>("AD7147_vals9", 20);
//  ros::Publisher pub_AD7147_vals10 = n.advertise<std_msgs::Int32MultiArray>("AD7147_vals10", 20);
//  ros::Publisher pub_AD7147_vals11 = n.advertise<std_msgs::Int32MultiArray>("AD7147_vals11", 20);
//  ros::Publisher pub_AD7147_vals12 = n.advertise<std_msgs::Int32MultiArray>("AD7147_vals12", 20);

  ros::Rate loop_rate(500);

  ad7147spidev_multi dev("/dev/spidev0.0",12,AD7147QUANTITY);  //??????12???start_bit??????which+start_bit???gpio?????????
 // for(int i=0;i<3;i++){
 //   auto x=dev.read(i,0x17);
 //     if(x>=0) speak16(x);
 //   else 
 //     printf("fail %d: %s\n",x,strerror(x));
  // };
  // printf("OK. test register write value\n");

  for(int i=0; i<AD7147QUANTITY; i++)
  {
    initialize(i, dev);
  }

while(ros::ok()){
  for(int i=0; i<AD7147QUANTITY; i++)
  {
    std_msgs::Int32MultiArray msg;
    //printf("%d\n",i);
    // auto rv=dev.read(i,0x17);
    // printf("%d:%X\n",i,rv);
    //continue;
    for(int j=0x0B; j<0x17; j++)
    {
      //printf("%d\n",j);
      auto x=dev.read(i,j);
      if(x>=0)
      {
        speak16(x);
        msg.data.push_back(tmp);
      //  printf("%d:%02X:%X\n",i,j,tmp);
      }
      else printf("fail");
      // else printf("fail %d: %s\n",zzzzzzz,strerror(zzzzzzz));
    }
    if(i==0) pub_AD7147_vals1.publish(msg);
    else if(i==1) pub_AD7147_vals2.publish(msg);
    else if(i==2)pub_AD7147_vals3.publish(msg);
    else if(i==3) pub_AD7147_vals4.publish(msg);
    else if(i==4) pub_AD7147_vals5.publish(msg);
    else if(i==5) pub_AD7147_vals6.publish(msg);
     // case 6: pub_AD7147_vals7.publish(msg); break;
     // case 7: pub_AD7147_vals8.publish(msg); break;
     // case 8: pub_AD7147_vals9.publish(msg); break;
     // case 9: pub_AD7147_vals10.publish(msg); break;
     // case 10: pub_AD7147_vals11.publish(msg); break;
     // case 11: pub_AD7147_vals12.publish(msg); break;
    ros::spinOnce();
    loop_rate.sleep();
  }
}

  /*
  initialize(dev);
  while(true){
    printf("V: ");
    speak16(dev.read(0xB));
    dev.read(8);
    dev.read(9);
    dev.read(10);
  };
*/
  return 0;
}
