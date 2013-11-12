#pragma once
class Light
{
public:
	Light(int _type, float* _position, float* _color);
	~Light();

	int type;
	float position[3];
	float color[3];
};

