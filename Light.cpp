/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 11/21/2013 - Assignment 7                  */

#include "Light.h"

Light::Light(int _type, float* _position, float* _color) {
	type = (_type == 0) ? Light::Directional : Light::Point;
	position = *new Vector3(_position[0], _position[1], _position[2]);
	color = *new Color(_color[0], _color[1], _color[2]);
}

Light::~Light() {
	delete &position, &color;
}
