#ifndef _VECTOR_H_
#define _VECTOR_H_
#endif

#ifdef PATI
#define GLEW_STATIC
#endif

#ifdef ANDI
#define _USE_MATH_DEFINES
#endif


/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* OpenGL includes */
#include <GL/glew.h>
#include <GL/freeglut.h>

/* Local includes */
#include "LoadShader.h"
#include "Matrix.h"
#include "OBJParser.h"     /* Loading function for triangle meshes in OBJ format */


struct vec3 {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

struct vec3 vec3(GLfloat* v);
GLfloat dot3(struct vec3 v1, struct vec3 v2);
GLfloat len3(struct vec3 v);
struct vec3 normalize3(struct vec3 v);
struct vec3 add(struct vec3 v1, struct vec3 v2);

GLfloat det3x3(GLfloat* x, GLfloat* y, GLfloat* z);
void calculateNormals(GLushort* ida, GLfloat* vba, GLfloat* nba, int ndxs, int vdxs);
GLfloat norm3(GLfloat* vec);
void normailize(GLfloat* nba, int len);