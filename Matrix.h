/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 10/07/2013 - Assignment 4                  */

#pragma once
#include "Vector3.h"

class Matrix {
public:
	float** matrix;
	int     rows;
	int     cols;

	        Matrix(void);
	        Matrix(int _rows, int _cols);
			Matrix(int _rows, int _cols, float** _matrix);
			Matrix(int _rows, int _cols, float* _matrix);
			Matrix(Vector3 &vector, int extraRows);
	        ~Matrix(void);

	float   get(int i, int j);
	void    set(int i, int j, float value);
	void    append(float* row, float* col);

	Matrix* operator*(Matrix* m);
	Matrix* operator*(float s);
	Matrix* operator/(float s);
	float*  operator[](const int index);

	Matrix* transpose();

	void    print(char* name);
	float*  toArray();
	Vector3* toVector3();
};