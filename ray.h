#ifndef RAY_H
#define RAY_H

#include "main.cpp"
#include "color.h"

using namespace std;

class Ray {
public:
	point origin;
	Color color;

	Ray(point _origin) {
		origin = _origin;
		color = Color(0,0,0);
	}

	~Ray() {
		delete origin, color;
	}
}
#endif