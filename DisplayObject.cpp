#include <stdlib.h>
#include <math.h>
#include "DisplayObject.h";

DisplayObject::~DisplayObject() {
	//delete material;
};


// TODO: implement
Color* DisplayObject::calculateIntensityAtPoint(Vector3* point, Vector3* V, Vector3* N, Light** lights, int numLights) {
	// TODO: normalize things

	for (int i = 0; i < numLights; i++) {
			if (lights[i]->type == Light::Point) {
			Vector3 lightPos (lights[i]->position);
			Vector3 L = (lightPos - *point);
			Vector3 R = ((*N * 2.0) * N->dot(L)) - L;
			Vector3 H = L + *V;
			L = L / L.magnitude();
			R = R / R.magnitude();
			H = H / H.magnitude();
		  
			// vec3 Ia = pAmbientMat * gl_FrontLightProduct[light].ambient.xyz;
			
			// vec3 Id = diffuseMat * gl_FrontLightProduct[light].diffuse.xyz * NDotL;
			// Id = clamp(Id, 0.0, 1.0);
			// vec3 Is = specularMat * gl_FrontLightProduct[light].specular.xyz * pow(RDotV, specularPower);
			// gl_FragColor = vec4((Ia + Id + Is), 1.0);
		}
	}

	// combine colors

	// XXX
	return NULL;
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
bool Sphere::intersects(Ray* _ray, Vector3* intersect, float* dist) {
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
		*dist = (*intersect - *_ray->origin).magnitude();
        return true;

	// else the intersection point is at t0
    } else {
        *intersect = *_ray->origin + (*_ray->direction * t0);
		*dist = (*intersect - *_ray->origin).magnitude();
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

bool Polygon::intersects(Ray* _ray, Vector3* intersect, float* dist) {
	Vector3 n = (*n2 - *n1).cross(&(*n3 - *n1));

	if (n.dot(_ray->direction) == 0) return false;
	float t = (n.dot(_ray->origin) + n.dot(n1)) / n.dot(_ray->direction);
	if (t < 0) return false;

	intersect = &(*_ray->origin + (*_ray->direction * t));
	*dist = (*intersect - *_ray->origin).magnitude();

	Vector3 edge0 = *v2 - *v1;
	Vector3 edge1 = *v3 - *v2;
	Vector3 edge2 = *v1 - *v3;
	Vector3 c0 = *intersect - *v1;
	Vector3 c1 = *intersect - *v2;
	Vector3 c2 = *intersect - *v3;

	return (n.dot(&(edge0.cross(&c0))) > 0 &&
		n.dot(&(edge1.cross(&c1))) > 0 &&
		n.dot(&(edge1.cross(&c1))) > 0);
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
	*normal = *normal / normal->magnitude();

	return normal;
}