#include "Ray.h"

Ray::Ray() {
		origin = point(0,0,0);
		direction = Vector3(1,1,1);
		color = Color();
}

Ray::Ray(point _origin, Vector3 _direction) {
		origin = _origin;
		direction = _direction;
		color = Color();
}

Ray::~Ray() {
		
}