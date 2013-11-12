
using namespace std;

#pragma once
class DisplayObject {
public:
	virtual ~DisplayObject();

	float ambient[3];
	float diffuse[3];
	float specular[3];
	float k_ambient;
	float k_diffuse;
	float k_specular;
	float specular_exponent;
	float refraction_index;
	float k_refractive;
	float k_reflective;
};

class Sphere : public DisplayObject {
public:
	Sphere(float* _center, float _radius, float* _ambient, float* _diffuse, float* _specular, float _k_ambient, float _k_diffuse, float _k_specular, float _specular_exponent, float _refraction_index, float _k_reflective, float _k_refractive);
	float center [3];
	float radius;
};

class Polygon : public DisplayObject {
public:
	Polygon(float* _vertices, float* _ambient, float* _diffuse, float* _specular, float _k_ambient, float _k_diffuse, float _k_specular, float _specular_exponent, float _refraction_index, float _k_reflective, float _k_refractive);
	// TODO remember to scale and rotate these before construction
	float vertices[3][3];

};