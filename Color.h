/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 11/21/2013 - Assignment 7                  */

#pragma once
class Color
{
public:
	double r,g,b;

	Color() { r = 0; g = 0; b = 0;}
	Color(Color* _color)
	{
		r = _color->r;
		g = _color->g;
		b = _color->b;
	}
	Color(double rr, double gg, double bb)
	{
		r = rr;
		g = gg;
		b = bb;
	}
	double operator[](const int &index) {
		if (index == 0) return r;
		if (index == 1) return g;
		if (index == 2) return b;
		return 0;
	}
	Color operator*(double num)
	{
		return Color(r * num, g * num, b * num);
	}
	Color operator*(Color c)
	{
		return Color(r * c.r, g * c.g, b * c.b);
	}

	Color operator+(Color c)
	{
		return Color(c.r + r, c.g + g, c.b + b);
	}
	Color operator-(Color c)
	{
		return Color(c.r - r, c.g - g, c.b - b);
	}
};
