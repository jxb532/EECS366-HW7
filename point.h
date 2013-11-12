
#pragma once
class point
{
public:
	double x,y,z,w;

	point(){ x = 0; y = 0; z = 0; w = 1;}
	point(double xa, double ya, double za)
	{
		x = xa; y = ya; z = za; w = 1.0;
	}
	point(double xa, double ya, double za, double wa)
	{
		x = xa; y = ya; z = za; w = wa;
	}
};