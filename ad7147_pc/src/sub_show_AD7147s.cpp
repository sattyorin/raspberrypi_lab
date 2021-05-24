#include <ros/ros.h>
#include <std_msgs/Int32MultiArray.h>

#include "../include/ad7147_pc/show_ad7147.hpp"

ShowAD7147 showad7147_0(3, 4, 4000);
ShowAD7147 showad7147_1(3, 4, 4000);
uint16_t data[NUMBER_OF_REGISTERS] = {0};
uint32_t offset[NUMBER_OF_REGISTERS*AD7147QUANTITY] = {0};
bool flag = false;
int num[AD7147QUANTITY] = {0};
int32_t tmp = 0;

void Callback(uint8_t which, const std_msgs::Int32MultiArray& msg)
{
	if (flag == false)
	{
		num[which]++;
		for (int i=0; i<NUMBER_OF_REGISTERS; i++)
		{
			offset[i+which*NUMBER_OF_REGISTERS] += msg.data[i];
		}
		if (num[which] > 100) //offset = uint32_t
		{
			for (int i=0; i<NUMBER_OF_REGISTERS; i++)
			{
				// offset[i+which*NUMBER_OF_REGISTERS] = offset[i+which*NUMBER_OF_REGISTERS]/(n+1);
				offset[i+which*NUMBER_OF_REGISTERS] = offset[i+which*NUMBER_OF_REGISTERS]/(num[which]+1) - 500;
				printf("%d : %d : %d\n", which, i, offset[i+which*NUMBER_OF_REGISTERS]);
			}
			flag = true;
		}
	}

	else
	{
		for (int i=0; i<NUMBER_OF_REGISTERS; i++)
		{
			tmp = msg.data[i] - offset[i+which*NUMBER_OF_REGISTERS];
			if (tmp > 0 && abs(tmp - data[i]) < 20000)
			{
				data[i] = tmp;
				std::cout << i << ":" << data[i] << std::endl;
			}
			else {data[i] = 0;}
		}
		// showad7147.show3DBox(data);
	}
}

void Callback0(const std_msgs::Int32MultiArray& msg)
{
	Callback(0, msg);
	showad7147_0.show3DBox(data);
}


void Callback1(const std_msgs::Int32MultiArray& msg)
{
	Callback(1, msg);
	showad7147_1.show3DBox(data);
}


int main(int argc, char **argv){
	ros::init(argc, argv, "sub_show_AD7147s");
	ROS_INFO("[sub_show_AD7147s] init to sub_show_AD7147s");

	ros::NodeHandle n;
	ros::Subscriber sub = n.subscribe("AD7147_vals0", 2, Callback0);
	ros::Subscriber sub = n.subscribe("AD7147_vals1", 2, Callback1);
	ros::Rate loop_rate(10);

	while(ros::ok())
	{
		ros::spinOnce();
		loop_rate.sleep();
	}
	return 0;
}
