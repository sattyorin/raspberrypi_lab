#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

class ShowAD7147
{
	private:
		int x_num, y_num;
	public:

		ShowAD7147(const int x, const int y);
		~ShowAD7147();
		void show3DBox(uint16_t (&data)[NUMBER_OF_REGISTERS]);
};

ShowAD7147::ShowAD7147(const int x, const int y)
	:x_num(x),
	y_num(y)
{
	if (x_num * y_num != NUMBER_OF_REGISTERS)
	{
		std::cout << "faild" << std::endl;
		exit(0);
	}
}

ShowAD7147::~ShowAD7147(){}

void ShowAD7147::show3DBox(uint16_t (&data)[NUMBER_OF_REGISTERS])
{
	FILE *gp;
	gp = popen("gnuplot", "w");
	fprintf(gp, "set boxwidth 0.4 absolute\n");
	fprintf(gp, "set boxdepth 0.3\n");
	fprintf(gp, "set style fill solid 1.00 border\n");
	fprintf(gp, "set grid nopolar\n"); //like ashadow
	fprintf(gp, "set grid xtics nomxtics ytics nomytics ztics nomztics nortics nomrtics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics\n");
	fprintf(gp, "set grid vertical layerdefault lt 0 linecolor 0 linewidth 1.000,  lt 0 linecolor 0 linewidth 1.000\n");
	fprintf(gp, "unset key\n");
	fprintf(gp, "set wall z0  fc  rgb 'slategrey' fillstyle  transparent solid 0.50 border lt -1\n");
	fprintf(gp, "set view 59, 24, 1, 1\n");
	fprintf(gp, "set style data lines\n");
	fprintf(gp, "set xyplane at 0\n");
	fprintf(gp, "set title 'give the boxes a 3D depth and correct depth sorting'\n");
	fprintf(gp, "set xrange [ * : * ] noreverse writeback\n");
	fprintf(gp, "set x2range [ * : * ] noreverse writeback\n");
	fprintf(gp, "set yrange [ * : * ] noreverse nowriteback\n");
	fprintf(gp, "set y2range [ * : * ] noreverse writeback\n");
	fprintf(gp, "set zrange [ * : * ] noreverse writeback\n");
	fprintf(gp, "set cbrange [ * : * ] noreverse writeback\n");
	fprintf(gp, "set rrange [ * : * ] noreverse writeback\n");
	fprintf(gp, "set pm3d depthorder base\n");
	fprintf(gp, "set pm3d interpolate 1,1 flush begin noftriangles border linewidth 1.000 dashtype solid corners2color mean\n");
	fprintf(gp, "rgbfudge(x) = x*51*32768 + (11-x)*51*128 + int(abs(5.5-x)*510/9.)\n");

	fprintf(gp, "splot '-' using 1:2:3:(rgbfudge($1)) with boxes fc rgb variable\n");
	
	for (int i=0; i<x_num; i++)
	{
		for (int j=0; j<y_num; j++)
		{
			fprintf(gp, "%d, %d, %d\n", i+1, j+1, data[i+j]);
		}
	}
	fprintf(gp, "e\n");
	fflush(gp);

}
int main()
{
	ShowAD7147 showad7147(3, 4);
	int data[12] = {44364, 41580, 41181, 40830, 41787, 41016, 40795, 41118, 41780, 41208, 41281, 41208};

	showad7147.show3DBox(data);
	return 0;
}

