/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 10/07/2013 - Assignment 4                  */

#include <stdio.h>
#include <math.h>
#include "Vector3.h"

Vector3::Vector3(void) {
	this->vector[0] = 0;
	this->vector[1] = 0;
	this->vector[2] = 0;
}

Vector3::Vector3(float x, float y, float z) {
	this->vector[0] = x;
	this->vector[1] = y;
	this->vector[2] = z;
}

Vector3::~Vector3(void)
{
	// shouldn't need to deallocate anything.
}

Vector3 Vector3::operator*(float s) {
	return Vector3(
		this->vector[0] * s,
		this->vector[1] * s,
		this->vector[2] * s);
}

Vector3 Vector3::operator/(float s) {
	return Vector3(
		this->vector[0] / s,
		this->vector[1] / s,
		this->vector[2] / s);
}

Vector3 Vector3::operator+(Vector3 &v) {
	return Vector3(
		this->vector[0] + v.vector[0],
		this->vector[1] + v.vector[1],
		this->vector[2] + v.vector[2]);
}

Vector3 Vector3::operator-(Vector3 &v) {
	return Vector3(
		this->vector[0] - v.vector[0],
		this->vector[1] - v.vector[1],
		this->vector[2] - v.vector[2]);
}

float&  Vector3::operator[](const int index) {
	return this->vector[index];
}

Vector3 Vector3::cross(Vector3* v) {
	return Vector3(
		this->vector[1]*v->vector[2]-this->vector[2]*v->vector[1],
		this->vector[2]*v->vector[0]-this->vector[0]*v->vector[2],
		this->vector[0]*v->vector[1]-this->vector[1]*v->vector[0]);
}

float Vector3::dot(Vector3* v) {
	return this->vector[0] * v->vector[0] +
		this->vector[1] * v->vector[1] +
		this->vector[2] * v->vector[2];
}

float Vector3::magnitude() {
	return sqrtf(
		this->vector[0] * this->vector[0] +
		this->vector[1] * this->vector[1] +
		this->vector[2] * this->vector[2]);
}

void Vector3::print() {
	printf("(%.2f, %.2f, %.2f)",
		this->vector[0],
		this->vector[1],
		this->vector[2]);
}

void Vector3::print(char* name) {
	printf("%f: (%.2f, %.2f, %.2f)", name,
		this->vector[0],
		this->vector[1],
		this->vector[2]);
}
