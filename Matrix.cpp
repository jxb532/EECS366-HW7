/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 10/07/2013 - Assignment 4                  */

#include <stdlib.h>
#include <stdio.h>
#include "Matrix.h"

Matrix::Matrix() {
	rows = 0;
	cols = 0;
	matrix = 0;
}

Matrix::Matrix(int _rows, int _cols) {
	rows = _rows;
	cols = _cols;
	matrix = new float*[rows];
	for (int i = 0; i < rows; ++i) {
		matrix[i] = new float[cols];
		for (int j = 0; j < cols; ++j) {
			matrix[i][j] = (i == j) ? 1 : 0;
		}
	}
}

// _matrix should be in COLUMN MAJOR
Matrix::Matrix(int _rows, int _cols, float** _matrix) {
	rows = _rows;
	cols = _cols;
	matrix = new float*[rows];
	for (int i = 0; i < rows; ++i) {
		matrix[i] = new float[cols];
		for (int j = 0; j < cols; ++j) {
			matrix[i][j] = _matrix[i][j];
		}
	}
}

Matrix::Matrix(int _rows, int _cols, float* _matrix) {
	rows = _rows;
	cols = _cols;
	matrix = new float*[rows];
	for (int i = 0; i < rows; ++i) {
		matrix[i] = new float[cols];
		for (int j = 0; j < cols; ++j) {
			matrix[i][j] = _matrix[i + cols * j];
		}
	}
}

Matrix::~Matrix(void) {
	for (int i = 0; i < rows; ++i) {
		if (matrix[i]) {
			delete [] matrix[i];
			matrix[i] = NULL;
		}
	}
	if (matrix) {
		delete [] matrix;
		matrix = NULL;
	}
}

float Matrix::get(int row, int col) {
	return matrix[row][col];
}

void Matrix::set(int row, int col, float value) {
	matrix[row][col] = value;
}

void Matrix::append(float* row, float* col) {
	bool newrow = row != 0;
	bool newcol = col != 0;
	float** temp = new float*[this->rows + newrow ? 0 : 1];

	for (int i = 0; i < this->rows; ++i) {
		temp[i] = new float[this->cols + newcol ? 0 : 1];
		for (int j = 0; j < this->cols; ++j) {
			temp[i][j] = this->matrix[i][j];
		}
		if (newcol) temp[i][this->cols] = col[i];
	}

	if (newrow) {
		for (int j = 0; j < this->cols; ++j) {
			temp[this->rows][j] = row[j];
		}
		if (newcol) temp[this->rows][this->cols] = row[this->cols];
	}

	this->rows += newrow ? 0 : 1;
	this->cols += newcol ? 0 : 1;
}

Matrix* Matrix::operator*(Matrix* m) {
	if (this->cols != m->rows) return 0;
	Matrix * temp = new Matrix(this->rows, m->cols);
	for (int i = 0; i < temp->rows; ++i) {
		for (int j = 0; j < temp->cols; ++j) {
			temp->matrix[i][j] = 0;
			for (int k = 0; k < m->rows; ++k) {
				temp->matrix[i][j] += this->matrix[i][k] * m->matrix[k][j];
			}
		}
	}
	return temp;
}

Matrix* Matrix::operator*(float s) {
	Matrix* temp = new Matrix(this->rows, this->cols);
	for (int i = 0; i < temp->rows; ++i) {
		for (int j = 0; j < temp->cols; ++j) {
			temp->matrix[i][j] *= s;
		}
	}
	return temp;
}

Matrix* Matrix::operator/(float s) {
	Matrix* temp = new Matrix(this->rows, this->cols);
	for (int i = 0; i < temp->rows; ++i) {
		for (int j = 0; j < temp->cols; ++j) {
			temp->matrix[i][j] /= s;
		}
	}
	return temp;
}

float* Matrix::operator[](const int index) {
	return this->matrix[index];
}

Matrix* Matrix::transpose() {
	Matrix* temp = new Matrix(this->rows, this->cols);
	for (int i = 0; i < temp->rows; ++i) {
		for (int j = 0; j < temp->cols; ++j) {
			temp->matrix[i][j] = this->matrix[j][i];
		}
	}
	return temp;
}

void Matrix::print(char* name) {
	printf("Matrix %s\n", name);
	for (int i = 0; i < this->rows; ++i) {
		for (int j = 0; j < this->cols; ++j) {
			printf("%.2f\t", this->matrix[i][j]);
		}
		printf("\n");
	}
}

float* Matrix::toArray() {
	float* temp = new float[this->rows * this->cols];
	for (int i = 0; i < this->rows; ++i) {
		for (int j = 0; j < this->cols; ++j) {
			temp[i + this->rows * j] = this->matrix[i][j];
		}
	}
	return temp;
}
