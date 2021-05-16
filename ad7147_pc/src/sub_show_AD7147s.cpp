#include <ros/ros.h>
#include <std_msgs/Int32MultiArray.h>

#include "../include/ad7147_pc/show_ad7147.hpp"

ShowAD7147 showad7147(3, 4, 2500);
uint16_t data[NUMBER_OF_REGISTERS] = {0};
uint32_t offset[NUMBER_OF_REGISTERS] = {0};
bool flag = false;
int n = 0;
int32_t tmp = 0;

void Callback(const std_msgs::Int32MultiArray& msg)
{
	if (flag == false)
	{
		n++;
		for (int i=0; i<NUMBER_OF_REGISTERS; i++)
		{
			offset[i] += msg.data[i];
		}
		if (n > 100) //false = uint32_t
		{
			for (int i=0; i<NUMBER_OF_REGISTERS; i++)
			{
				offset[i] = offset[i]/n;
			}
			flag = true;
		}
	}

	else
	{
		for (int i=0; i<NUMBER_OF_REGISTERS; i++)
		{
			tmp = msg.data[i] - offset[i];
			if (tmp > 0) 
			{
				data[i] = tmp;
				std::cout << i << ":" << data[i] << std::endl;
			}
			else {data[i] = 0;}
		}
		showad7147.show3DBox(data);
	}
}

int main(int argc, char **argv){
	ros::init(argc, argv, "sub_show_AD7147s");
	ROS_INFO("[sub_show_AD7147s] init to sub_show_AD7147s");

	ros::NodeHandle n;
	ros::Subscriber sub = n.subscribe("AD7147_vals0", 2, Callback);
	ros::Rate loop_rate(10);

	while(ros::ok())
	{
		ros::spinOnce();
		loop_rate.sleep();
	}
	return 0;
}
