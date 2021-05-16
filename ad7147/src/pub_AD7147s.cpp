#include "pub_AD7147s.hpp"

int main(int argc, char **argv){
	ros::init(argc, argv, "read_AD7147s");
	ROS_INFO("[read_AD7147s] init to read_AD7147s");

	ros::NodeHandle n;
	ros::Publisher pub_AD7147_vals0 = n.advertise<std_msgs::Int16MultiArray>("AD7147_vals0", 20);
	ros::Rate loop_rate(500);

	AD7147SPIDev ad7147spidev("/dev/spidev0.0", 12, AD7147QUANTITY); // 12:gpio
	ad7147spidev.initialize();
	
	// uint16_t data[NUMBER_OF_REGISTERS] = {0};
	std_msgs::Int16MultiArray msg[NUMBER_OF_REGISTERS];
	while(ros::ok())
	{
		ad7147spidev.getCVal(msg);
		// msg.data.push_back(data[]);
		pub_AD7147_vals1.publish(msg);
		ros::spinOnce();
		loop_rate.sleep();
	}
	return 0;
}