/******************************************************************
 *
 * MerryGoRound.c
 *
 * Computer Graphics Proseminar SS 2015
 *
 * Team Danek/Deutsch/Peringer
 *
 * Changes in Assignment 2:
 * 1. animation of inner objects
 * 2. keyboard functions
 * 3. mouse functions
 * 4. automatic zoom (only active in auto-camera mode)
 * 5. floor added
 * 6. background object added
 *
 * Keys:
 * q or Q: quit program
 * 0: activate or deactivate mouse controlled camera mode
 * 1: activate automatic camera mode (with camera position reset)
 * o: reset camera position
 *
 * Mouse:
 * Left-button: Xaxis rotation
 * Middle-button: Yaxis rotation
 * Right-button: Zaxis rotation
 *
 *******************************************************************/
#define _USE_MATH_DEFINES

#include "Pati.h"

#ifdef PATI
#define GLEW_STATIC
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

/* Flag for starting/stopping animation */
GLboolean anim = GL_FALSE;
GLboolean auto_anim = GL_TRUE;

//initialization camera distance
float distance = -20.0;

//initialization of zoom direction with -1 -> zomm in
int dir = 1;

/* Reference time for animation */
int oldTime = 0;

/*----------------------------------------------------------------*/

/* Define handles to vertex buffer objects */
GLuint VBO, VBO2, VBO_TEAPOT;

/* Define handles to color buffer objects */
GLuint CBO, CBO2, CBO3, CBO4, CBO5, CBO6, CBO7;

/* Define handles to index buffer objects */
GLuint IBO, IBO2, IBO_TEAPOT;

/* Arrays for holding vertex data of the two models */
GLfloat *vertex_buffer_teapot;

/* Arrays for holding indices of the two models */
GLushort *index_buffer_teapot;

//objects
obj_scene_data data;

/* Indices to vertex attributes; in this case positon and color */
enum DataID {
	vPosition = 0, vColor = 1
};

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

GLuint ShaderProgram;

float ProjectionMatrix[16]; /* Perspective projection matrix */
float ViewMatrix[16]; /* Camera view matrix */
float Rotation[16]; /* Rotation matrix */

/* Transformation matrices for model rotation */
float RotationMatrixAnimX[16];
float RotationMatrixAnimY[16];
float RotationMatrixAnimZ[16];

/* Variables for storing current rotation angles */
float angleX, angleY, angleZ = 0.0f;

/* Indices to active rotation axes */
enum {
	Xaxis = 0, Yaxis = 1, Zaxis = 2
};
int axis = Yaxis;

//Model matrices
float ModelMatrix[16];
float ModelMatrixPyramid[16];
float ModelMatrixPyramid2[16];
float ModelMatrixPyramid3[16];
float ModelMatrixPyramid4[16];
float ModelMatrixPyramid5[16];
float ModelMatrixPyramidTeapot[16];
float ModelMatrixPyramidTop[16];
float ModelMatrixTop[16];
float ModelMatrixFloor[16];

/*----------------------------------------------------------------*/
//Buffers for cuboid:
//Vertex buffer
GLfloat vertex_buffer_data[] = { /* 8 cube vertices */-4.0, -0.25, -4.0, -4.0,
		-0.25, 4.0, -4.0, 0.25, 4.0, -4.0, 0.25, -4.0, 4.0, -0.25, 4.0, 4.0,
		-0.25, -4.0, 4.0, 0.25, -4.0, 4.0, 0.25,  4.0, };

//Color buffers
GLfloat color_buffer_data[] = { /* RGB color values for vertices */
0.4, 0.2, 0.4, 0.3, 0.3, 0.4, 0.4, 0.2, 0.4, 0.3, 0.3, 0.4, 0.4, 0.2, 0.4, 0.3,
		0.3, 0.4,
0.4, 0.2, 0.4,
0.3, 0.3, 0.4, };

//Index buffer
GLushort index_buffer_data[] = { /* Indices of triangles */
0, 1, 2, 2, 3, 0, 1, 4, 7, 7, 2, 1, 7, 6, 5, 5, 4, 7, 4, 5, 0, 0, 1, 4, 2, 7, 6,
		6, 3, 2, 6, 3, 0, 0, 5, 6, };

/*----------------------------------------------------------------*/
//Buffers for pyramids:
//Vertex buffer
GLfloat vertex_buffer_data2[] = { //Definition of a Pyramid
		-1.0, -2.0, -1.0, 1.0, -2.0, -1.0, 1.0, -2.0, 1.0, -1.0, -2.0, 1.0, 0.0,
				0.0, 0.0, };

//Color buffer - red pyramid
GLfloat color_buffer_data_red[] = { /* RGB color values for vertices */
1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, };

//Color buffer - green pyramid
GLfloat color_buffer_data_green[] = { /* RGB color values for vertices */
0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, };

//Color buffer - blue pyramid
GLfloat color_buffer_data_blue[] = { /* RGB color values for vertices */
0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, };

//Color buffer - black pyramid
GLfloat color_buffer_data_black[] = { /* RGB color values for vertices */
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };

//Color buffer
GLfloat color_buffer_data_floor[] = { /* RGB color values for vertices */
1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
		1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, };

GLfloat color_buffer_data_background_object[] = { /* RGB color values for vertices */
1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, };

//Index buffer
GLushort index_buffer_data2[] = { /* Indices of triangles */
0, 1, 2, 2, 3, 0, 0, 1, 4, 2, 4, 1, 0, 4, 3, 3, 2, 4, };

/*----------------------------------------------------------------*/

/******************************************************************
 *
 * Display
 *
 * This function is called when the content of the window needs to be
 * drawn/redrawn. It has been specified through 'glutDisplayFunc()';
 * Enable vertex attributes, create binding between C program and
 * attribute name in shader
 *
 *******************************************************************/

void Display() {
	/* Clear window; color specified in 'Initialize()' */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableVertexAttribArray(vPosition);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(vColor);
	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	GLint size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	/* Associate program with shader matrices */
	GLint projectionUniform = glGetUniformLocation(ShaderProgram,
			"ProjectionMatrix");
	if (projectionUniform == -1) {
		fprintf(stderr, "Could not bind uniform ProjectionMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, ProjectionMatrix);

	GLint ViewUniform = glGetUniformLocation(ShaderProgram, "ViewMatrix");
	if (ViewUniform == -1) {
		fprintf(stderr, "Could not bind uniform ViewMatrix\n");
		exit(-1);
	}
	glUniformMatrix4fv(ViewUniform, 1, GL_TRUE, ViewMatrix);

	GLint RotationUniform = glGetUniformLocation(ShaderProgram, "ModelMatrix");
	if (RotationUniform == -1) {
		fprintf(stderr, "Could not bind uniform ModelMatrix\n");
		exit(-1);
	}

	//Screen elements:

	//lower platform
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, Rotation);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//upper platform
	float scaling[16];
	SetScaling(1.0, 1.0, 1.0, scaling);
	MultiplyMatrix(ModelMatrixTop, scaling, ModelMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixTop);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//connectors
	float transform[16];

	//connector 1
	SetScaling(0.05, 8.0, 0.05, scaling);
	SetTranslation(1.7, 1.1, 1.7, transform);
	MultiplyMatrix(transform, scaling, ModelMatrix);
	MultiplyMatrix(transform, ModelMatrix, transform);
	MultiplyMatrix(Rotation, transform, ModelMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//connector 2
	SetTranslation(-1.7, 1.1, -1.7, transform);
	MultiplyMatrix(transform, scaling, ModelMatrix);
	MultiplyMatrix(transform, ModelMatrix, transform);
	MultiplyMatrix(Rotation, transform, ModelMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//connector 3
	SetTranslation(-1.7, 1.1, 1.7, transform);
	MultiplyMatrix(transform, scaling, ModelMatrix);
	MultiplyMatrix(transform, ModelMatrix, transform);
	MultiplyMatrix(Rotation, transform, ModelMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//connector 4
	SetTranslation(1.7, 1.1, -1.7, transform);
	MultiplyMatrix(transform, scaling, ModelMatrix);
	MultiplyMatrix(transform, ModelMatrix, transform);
	MultiplyMatrix(Rotation, transform, ModelMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//switch to pyramid buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	//top pyramid
	SetScaling(4.0, 1.0, 4.0, scaling);
	MultiplyMatrix(ModelMatrixPyramidTop, scaling, ModelMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//switch color buffer
	glBindBuffer(GL_ARRAY_BUFFER, CBO2);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//transformation
	SetScaling(1.0, 1.0, 1.0, scaling);
	SetRotationZ(0.0, transform);
	MultiplyMatrix(transform, scaling, ModelMatrix);
	SetTranslation(0.0, 1.0, 0.0, transform);
	MultiplyMatrix(transform, ModelMatrix, transform);

	//inner elements:

	//pyramid 1
	MultiplyMatrix(ModelMatrixPyramid, transform, ModelMatrix);
	MultiplyMatrix(Rotation, ModelMatrix, ModelMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//switch color buffer
	glBindBuffer(GL_ARRAY_BUFFER, CBO3);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//pyramid 2
	MultiplyMatrix(ModelMatrixPyramid2, transform, ModelMatrix);
	MultiplyMatrix(Rotation, ModelMatrix, ModelMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//switch color buffer
	glBindBuffer(GL_ARRAY_BUFFER, CBO4);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//pyramid 3
	MultiplyMatrix(ModelMatrixPyramid3, transform, ModelMatrix);
	MultiplyMatrix(Rotation, ModelMatrix, ModelMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//switch color buffer
	glBindBuffer(GL_ARRAY_BUFFER, CBO5);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//pyramid 4
	MultiplyMatrix(ModelMatrixPyramid4, transform, ModelMatrix);
	MultiplyMatrix(Rotation, ModelMatrix, ModelMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//begin floor
	//pyramid 5 - static

	//color buffer
	glBindBuffer(GL_ARRAY_BUFFER, CBO7);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	SetScaling(3.0, 3.0, 3.0, scaling);
	SetTranslation(10.0, 5.0, -10.0, transform);
	MultiplyMatrix(transform, scaling, ModelMatrixPyramid5);
	MultiplyMatrix(transform, ModelMatrixPyramid5, transform);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixPyramid5);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//switch buffers

	//vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	//color buffer
	glBindBuffer(GL_ARRAY_BUFFER, CBO6);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//floor
	SetScaling(50.0, 1.0, 50.0, scaling);
	SetTranslation(0.0, -1.0, 0.0, transform);
	MultiplyMatrix(transform, scaling, ModelMatrixFloor);
	MultiplyMatrix(transform, ModelMatrixFloor, transform);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixFloor);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//end floor

	//begin teapot

	//switch to pyramid buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO_TEAPOT);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_TEAPOT);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	//switch color buffer
	glBindBuffer(GL_ARRAY_BUFFER, CBO5);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

	SetScaling(3.0, 3.0, 3.0, scaling);
	SetTranslation(10.0, 5.0, -10.0, transform);
	MultiplyMatrix(transform, scaling, ModelMatrixPyramidTeapot);
	MultiplyMatrix(transform, ModelMatrixPyramidTeapot, transform);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixPyramidTeapot);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	//end teapot

	/* Disable attributes */
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);

	/* Swap between front and back buffer */
	glutSwapBuffers();
}

/******************************************************************
 *
 * OnIdle
 *
 *
 *
 *******************************************************************/

void OnIdle() {
	float angle = (glutGet(GLUT_ELAPSED_TIME) / 1000.0) * (180.0 / M_PI);
	float rotation[16];
	float temp[16];
	float temp2[16];

	//rotation
	SetRotationY(angle, Rotation);

	//translation of upper platform
	SetTranslation(0.0, 4.0, 0.0, temp);
	MultiplyMatrix(temp, Rotation, ModelMatrixTop);

	//top pyramid
	SetTranslation(0.0, 6.25, 0.0, temp);
	MultiplyMatrix(temp, Rotation, ModelMatrixPyramidTop);

	//pyramid 1
	SetTranslation(0.3, (sin(M_PI / 180 * angle) * 1.0 + 2) / 2, 0.0, temp2);
	SetRotationY(angle * 2, rotation);
	SetTranslation(-2.5, 1.1, 0.0, temp);
	MultiplyMatrix(temp2, rotation, rotation);
	MultiplyMatrix(temp, rotation, ModelMatrixPyramid);

	//pyramid 2
	SetTranslation(0.0, (cos(M_PI / 180 * angle) * 1.0 + 2) / 2, -0.3, temp2);
	SetRotationY(angle * 2, rotation);
	SetTranslation(2.5, 1.1, 0.0, temp);
	MultiplyMatrix(temp2, rotation, rotation);
	MultiplyMatrix(temp, rotation, ModelMatrixPyramid2);

	//pyramid 3
	SetTranslation(-0.3, (-sin(M_PI / 180 * angle) + 2) / 2, 0.0, temp2);
	SetRotationY(angle * (-2), rotation);
	SetTranslation(0.0, 1.1, -2.5, temp);
	MultiplyMatrix(temp2, rotation, rotation);
	MultiplyMatrix(temp, rotation, ModelMatrixPyramid3);

	//pyramid 4
	SetTranslation(0.0, (-cos(M_PI / 180 * angle) + 2) / 2, 0.3, temp2);
	SetRotationY(angle * (-2), rotation);
	SetTranslation(0.0, 1.1, 2.5, temp);
	MultiplyMatrix(temp2, rotation, rotation);
	MultiplyMatrix(temp, rotation, ModelMatrixPyramid4);

	/* Determine delta time between two frames to ensure constant animation */
	int newTime = glutGet(GLUT_ELAPSED_TIME);
	int delta = newTime - oldTime;
	oldTime = newTime;

	//manual animation
	if (anim) {
		/* Increment rotation angles and update matrix */
		if (axis == Xaxis) {
			angleX = fmod(angleX + delta / 20.0, 360.0);
			SetRotationX(angleX, RotationMatrixAnimX);
		} else if (axis == Yaxis) {
			angleY = fmod(angleY + delta / 20.0, 360.0);
			SetRotationY(angleY, RotationMatrixAnimY);
		} else if (axis == Zaxis) {
			angleZ = fmod(angleZ + delta / 20.0, 360.0);
			SetRotationZ(angleZ, RotationMatrixAnimZ);
		}
	}

	//auto animation
	if (auto_anim) {
		//rotate via yaxis
		angleY = fmod(angleY + delta / 50.0, 360.0);
		SetRotationY(angleY, RotationMatrixAnimY);
	}

	/* Update of transformation matrices
	 * Note order of transformations and rotation of reference axes */
	MultiplyMatrix(RotationMatrixAnimX, RotationMatrixAnimY, rotation);
	MultiplyMatrix(rotation, RotationMatrixAnimZ, rotation);

	//automatic zoom
	float zoom = 0.0005;
	if (auto_anim) {
		//change zoom direction
		if (distance > -15 || distance < -20) {
			dir *= -1;
		}
		//get new camera distance
		distance += zoom * dir;
	}

	//translation of the whole object
	SetTranslation(0.0, -3.0, distance, ViewMatrix);
	MultiplyMatrix(ViewMatrix, rotation, ViewMatrix);

	/* Request redrawing forof window content */
	glutPostRedisplay();
}

/******************************************************************
 *
 * SetupDataBuffers
 *
 * Create buffer objects and load data into buffers
 *
 *******************************************************************/

void SetupDataBuffers() {

	//Buffers for platforms:

	//vertex buffer
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data),
			vertex_buffer_data, GL_STATIC_DRAW);

	//index buffer
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data),
			index_buffer_data, GL_STATIC_DRAW);

	//color buffer
	glGenBuffers(1, &CBO);
	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data,
	GL_STATIC_DRAW);

	//Buffers for innter elements:

	//vertex buffer (pyramid)
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data2),
			vertex_buffer_data2, GL_STATIC_DRAW);

	//index buffer
	glGenBuffers(1, &IBO2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data2),
			index_buffer_data2, GL_STATIC_DRAW);

	//color buffer (red)
	glGenBuffers(1, &CBO2);
	glBindBuffer(GL_ARRAY_BUFFER, CBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_red),
			color_buffer_data_red, GL_STATIC_DRAW);

	//color buffer (green)
	glGenBuffers(1, &CBO3);
	glBindBuffer(GL_ARRAY_BUFFER, CBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_green),
			color_buffer_data_green, GL_STATIC_DRAW);

	//color buffer (blue)
	glGenBuffers(1, &CBO4);
	glBindBuffer(GL_ARRAY_BUFFER, CBO4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_blue),
			color_buffer_data_blue, GL_STATIC_DRAW);

	//color buffer (black)
	glGenBuffers(1, &CBO5);
	glBindBuffer(GL_ARRAY_BUFFER, CBO5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_black),
			color_buffer_data_black, GL_STATIC_DRAW);

	//color buffer (white)
	glGenBuffers(1, &CBO6);
	glBindBuffer(GL_ARRAY_BUFFER, CBO6);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_floor),
			color_buffer_data_floor, GL_STATIC_DRAW);

	//color buffer (multicolor)
	glGenBuffers(1, &CBO7);
	glBindBuffer(GL_ARRAY_BUFFER, CBO7);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data_background_object),
			color_buffer_data_background_object, GL_STATIC_DRAW);


	//vertex buffer for teapot
    glGenBuffers(1, &VBO_TEAPOT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TEAPOT);
    glBufferData(GL_ARRAY_BUFFER, data.vertex_count*3*sizeof(GLfloat), vertex_buffer_teapot, GL_STATIC_DRAW);

    //index buffer for teapot
    glGenBuffers(1, &IBO_TEAPOT);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_TEAPOT);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.face_count*3*sizeof(GLushort), index_buffer_teapot, GL_STATIC_DRAW);

}

/******************************************************************
 *
 * AddShader
 *
 * This function creates and adds individual shaders
 *
 *******************************************************************/

void AddShader(GLuint ShaderProgram, const char* ShaderCode, GLenum ShaderType) {
	/* Create shader object */
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	/* Associate shader source code string with shader object */
	glShaderSource(ShaderObj, 1, &ShaderCode, NULL);

	GLint success = 0;
	GLchar InfoLog[1024];

	/* Compile shader source code */
	glCompileShader(ShaderObj);
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType,
				InfoLog);
		exit(1);
	}

	/* Associate shader with shader program */
	glAttachShader(ShaderProgram, ShaderObj);
}

/******************************************************************
 *
 * CreateShaderProgram
 *
 * This function creates the shader program; vertex and fragment
 * shaders are loaded and linked into program; final shader program
 * is put into the rendering pipeline
 *
 *******************************************************************/

void CreateShaderProgram() {
	/* Allocate shader object */
	ShaderProgram = glCreateProgram();

	if (ShaderProgram == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	/* Load shader code from file */
	VertexShaderString = LoadShader("vertexshader.vs");
	FragmentShaderString = LoadShader("fragmentshader.fs");

	/* Separately add vertex and fragment shader to program */
	AddShader(ShaderProgram, VertexShaderString, GL_VERTEX_SHADER);
	AddShader(ShaderProgram, FragmentShaderString, GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024];

	/* Link shader code into executable shader program */
	glLinkProgram(ShaderProgram);

	/* Check results of linking step */
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	/* Check if shader program can be executed */
	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);

	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	/* Put linked shader program into drawing pipeline */
	glUseProgram(ShaderProgram);
}

/******************************************************************
 *
 * Initialize
 *
 * This function is called to initialize rendering elements, setup
 * vertex buffer objects, and to setup the vertex and fragment shader
 *
 *******************************************************************/

void Initialize(void) {

	int i;
	int success;


	/* Load first OBJ model */
	char* filename = "models/teapot.obj";
	success = parse_obj_scene(&data, filename);

	if (!success)
		printf("Could not load file. Exiting.\n");

	/*  Copy mesh data from structs into appropriate arrays */
	int vert = data.vertex_count;
	int indx = data.face_count;

	vertex_buffer_teapot = (GLfloat*) calloc(vert * 3, sizeof(GLfloat));
	index_buffer_teapot = (GLushort*) calloc(indx * 3, sizeof(GLushort));

	/* Vertices */
	for (i = 0; i < vert; i++) {
		vertex_buffer_teapot[i * 3] = (GLfloat) (*data.vertex_list[i]).e[0];
		vertex_buffer_teapot[i * 3 + 1] = (GLfloat) (*data.vertex_list[i]).e[1];
		vertex_buffer_teapot[i * 3 + 2] = (GLfloat) (*data.vertex_list[i]).e[2];
	}

	/* Indices */
	for (i = 0; i < indx; i++) {
		index_buffer_teapot[i * 3] =
				(GLushort) (*data.face_list[i]).vertex_index[0];
		index_buffer_teapot[i * 3 + 1] =
				(GLushort) (*data.face_list[i]).vertex_index[1];
		index_buffer_teapot[i * 3 + 2] =
				(GLushort) (*data.face_list[i]).vertex_index[2];
	}


	/* Set background (clear) color to blue */
	glClearColor(0.5, 0.6, 1.0, 0.0);

	/* Enable depth testing */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	/* Setup vertex, color, and index buffer objects */
	SetupDataBuffers();

	/* Setup shaders and shader program */
	CreateShaderProgram();

	/* Initialize matrices */
	SetIdentityMatrix(ProjectionMatrix);
	SetIdentityMatrix(ViewMatrix);
	SetIdentityMatrix(ModelMatrix);

	/* Initialize animation matrices */
	SetIdentityMatrix(RotationMatrixAnimX);
	SetIdentityMatrix(RotationMatrixAnimY);
	SetIdentityMatrix(RotationMatrixAnimZ);

	/* Set projection transform */
	float fovy = 45.0;
	float aspect = 1.0;
	float nearPlane = 1.0;
	float farPlane = 50.0;
	SetPerspectiveMatrix(fovy, aspect, nearPlane, farPlane, ProjectionMatrix);

}

/******************************************************************
 *
 * Keyboard
 *
 * Function to be called on key press in window; set by
 * glutKeyboardFunc(); x and y specify mouse position on keypress;
 * not used in this example
 *
 *******************************************************************/

void Keyboard(unsigned char key, int x, int y) {

	switch (key) {

	//activate mouse animation
	case '0':
		if (anim) {
			anim = GL_FALSE;
		} else {
			anim = GL_TRUE;
		}
		auto_anim = GL_FALSE;

		break;

		//activate auto animation again
	case '1':
		anim = GL_FALSE;
		auto_anim = GL_TRUE;

		SetIdentityMatrix(RotationMatrixAnimX);
		SetIdentityMatrix(RotationMatrixAnimY);
		SetIdentityMatrix(RotationMatrixAnimZ);
		angleX = 0.0;
		angleY = 0.0;
		angleZ = 0.0;
		break;
		/* Reset initial rotation of object */
	case 'o':
		SetIdentityMatrix(RotationMatrixAnimX);
		SetIdentityMatrix(RotationMatrixAnimY);
		SetIdentityMatrix(RotationMatrixAnimZ);
		angleX = 0.0;
		angleY = 0.0;
		angleZ = 0.0;

		distance = -20.0;
		break;

	case 'q':
	case 'Q':
		exit(0);
		break;
	}

	glutPostRedisplay();
}

/******************************************************************
 *
 * Mouse
 *
 * Function is called on mouse button press; has been seta
 * with glutMouseFunc(), x and y specify mouse coordinates,
 * but are not used here.
 *
 *******************************************************************/

void Mouse(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) {
		/* Depending on button pressed, set rotation axis,
		 * turn on animation */
		switch (button) {
		case GLUT_LEFT_BUTTON:
			axis = Xaxis;
			break;

		case GLUT_MIDDLE_BUTTON:
			axis = Yaxis;
			break;

		case GLUT_RIGHT_BUTTON:
			axis = Zaxis;
			break;
		}
		anim = GL_TRUE;
	}

	glutPostRedisplay();
}

/******************************************************************
 *
 * main
 *
 * Main function to setup GLUT, GLEW, and enter rendering loop
 *
 *******************************************************************/

int main(int argc, char** argv) {
	/* Initialize GLUT; set double buffered window and RGBA color model */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(400, 400);
	glutCreateWindow("Assignment 2 - MerryGoRound");

	/* Initialize GL extension wrangler */
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	/* Setup scene and rendering parameters */
	Initialize();

	/* Specify callback functions;enter GLUT event processing loop,
	 * handing control over to GLUT */
	glutIdleFunc(OnIdle);
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMainLoop();

	/* ISO C requires main to return int */
	return 0;
}
