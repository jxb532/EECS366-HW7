#include "point.h"
#include "Vector3.h"
#include "color.h"

using namespace std;

#pragma once
class Ray {
public:
	Ray();
	Ray(Vector3* _origin, Vector3* _direction);
	Ray(Vector3* _origin, Vector3* _direction, Color* _color);
	~Ray();
	
	Vector3* origin;
	Vector3* direction;
	Color* color;
};