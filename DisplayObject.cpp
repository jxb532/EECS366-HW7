#include <stdlib.h>
#include <math.h>
#include <vector>
#include "DisplayObject.h";

DisplayObject::~DisplayObject() {
	// Nothing to delete.
};


Color* DisplayObject::calculateIntensityAtPoint(Vector3* point, Vector3* V, Vector3* N, vector<Light*> lights, int numLights) {

	float temp;
	Vector3 color (0, 0, 0);
	for (int i = 0; i < numLights; i++) {
		//if (lights[i]->type == Light::Point) {
			Vector3 lightPos (lights[i]->position);

			Vector3 L = (lightPos - *point); L.normalize();
			Vector3 R = ((*N * 2.0) * N->dot(L)) - L; R.normalize();
			//Vector3 H = L + *V; H.normalize();
		  
			Vector3 Ia (material->ambient[0] * lights[i]->color[0],
						material->ambient[1] * lights[i]->color[1],
						material->ambient[2] * lights[i]->color[2]);

			temp = N->dot(L);
			Vector3 Id (material->diffuse[0] * lights[i]->color[0] * temp,
						material->diffuse[1] * lights[i]->color[1] * temp,
						material->diffuse[2] * lights[i]->color[2] * temp);
			Id[0] = (Id[0] > 0) ? (Id[0] < 1) ? Id[0] : 1 : 0;
			Id[1] = (Id[1] > 0) ? (Id[1] < 1) ? Id[1] : 1 : 0;
			Id[2] = (Id[2] > 0) ? (Id[2] < 1) ? Id[2] : 1 : 0;

			temp = powf(R.dot(V), material->specular_exponent);
			Vector3 Is (material->specular[0] * lights[i]->color[0] * temp,
						material->specular[1] * lights[i]->color[1] * temp,
						material->specular[2] * lights[i]->color[2] * temp);

			color = color + Ia + Id + Is;
		//} else if (lights[i]->type == Light::Directional) {
			// TODO: Implement.
		//}
	}

	return new Color(color[0], color[1], color[2]);
}

Sphere::Sphere(float* _center, float _radius, Material* _material) {
	center = new Vector3(_center[0], _center[1], _center[2]);

	radius = _radius;

	material = _material;
}

Sphere::~Sphere() {
	delete center;
}


// Adapted from http://wiki.cgsociety.org/index.php/Ray_Sphere_Intersection
bool Sphere::intersects(Ray* _ray, Vector3* intersect, float &dist) {
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
		dist = (*intersect - *_ray->origin).magnitude();
        return true;

	// else the intersection point is at t0
    } else {
        *intersect = *_ray->origin + (*_ray->direction * t0);
		dist = (*intersect - *_ray->origin).magnitude();
        return true;
    }
}

Vector3* Sphere::normalAtPoint(Vector3* point) {
	Vector3 norm = *point - *this->center;
	return new Vector3(norm / norm.magnitude());
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

bool Polygon::intersects(Ray* _ray, Vector3* intersect, float &dist) {
	Vector3 n = (*v2 - *v1).cross(&(*v3 - *v1)); n.normalize();

	if (n.dot(_ray->direction) == 0) return false;
	float t = -(n.dot(_ray->origin) - n.dot(v1)) / n.dot(_ray->direction);
	if (t < 0) return false;

	*intersect = *_ray->origin + (*_ray->direction * t);
	dist = (*intersect - *_ray->origin).magnitude();

	Vector3 edge0 = *v2 - *v1;
	Vector3 edge1 = *v3 - *v2;
	Vector3 edge2 = *v1 - *v3;
	Vector3 c0 = *intersect - *v1;
	Vector3 c1 = *intersect - *v2;
	Vector3 c2 = *intersect - *v3;

	return (n.dot(&(edge0.cross(&c0))) > 0 &&
		n.dot(&(edge1.cross(&c1))) > 0 &&
		n.dot(&(edge2.cross(&c2))) > 0);
}

float polygonArea(Vector3* a, Vector3* b, Vector3* c) {
	Vector3 ab = *b - *a;
	Vector3 ac = *c - *a;
	return ab.cross(&ac).magnitude() / 2;
}

Vector3* Polygon::normalAtPoint(Vector3* point) {
	float a1 = polygonArea(point, this->v2, this->v3);
	float a2 = polygonArea(point, this->v1, this->v3);
	float a3 = polygonArea(point, this->v1, this->v2);
	float a = a1 + a2 + a3;
	Vector3* normal = &((*this->n1 * (a1/a)) + (*this->n2 * (a2/a)) + (*this->n3 * (a3/a)));
	normal = new Vector3(*normal / normal->magnitude());

	return normal;
}