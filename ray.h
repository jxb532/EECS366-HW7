#include "point.h"
#include "Vector3.h"
#include "color.h"

using namespace std;

#pragma once
class Ray {
public:
	Ray();
	Ray(point _origin, Vector3 _direction);
	~Ray();
	
	point origin;
	Vector3 direction;
	Color color;
};