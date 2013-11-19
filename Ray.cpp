#include "Ray.h"

Ray::Ray() {
		origin = new Vector3(0.0, 0.0, 0.0);
		direction = new Vector3(0.0, 0.0, -1.0);
		color = new Color(0.0, 0.0, 0.0);
}

Ray::Ray(Vector3* _origin, Vector3* _direction) {
		origin = new Vector3(*_origin);
		direction = &(*_direction / _direction->magnitude());
		color = new Color(0.0, 0.0, 0.0);
}

Ray::Ray(Vector3* _origin, Vector3* _direction, Color* _color) {
		origin = new Vector3(*_origin);
		direction = &(*_direction / _direction->magnitude());
		color = new Color(_color);
}

Ray::~Ray() {
		
}