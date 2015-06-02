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

GLfloat det3x3(GLfloat* x, GLfloat* y, GLfloat* z);
void calculateNormals(GLushort* ida, GLfloat* vba, GLfloat* nba, int len);
void normailize(GLfloat* nba, int len);