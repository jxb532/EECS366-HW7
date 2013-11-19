#pragma once
#include "Color.h"
#include "Vector3.h"

class Light
{
public:
	enum Type { Directional, Point };
	Light(int _type, float* _position, float* _color);
	~Light();

	Type type;
	Vector3 position;
	Color color;
};
