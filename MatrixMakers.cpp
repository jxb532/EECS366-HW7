/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 10/07/2013 - Assignment 4                  */

#include "MatrixMakers.h"
#include "Matrix.h"
#include "Vector3.h"
#include <math.h>

#define X 0
#define Y 1
#define Z 2
#define H 3

Matrix* modelMatrix(Matrix* r, Vector3* p) {
	Matrix* temp = new Matrix(r->rows + 1, r->cols + 1);
	for (int i = 0; i < r->rows; ++i) {
		int j;
		for (j = 0; j < r->cols; ++j) {
			temp->set(i, j, r->get(i, j));
		}
		temp->set(i, j, p->vector[i]);
	}
	temp->set(temp->rows - 1, temp->cols - 1, 1);
	return temp;
}

Matrix* viewMatrix(Vector3* P, Vector3* N, Vector3* V) {
	Matrix *m = new Matrix(4,4);

	Vector3 n = *N * (-1.0 / N->magnitude());
	Vector3 u = (V->cross(&(*N * -1.0))) * (1 / ((V->cross(N)).magnitude()));
	Vector3 v = n.cross(&u);

    for (int i = 0; i < 3; ++i)
    {
		m->set(0, i, u.vector[i]);
		m->set(1, i, v.vector[i]);
		m->set(2, i, n.vector[i]);
		m->set(3, i, 0);
    }

	m->set(0, 3, -1.0 * u.dot(P));
	m->set(1, 3, -1.0 * v.dot(P));
	m->set(2, 3, -1.0 * n.dot(P));
	m->set(3, 3, 1.0);

    return m;
}

Matrix* rotateMatrix(float thetaDeg, char axis) {
	float thetaRad = thetaDeg * (3.14159 / 180.0);
	float matrixX[9] = {1, 0, 0, 0, cos(thetaRad), sin(thetaRad), 0, -1.0 * sin(thetaRad), cos(thetaRad)};
	float matrixY[9] = {cos(thetaRad), 0, -1.0 * sin(thetaRad), 0, 1, 0, sin(thetaRad), 0, cos(thetaRad)};
	float matrixZ[9] = {cos(thetaRad), sin(thetaRad), 0, -1.0 * sin(thetaRad), cos(thetaRad), 0, 0, 0, 1};

	switch (axis) {
	case 'x':
	case 'X':
		return new Matrix(3, 3, matrixX);
	case 'y':
	case 'Y':
		return new Matrix(3, 3, matrixY);
	case 'z':
	case 'Z':
		return new Matrix(3, 3, matrixZ);
	default:
		return new Matrix(3, 3);
	}
}

Matrix* translateMatrix(float x, float y, float z) {
	float matrix[3] = {x, y, z};
	return new Matrix(3, 1, matrix);
}

Matrix* perspectiveMatrix(Vector3 prp, Vector3 vp, float s, float t, float d) {
	float matrix[] = {
		prp[Z] - vp[Z], 0, 0, 0,
		0, prp[Z] - vp[Z], 0, 0,
		-prp[X], -prp[Y], s, -1 / d,
		prp[X] * prp[Z], prp[X] * prp[Z], t, prp[Z] / d
	};
	
	return new Matrix(4, 4, matrix);
}

Matrix* perspectiveMatrix(float d, float xMin, float xMax, float yMin, float yMax, float n, float f) {
	
	float per_1a[] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		(xMax + xMin) / (2 * d), (yMax + yMin) / (2 * d), 1, 0,
		0, 0, 0, 1
	};

	float per_1b[] = {
		(2 * d) / (xMax - xMin), 0, 0, 0,
		0, (2 * d) / (yMax - yMin), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	float per_2[] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, -(f + n) / (f - n), -1,
		0, 0, -(2 * f * n) / (f - n), 0
	};

	Matrix* t1 = new Matrix(4, 4, per_1a);
	Matrix* t2 = new Matrix(4, 4, per_1b);
	Matrix* t3 = new Matrix(4, 4, per_2);

	Matrix* t4 = *t3 * t2;
	Matrix* t5 = *t4 * t1;
	delete t1, t2, t3, t4;

	return t5;
}

Matrix* orthographicMatrix(Vector3* V_p, float xMin, float xMax, float yMin, float yMax, float n, float f) {
	// equvalent to M_parallel in notes
	float ortho_1[] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		(-V_p->vector[0]/V_p->vector[2]), (-V_p->vector[1]/V_p->vector[2]), 1, 0,
		0, 0, 0, 1
	};

	float ortho_2[] = {
		2 / (xMax-xMin), 0, 0, 0,
		0, 2 / (yMax - yMin), 0, 0,
		0, 0, -2 / (f - n), 0,
		-(xMax + xMin)/(xMax - xMin), -(yMax + yMin)/(yMax - yMin), -(f + n)/(f - n), 1
	};

	Matrix* t1 = new Matrix(4, 4, ortho_1);
	Matrix* t2 = new Matrix(4, 4, ortho_2);

	Matrix* t3 = *t2 * t1;
	delete t1, t2;
	
	return t3;
}

Matrix* viewportMatrix(float xvMin, float xvMax, float yvMin, float yvMax) {
	float matrix[] = {
		(xvMax - xvMin) / 2, 0, 0, 0,
		0, (yvMax - yvMin) / 2, 0, 0,
		0, 0, 0.5, 0,
		(xvMax + xvMin) / 2, (yvMax + yvMin) / 2, 0.5, 1
	};

	return new Matrix(4, 4, matrix);
}
