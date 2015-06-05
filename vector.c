#include "vector.h"


struct vec3 vec3(GLfloat* v){
	struct vec3 result;
	result.x = v[0];
	result.y = v[1];
	result.z = v[2];

	return result;
}


struct vec3 add(struct vec3 v1, struct vec3 v2){
	struct vec3 res;
	res.x = v1.x + v2.x;
	res.y = v1.y + v2.y;
	res.z = v1.z + v2.z;

	return res;
}

GLfloat dot3(struct vec3 v1, struct vec3 v2){
	GLfloat res = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return res;
}


GLfloat len3(struct vec3 v){
	GLfloat res = sqrtf(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
	return res;
}

struct vec3 normalize3(struct vec3 v){
	struct vec3 res;
	GLfloat len = len3(v);

	res.x = v.x / len;
	res.y = v.y / len;
	res.z = v.z / len;

	return res;
}

GLfloat det3x3(GLfloat* x, GLfloat* y, GLfloat* z){
	return x[0] * y[1] * z[2] + x[1] * y[2] * z[0] + x[2] * y[0] * z[1]
		- x[2] * y[1] * z[0] - x[0] * y[2] * z[1] - x[1] * y[0] * z[2];
}

struct vec3 crossProduct3(GLfloat* v1, GLfloat* v2){

	struct vec3 result;
	result.x = 0;
	result.y = 0;
	result.z = 0;

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

void calculateNormals(GLushort* ida, GLfloat* vba, GLfloat* nba, int idxs, int vdxs){

	if (idxs % 3 != 0){
		printf("ERROR: Invalid length of index array (%d)\n", idxs);
		return;
	}

	idxs /= 3;

	for (int t = 0; t < idxs; t++){

		GLushort a = ida[t * 3];
		GLushort b = ida[t * 3 + 1];
		GLushort c = ida[t * 3 + 2];

		GLfloat* A = &vba[a * 3];
		GLfloat* B = &vba[b * 3];
		GLfloat* C = &vba[c * 3];

		/*
		// triangeles must have the same orientation
		if (det3x3(A, B, C) > 0){

			printf("Wrong orientation: triangle %d: %d %d %d\n", t, a, b, c);
			//GLfloat* tmp = B;
			//B = C;
			//C = tmp;
		}
		*/

		// Calculate vectors AB and AC
		GLfloat v1[] = { B[0] - A[0], B[1] - A[1], B[2] - A[2] };
		GLfloat v2[] = { C[0] - A[0], C[1] - A[1], C[2] - A[2] };

		// Calculate cross product.
		struct vec3 n = crossProduct3(v1, v2);

		addVector3(&nba[a * 3], n);
		addVector3(&nba[b * 3], n);
		addVector3(&nba[c * 3], n);

		printf("Normal of triangle %d: %5.2f %5.2f %5.2f\n", t, n.x, n.y, n.z);
	}

	normailize(nba, vdxs);
}

GLfloat norm3(GLfloat* vec){
	GLfloat x2 = pow(vec[0], 2);
	GLfloat y2 = pow(vec[1], 2);
	GLfloat z2 = pow(vec[2], 2);

	return sqrtf(x2 + y2 + z2);
}

void normailize(GLfloat* nba, int len){
		
	len /= 3;

	for (int i = 0; i < len; i++){

		GLfloat x = nba[i * 3];
		GLfloat y = nba[i * 3 + 1];
		GLfloat z = nba[i * 3 + 2];

		GLfloat n = norm3(&nba[i * 3]);

		//printf("Normal of vertex %d: %5.2f %5.2f %5.2f\n", i, nba[i * 3 + 0], nba[i * 3 + 1], nba[i * 3 + 2]);

		nba[i * 3 + 0] = x / n;
		nba[i * 3 + 1] = y / n;
		nba[i * 3 + 2] = z / n;

		printf("Normal of vertex %d: %5.2f %5.2f %5.2f\n", i, nba[i * 3 + 0], nba[i * 3 + 1], nba[i * 3 + 2]);
	}
}
