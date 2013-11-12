/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 10/07/2013 - Assignment 4                  */

#define LENGTH 3

#pragma once
class Vector3 {
public:
	        Vector3(void);
		    Vector3(float x, float y, float z);
	        ~Vector3(void);

	Vector3 operator*(float s);
	Vector3 operator/(float s);
	Vector3 operator+(Vector3 &v);
	Vector3 operator-(Vector3 &v);
	float&  operator[](const int index);

	Vector3 cross(Vector3* v);
	float   dot(Vector3* v);
	float   magnitude();

	void    print();
	void    print(char* name);
	float   vector [LENGTH];
};