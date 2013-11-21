/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 11/21/2013 - Assignment 7                  */

#pragma once
#include "Color.h"
#include "Vector3.h"

class Light
{
public:
	enum Type { Directional, Point };
	Light(int _type, float* _position, float* _color);
	~Light();

	Type type;
	Vector3 position;
	Color color;
};
