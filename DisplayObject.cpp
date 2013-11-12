#include "DisplayObject.h";

DisplayObject::~DisplayObject() {
	delete [] ambient, diffuse, specular;
};

Sphere::Sphere(float* _center, float _radius, float* _ambient, float* _diffuse, float* _specular, float _k_ambient, float _k_diffuse, float _k_specular, float _specular_exponent, float _refraction_index, float _k_refractive, float _k_reflective) {
	center[0] = _center[0];
	center[1] = _center[1];
	center[2] = _center[2];

	radius = _radius;

	ambient[0] = _ambient[0];
	ambient[1] = _ambient[1];
	ambient[2] = _ambient[2];

	diffuse[0] = _diffuse[0];
	diffuse[1] = _diffuse[1];
	diffuse[2] = _diffuse[2];

	specular[0] = _specular[0];
	specular[1] = _specular[1];
	specular[2] = _specular[2];

	k_ambient = _k_ambient;
	k_diffuse = _k_diffuse;
	k_specular = _k_specular;
	specular_exponent = _specular_exponent;
	refraction_index = _refraction_index;
	k_refractive = _k_refractive;
	k_reflective = _k_reflective;
}

Polygon::Polygon(float* _vertices, float* _ambient, float* _diffuse, float* _specular, float _k_ambient, float _k_diffuse, float _k_specular, float _specular_exponent, float _refraction_index, float _k_refractive, float _k_reflective) {
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			vertices[i][j] = _vertices[i * 3 + j];
		}
	}

	ambient[0] = _ambient[0];
	ambient[1] = _ambient[1];
	ambient[2] = _ambient[2];

	diffuse[0] = _diffuse[0];
	diffuse[1] = _diffuse[1];
	diffuse[2] = _diffuse[2];

	specular[0] = _specular[0];
	specular[1] = _specular[1];
	specular[2] = _specular[2];

	k_ambient = _k_ambient;
	k_diffuse = _k_diffuse;
	k_specular = _k_specular;
	specular_exponent = _specular_exponent;
	refraction_index = _refraction_index;
	k_refractive = _k_refractive;
	k_reflective = _k_reflective;
}