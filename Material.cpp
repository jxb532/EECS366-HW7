#include "Material.h"

Material::Material() {
	ambient[0] = 0;
	ambient[1] = 0;
	ambient[2] = 0;

	diffuse[0] = 0;
	diffuse[1] = 0;
	diffuse[2] = 0;

	specular[0] = 0;
	specular[1] = 0;
	specular[2] = 0;

	k_ambient = 0;
	k_diffuse = 0;
	k_specular = 0;
	specular_exponent = 0;
	refraction_index = 0;
	k_reflective = 0;
	k_refractive = 0;
}

Material::Material( float* _ambient, float* _diffuse, float* _specular, float _k_ambient, float _k_diffuse, float _k_specular, float _specular_exponent, float _refraction_index, float _k_reflective, float _k_refractive)
{
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
	k_reflective = _k_reflective;
	k_refractive = _k_refractive;
}


Material::~Material(void)
{
	delete [] ambient, diffuse, specular;
}
