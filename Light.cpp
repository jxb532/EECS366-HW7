#include "Light.h"


Light::Light(int _type, float* _position, float* _color) {
	type = _type;
	
	position[0] = _position[0];
	position[1] = _position[1];
	position[2] = _position[2];

	color[0] = _color[0];
	color[1] = _color[1];
	color[2] = _color[2];
}


Light::~Light() {
	delete [] position, color;
}
