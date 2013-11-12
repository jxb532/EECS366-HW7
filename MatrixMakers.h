/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 10/07/2013 - Assignment 4                  */

#include "Matrix.h"
#include "Vector3.h"

Matrix* modelMatrix(Matrix* r, Vector3* p);
Matrix* viewMatrix(Vector3* P, Vector3* N, Vector3* V);
Matrix* rotateMatrix(float thetaDeg, char axis);
Matrix* translateMatrix(float x, float y, float z);
Matrix* perspectiveMatrix(Vector3 prp, Vector3 vp, float s, float t, float d);
Matrix* perspectiveMatrix(float d, float xMin, float xMax, float yMin, float yMax, float n, float f);
Matrix* orthographicMatrix(Vector3* V_p, float xMin, float xMax, float yMin, float yMax, float n, float f);
Matrix* viewportMatrix(float xvMin, float xvMax, float yvMin, float yvMax);