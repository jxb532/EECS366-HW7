/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 11/21/2013 - Assignment 7                  */

#include "Ray.h"

Ray::Ray() {
		origin = new Vector3(0.0, 0.0, 0.0);
		direction = new Vector3(0.0, 0.0, -1.0);
		color = new Color(0.0, 0.0, 0.0);
}

Ray::Ray(Vector3* _origin, Vector3* _direction) {
		origin = new Vector3(*_origin);
		direction = new Vector3(*_direction / _direction->magnitude());
		color = new Color(0.0, 0.0, 0.0);
}

Ray::Ray(Vector3* _origin, Vector3* _direction, Color* _color) {
		origin = new Vector3(*_origin);
		direction = new Vector3(*_direction / _direction->magnitude());
		color = new Color(_color);
}

Ray::~Ray() {
		
}