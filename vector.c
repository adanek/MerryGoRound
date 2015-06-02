#include "vector.h"

struct vec3 {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

GLfloat det3x3(GLfloat* x, GLfloat* y, GLfloat* z){
	return x[0] * y[1] * z[2] + x[1] * y[2] * z[0] + x[2] * y[0] * z[1]
		- x[2] * y[1] * z[0] - x[0] * y[2] * z[1] - x[1] * y[0] * z[2];
}

struct vec3 crossProduct3(GLfloat* v1, GLfloat* v2){

	struct vec3 result;
	result.x = v1[1] * v2[2] - v1[2] * v2[1];
	result.y = v1[2] * v2[0] - v1[0] * v2[2];
	result.z = v1[0] * v2[1] - v1[1] * v2[0];

	return result;
}

void addVector3(GLfloat* buf, struct vec3 v){
	buf[0] += v.x;
	buf[1] += v.y;
	buf[2] += v.z;
}

void calculateNormals(GLushort* ida, GLfloat* vba, GLfloat* nba, int len){

	for (int t = 0; t < len; t++){

		GLushort a = ida[t * 3];
		GLushort b = ida[t * 3 + 1];
		GLushort c = ida[t * 3 + 2];

		GLfloat* A = &vba[a * 3];
		GLfloat* B = &vba[b * 3];
		GLfloat* C = &vba[c * 3];

		// triangeles must have the same orientation
		if (det3x3(A, B, C)< 0){

			puts("Switching orientation");
			GLfloat* tmp = B;
			B = C;
			C = tmp;
		}

		// Calculate vectors AB and AC
		GLfloat v1[] = { B[0] - A[0], B[1] - A[1], B[2] - A[2] };
		GLfloat v2[] = { C[0] - A[0], C[1] - A[1], C[2] - A[2] };

		// Calculate cross product.
		struct vec3 n = crossProduct3(v1, v2);
		//GLfloat Nx = v1[1] * v2[2] - v1[2] * v2[1];
		//GLfloat Ny = v1[2] * v2[0] - v1[0] * v2[2];
		//GLfloat Nz = v1[0] * v2[1] - v1[1] * v2[0];


		GLfloat* Na = &vba[a * 3];
		Na[0] = Na[0] + n.x;
		Na[1] = Na[1] + n.y;
		Na[2] = Na[2] + Nz;

		GLfloat* Nb = &vba[b * 3];
		Nb[0] = Nb[0] + Nx;
		Nb[1] = Nb[1] + Ny;
		Nb[2] = Nb[2] + Nz;

		GLfloat* Nc = &vba[c * 3];
		Nc[0] = Nc[0] + Nx;
		Nc[1] = Nc[1] + Ny;
		Nc[2] = Nc[2] + Nz;

		printf("(%4.2f, %4.2f, %4.2f) * (%4.2f, %4.2f, %4.2f) = (%4.2f, %4.2f, %4.2f)\n", v1[0], v1[1], v1[2], v2[0], v2[1], v2[2], Nx, Ny, Nz);


}