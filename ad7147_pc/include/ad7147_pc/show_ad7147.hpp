#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#define NUMBER_OF_REGISTERS 12
#define AD7147QUANTITY 2

class ShowAD7147
{
	private:
		int x_num, y_num, z_range;
		FILE *gp;
	public:

		ShowAD7147(const int x, const int y, const int z_range);
		~ShowAD7147();
		void show3DBox(uint16_t (&data)[NUMBER_OF_REGISTERS]);
};