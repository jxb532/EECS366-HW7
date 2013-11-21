#pragma once
class Material
{
public:
	Material();
	Material( float* _ambient, float* _diffuse, float* _specular, float _k_ambient, float _k_diffuse, float _k_specular, float _specular_exponent, float _refraction_index, float _k_reflective, float _k_refractive);
	~Material(void);

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

