#include <stdlib.h>
#include <math.h>
#include "DisplayObject.h";

DisplayObject::~DisplayObject() {
	//delete material;
};

Sphere::Sphere(float* _center, float _radius, Material* _material) {
	center = new Vector3(_center[0], _center[1], _center[2]);

	radius = _radius;

	material = _material;
}

Sphere::~Sphere() {
	delete center;
}


// Adapted from http://wiki.cgsociety.org/index.php/Ray_Sphere_Intersection
bool Sphere::intersects(Ray* _ray, Vector3* intersect) {
	// transform ray into object space
	Vector3 rayOrigin = *_ray->origin - *this->center;
	Ray ray = Ray(&rayOrigin, _ray->direction);

	//Compute A, B and C coefficients
	float a = ray.direction->dot(ray.direction);
	float b = 2 * ray.direction->dot(ray.origin);
	float c = ray.origin->dot(ray.origin) - (this->radius * this->radius);

    //Find discriminant
    float disc = b * b - 4 * a * c;
    
    // if discriminant is negative there are no real roots, so return 
    // false as ray misses sphere
    if (disc < 0) {
        return false;
	}

    // compute q as described above
    float distSqrt = sqrtf(disc);
    float q;
    if (b < 0)
        q = (-b - distSqrt)/2.0;
    else
        q = (-b + distSqrt)/2.0;

    // compute t0 and t1
    float t0 = q / a;
    float t1 = c / q;

    // make sure t0 is smaller than t1
    if (t0 > t1) {
        // if t0 is bigger than t1 swap them around
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }

    // if t1 is less than zero, the object is in the ray's negative direction
    // and consequently the ray misses the sphere
    if (t1 < 0) {
        return false;
	}

    // if t0 is less than zero, the intersection point is at t1
    if (t0 < 0) {
		*intersect = *_ray->origin + (*_ray->direction * t1);
        return true;

	// else the intersection point is at t0
    } else {
        *intersect = *_ray->origin + (*_ray->direction * t0);
        return true;
    }
}

Vector3* Sphere::normalAtPoint(Vector3* point) {
	Vector3 norm = *point - *this->center;
	return &(norm / norm.magnitude());
}

Polygon::Polygon(Vector3* vertex1, Vector3* vertex2, Vector3* vertex3, Vector3* normal1, Vector3* normal2, Vector3* normal3, Material* _material) {
	
	v1 = new Vector3(vertex1);
	v2 = new Vector3(vertex2);
	v3 = new Vector3(vertex3);
	n1 = new Vector3(normal1);
	n2 = new Vector3(normal2);
	n3 = new Vector3(normal3);
	material = _material;
}

Polygon::~Polygon() {
	delete v1, v2, v3, n1, n2, n3;
}

// TODO implement
bool Polygon::intersects(Ray* _ray, Vector3* intersect) {
	return false;
}

// TODO implement
Vector3* Polygon::normalAtPoint(Vector3* point) {
	return NULL;
}