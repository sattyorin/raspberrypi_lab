#include "../include/ad7147/pub_AD7147s.hpp"

int main(int argc, char **argv){
	ros::init(argc, argv, "pub_AD7147s");
	ROS_INFO("[pub_AD7147s] init to pub_AD7147s");

	ros::NodeHandle n;
	ros::Publisher pub_AD7147_vals0 = n.advertise<std_msgs::Int32MultiArray>("AD7147_vals0", 20);
	ros::Rate loop_rate(500);

	AD7147SPIDev ad7147spidev("/dev/spidev0.0", 12, AD7147QUANTITY); // 12:gpio
	ad7147spidev.initialize();

	int i = 0;
	uint16_t data[NUMBER_OF_REGISTERS] = {0};
	std_msgs::Int32MultiArray msg;
	for (i=0; i<NUMBER_OF_REGISTERS; i++)
	{
		msg.data.push_back(0);
	}

	while(ros::ok())
	{
		ad7147spidev.getCVal(data);
		for (i=0; i<NUMBER_OF_REGISTERS; i++)
		{
			msg.data[i] = data[i];
		}
		pub_AD7147_vals0.publish(msg);
		ros::spinOnce();
		loop_rate.sleep();
	}
	return 0;
}
