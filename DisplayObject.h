#include "Material.h"
#include "Vector3.h"
#include "Ray.h"

using namespace std;

#pragma once
class DisplayObject {
public:
	virtual ~DisplayObject();
	virtual bool intersects(Ray* ray, Vector3* intersect) = 0;
	virtual Vector3* normalAtPoint(Vector3* point) = 0;

	Material* material;
};

class Sphere : public DisplayObject {
public:
	Sphere(float* _center, float _radius, Material* _material);
	~Sphere();
	bool intersects(Ray* ray, Vector3* intersect);
	Vector3* normalAtPoint(Vector3* point);

	Vector3* center;
	float radius;
};

class Polygon : public DisplayObject {
public:
	Polygon(Vector3* vertex1, Vector3* vertex2, Vector3* vertex3, Vector3* normal1, Vector3* normal2, Vector3* normal3, Material* _material);
	~Polygon();
	bool intersects(Ray* ray, Vector3* intersect);
	Vector3* normalAtPoint(Vector3* point);

	Vector3* v1;
	Vector3* v2;
	Vector3* v3;
	Vector3* n1;
	Vector3* n2;
	Vector3* n3;
};