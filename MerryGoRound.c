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
 * 6. background object added (pyramid with teapot on top)
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
 * Changes in Assignment 3:
 * Lighting:
 * Keys:
 * a or A: decrease ambient light by 0.2
 * s or S: increase ambient light by 0.2
 * v or V: turn on/off ambient light
 * b or B: turn on/off diffuse light
 * n or N: turn on/off specular light
 *
 * 2 point lights added
 *
 * Phong shading and Phong lighting/reflection implemented (see fragmentshader.fs)
 * The first result was according to our simple mesh not as expected.
 * a directly lit face was not complete bright (because of the interpolated normals)
 * thats why we used vertex doubling do receive a constand normal for each face.
 *
 * Changes in Assignment 4:
 * Texturing:
 * floor texture added
 * background pyramid texture added
 * top texture added
 *
 *******************************************************************/

// Specific host configuration file
#include "Host.h"
#include "vector.h"

#ifdef PATI
#define GLEW_STATIC
#define _USE_MATH_DEFINES
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
#include "LoadTexture.h"
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
GLuint VBO_CUBE, VBO_PYRAMID, VBO_TEAPOT, VBO_FLOOR, VBO_PYRAMID_T;

/* Define handles to color buffer objects */
GLuint CBO_CUBE, CBO_PYRAMID, CBO_PYRAMID2, CBO_PYRAMID3, CBO_PYRAMID4,
		CBO_PYRAMID5, CBO_PYRAMID6;

/* Define handles to color buffer objects */
GLuint NBO_CUBE, NBO_PYRAMID, NBO_TEAPOT;

/* Define handles to index buffer objects */
GLuint IBO_CUBE, IBO_PYRAMID, IBO_TEAPOT, IBO_FLOOR;

/* Arrays for holding vertex data of the two models */
GLfloat *vba_teapot;

/* Arrays for holding indices of the two models */
GLushort *iba_teapot;

//objects
obj_scene_data data;

/* Variables for texture handling */
GLuint TextureIDChess;
GLuint TextureIDPyramid;
GLuint TextureIDTop;
GLuint TextureIDTeapot;
GLuint TextureUniform;
TextureDataPtr Texture;
TextureDataPtr TexturePyramid;
TextureDataPtr TextureTop;
TextureDataPtr TextureTeapot;

/* Indices to vertex attributes; in this case positon and color */
enum DataID {
	vPosition = 0, vColor = 1, vNormal = 2, vUV = 3,
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

// Enables the animation of the camera
int animateCamera = 1;

/*----------------------------------------------------------------*/
//Buffers for cuboid:
//Vertex buffer array
GLfloat vba_cube[] = {

// bottom
		-4.0, -0.25, -4.0, // 0 0
		-4.0, -0.25, 4.0, // 1 1
		4.0, -0.25, 4.0, // 4 2
		4.0, -0.25, -4.0, // 5 3

		// top
		-4.0, 0.25, 4.0, // 2 4
		-4.0, 0.25, -4.0, // 3 5
		4.0, 0.25, -4.0, // 6 6
		4.0, 0.25, 4.0, // 7 7

		// Front
		-4.0, -0.25, 4.0, // 1 8
		-4.0, 0.25, 4.0, // 2 9
		4.0, -0.25, 4.0, // 4 10
		4.0, 0.25, 4.0, // 7 11

		// Back
		-4.0, -0.25, -4.0, // 0 12
		-4.0, 0.25, -4.0, // 3 13
		4.0, -0.25, -4.0, // 5 14
		4.0, 0.25, -4.0, // 6 15

		// Left
		-4.0, -0.25, -4.0, // 0 16
		-4.0, -0.25, 4.0, // 1 17
		-4.0, 0.25, 4.0, // 2 18
		-4.0, 0.25, -4.0, // 3 19

		// Right
		4.0, -0.25, 4.0, // 4 20
		4.0, -0.25, -4.0, // 5 21
		4.0, 0.25, -4.0, // 6 22
		4.0, 0.25, 4.0  // 7 23
		};

//Index buffer array
GLushort iba_cube[] = {

// Bottom
		0, 2, 1, 0, 3, 2,

		// Top
		4, 7, 5, 5, 7, 6,

		// Front
		8, 10, 9, 9, 10, 11,

		// Back
		12, 13, 14, 13, 15, 14,

		// Left
		16, 17, 18, 16, 18, 19,

		// Right
		20, 21, 22, 20, 22, 23

/*
 0, 1, 2,
 2, 3, 0,
 1, 4, 7,
 7, 2, 1,
 6, 7, 5,
 4, 5, 7,
 4, 0, 5,
 0, 4, 1,
 2, 7, 6,
 6, 3, 2,
 6, 0, 3,
 0, 6, 5 */
};

//Color buffer array
GLfloat cba_cube[] = { /* RGB color values for vertices */0.4, 0.2, 0.4, 0.3, 0.3, 0.4, 0.4, 0.2, 0.4, 0.3, 0.3, 0.4, 0.4, 0.2, 0.4, 0.3,
		0.3, 0.4, 0.4, 0.2, 0.4, 0.3, 0.3, 0.4, };

GLfloat cba_purple[] = { 0.4, 0.2, 0.4 };
GLfloat cba_red[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat cba_green[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat cba_blue[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat cba_black[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat cba_floors[] = { 0.5, 0.5, 0.0, 1.0 };

/*----------------------------------------------------------------*/
//Buffers for pyramids:
//Vertex buffer
GLfloat vba_pyramid[] = { -1.0, -2.0, -1.0, 1.0, -2.0, -1.0, 1.0, -2.0, 1.0,
		-1.0, -2.0, 1.0,

		1.0, -2.0, -1.0, -1.0, -2.0, -1.0, 0.0, 0.0, 0.0,

		1.0, -2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, 1.0,

		-1.0, -2.0, -1.0, -1.0, -2.0, 1.0, 0.0, 0.0, 0.0,

		0.0, 0.0, 0.0, 1.0, -2.0, 1.0, 1.0, -2.0, -1.0, };

//Index buffer
GLushort iba_pyramid[] = {
// bottom
		1, 2, 0, 3, 0, 2,

		// back face
		4, 5, 6,

		// front face
		7, 8, 9,

		//left face
		10, 11, 12,

		// right face
		13, 14, 15 };

//Color buffer - red pyramid
GLfloat cba_pyramid_red[] = { /* RGB color values for vertices */
1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, };

//Color buffer - green pyramid
GLfloat cba_pyramid_green[] = { /* RGB color values for vertices */
0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, };

//Color buffer - blue pyramid
GLfloat cba_pyramid_blue[] = { /* RGB color values for vertices */
0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, };

//Color buffer - black pyramid
GLfloat cba_pyramid_black[] = { /* RGB color values for vertices */
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };

//Color buffer
GLfloat cba_floor[] = { 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, };

GLfloat cba_pyramid_back[] = { /* RGB color values for vertices */
1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, };

//normal buffers:
GLfloat *nba_cube;
GLfloat *nba_pyramid;
GLfloat *nba_teapot;

/* Structure containing XYZ position and RGB color data */
typedef struct {
	GLfloat Position[3];
	GLfloat UV[2];
} VertexData;

/* Define vertex positions and UV coordinates */
VertexData Floor[] = { { { -40.0, 0.0, 40.0 }, { 0, 1 } },    // 2 4
		{ { -40.0, 0.0, -40.0 }, { 0, 0 } },    // 3 5
		{ { 40.0, 0.0, -40.0 }, { 1, 0 } },    // 6 6
		{ { 40.0, 0.0, 40.0 }, { 1, 1 } },    // 7 7

		};

//Index buffer array - floor
GLushort iba_floor[] = { 0, 3, 1, 1, 3, 2, };

//Vertex buffer
VertexData vba_pyramid_texture[] = {

		//bottom
		{ { -1.0, -2.0, -1.0 }, { 0, 0 } }, { { 1.0, -2.0, -1.0 }, { 0, 0 } }, {
				{ 1.0, -2.0, 1.0 }, { 0, 0 } },
		{ { -1.0, -2.0, 1.0 }, { 0, 0 } },

		//back
		{ { 1.0, -2.0, -1.0 }, { 1, 0 } }, { { -1.0, -2.0, -1.0 }, { 0, 0 } }, {
				{ 0.0, 0.0, 0.0 }, { 0.5, 0.5 } },

		//front
		{ { 1.0, -2.0, 1.0 }, { 1, 1 } }, { { 0.0, 0.0, 0.0 }, { 0.5, 0.5 } }, {
				{ -1.0, -2.0, 1.0 }, { 0, 1 } },

		//left
		{ { -1.0, -2.0, -1.0 }, { 0, 0 } }, { { -1.0, -2.0, 1.0 }, { 0, 1 } }, {
				{ 0.0, 0.0, 0.0 }, { 0.5, 0.5 } },

		//right
		{ { 0.0, 0.0, 0.0 }, { 0.5, 0.5 } }, { { 1.0, -2.0, 1.0 }, { 1, 1 } }, {
				{ 1.0, -2.0, -1.0 }, { 1, 0 } } };

/*----------------------------------------------------------------*/

/******************************************************************
 *
 * setAmbientLighting
 *
 * This function is called if the ambient lighting factor should 
 * be changed.
 *
 *******************************************************************/
void setAmbientLighting(GLfloat val) {

	GLint ambientLoc = glGetUniformLocation(ShaderProgram, "Ambient");
	glUniform1f(ambientLoc, val);
}

void setShowAmbient(GLint val) {

	GLint show_ambientLoc = glGetUniformLocation(ShaderProgram, "ShowAmbient");
	glUniform1i(show_ambientLoc, val);
}

void setShowDiffuse(GLint val) {

	GLint show_diffuseLoc = glGetUniformLocation(ShaderProgram, "ShowDiffuse");
	glUniform1i(show_diffuseLoc, val);
}

void setShowSpecular(GLint val) {

	GLint show_specularLoc = glGetUniformLocation(ShaderProgram,
			"ShowSpecular");
	glUniform1i(show_specularLoc, val);
}

void setShininess(GLfloat val) {

	GLint ShininessLoc = glGetUniformLocation(ShaderProgram, "Shininess");
	glUniform1f(ShininessLoc, val);
}

void setStrength(GLfloat val) {

	GLint StrengthLoc = glGetUniformLocation(ShaderProgram, "Strength");
	glUniform1f(StrengthLoc, val);
}

void setLightColor1(GLfloat r, GLfloat g, GLfloat b) {

	GLint LightColorLoc = glGetUniformLocation(ShaderProgram, "L1_Color");
	glUniform3f(LightColorLoc, r, g, b);

}

void setLightPosition1(GLfloat x, GLfloat y, GLfloat z) {

	GLint LightPositionLoc = glGetUniformLocation(ShaderProgram, "L1_Position");
	glUniform3f(LightPositionLoc, x, y, z);

}

void setLightColor2(GLfloat r, GLfloat g, GLfloat b) {

	GLint LightColorLoc = glGetUniformLocation(ShaderProgram, "L2_Color");
	glUniform3f(LightColorLoc, r, g, b);

}

void setLightPosition2(GLfloat x, GLfloat y, GLfloat z) {

	GLint LightPositionLoc = glGetUniformLocation(ShaderProgram, "L2_Position");
	glUniform3f(LightPositionLoc, x, y, z);

}

void setAttenuation(GLfloat constant, GLfloat linear, GLfloat quadratic) {

	GLint AttenuationLoc = glGetUniformLocation(ShaderProgram, "Attenuation");
	glUniform3f(AttenuationLoc, constant, linear, quadratic);

}

GLfloat getAmbientLighting() {

	GLint ambientLoc = glGetUniformLocation(ShaderProgram, "Ambient");
	GLfloat val;
	glGetUniformfv(ShaderProgram, ambientLoc, &val);
	return val;
}

GLint getShowAmbient() {

	GLint show_ambientLoc = glGetUniformLocation(ShaderProgram, "ShowAmbient");
	GLint val;
	glGetUniformiv(ShaderProgram, show_ambientLoc, &val);
	return val;
}

GLint getShowDiffuse() {

	GLint show_diffuseLoc = glGetUniformLocation(ShaderProgram, "ShowDiffuse");
	GLint val;
	glGetUniformiv(ShaderProgram, show_diffuseLoc, &val);
	return val;
}

GLint getShowSpecular() {

	GLint show_specularLoc = glGetUniformLocation(ShaderProgram,
			"ShowSpecular");
	GLint val;
	glGetUniformiv(ShaderProgram, show_specularLoc, &val);
	return val;
}

void setColor(GLfloat* val) {

	GLint ColorLoc = glGetUniformLocation(ShaderProgram, "FixedColor");
	glUniform4f(ColorLoc, val[0], val[1], val[2], val[3]);
}

void enableTexture() {

	GLint useTextureLoc = glGetUniformLocation(ShaderProgram, "UseTexture");
	glUniform1i(useTextureLoc, 1);
}

void disableTexture() {

	GLint useTextureLoc = glGetUniformLocation(ShaderProgram, "UseTexture");
	glUniform1i(useTextureLoc, 0);
}

void setEyeDirection(GLfloat x, GLfloat y, GLfloat z) {

	GLint EyeDirectionLoc = glGetUniformLocation(ShaderProgram, "EyeDirection");
	glUniform3f(EyeDirectionLoc, x, y, z);
}

void activateTexture(GLuint handle, GLuint id) {

	/* Activate texture unit */
	switch (handle) {
	case 0:

		glActiveTexture(GL_TEXTURE0);
		break;
	case 1:
		glActiveTexture(GL_TEXTURE1);
		break;
	case 2:
		glActiveTexture(GL_TEXTURE2);
		break;
	case 3:
		glActiveTexture(GL_TEXTURE3);
		break;
	}

	/* Bind current texture  */
	glBindTexture(GL_TEXTURE_2D, id);

	/* Get texture uniform handle from fragment shader */
	TextureUniform = glGetUniformLocation(ShaderProgram, "myTextureSampler");

	/* Set location of uniform sampler variable */
	glUniform1i(TextureUniform, handle);

}

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
	glBindBuffer(GL_ARRAY_BUFFER, VBO_CUBE);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(vColor);
	glBindBuffer(GL_ARRAY_BUFFER, CBO_CUBE);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
	setColor(cba_purple);

	glEnableVertexAttribArray(vNormal);
	glBindBuffer(GL_ARRAY_BUFFER, NBO_CUBE);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_CUBE);
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
	glBindBuffer(GL_ARRAY_BUFFER, VBO_PYRAMID);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO_PYRAMID);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_PYRAMID);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);


//switch color buffer
	glBindBuffer(GL_ARRAY_BUFFER, CBO_PYRAMID);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
	setColor(cba_red);

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
	glBindBuffer(GL_ARRAY_BUFFER, CBO_PYRAMID2);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
	setColor(cba_green);

//pyramid 2
	MultiplyMatrix(ModelMatrixPyramid2, transform, ModelMatrix);
	MultiplyMatrix(Rotation, ModelMatrix, ModelMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

//switch color buffer
	glBindBuffer(GL_ARRAY_BUFFER, CBO_PYRAMID3);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
	setColor(cba_blue);

//pyramid 3
	MultiplyMatrix(ModelMatrixPyramid3, transform, ModelMatrix);
	MultiplyMatrix(Rotation, ModelMatrix, ModelMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

//switch color buffer
	glBindBuffer(GL_ARRAY_BUFFER, CBO_PYRAMID4);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

//pyramid 4
	MultiplyMatrix(ModelMatrixPyramid4, transform, ModelMatrix);
	MultiplyMatrix(Rotation, ModelMatrix, ModelMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

//begin background objects


//begin textures

	//begin teapot

	//switch to teapot buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO_TEAPOT);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, NBO_TEAPOT);
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_TEAPOT);
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);


		//activate texture unit for teapot
		activateTexture(3, TextureIDTeapot);

	//	/* For each vertex attribue specify location of data */
		glEnableVertexAttribArray(vUV);
		glVertexAttribPointer(vUV, 2, GL_FLOAT, GL_FALSE, sizeof(data.vertex_count),
				(const GLvoid*) (2 * sizeof(obj_vector)));

	//enable texture in fragment shader
		enableTexture();

		SetScaling(0.4, 0.4, 0.4, scaling);
		SetTranslation(10.0, 4.0, -10.0, transform);
		MultiplyMatrix(transform, scaling, ModelMatrixPyramidTeapot);
		MultiplyMatrix(transform, ModelMatrixPyramidTeapot, transform);
		glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixPyramidTeapot);
		glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	//end teapot

	//activate texture unit for floor
	activateTexture(0, TextureIDChess);

	/* Enable position and UV attribute */
	glEnableVertexAttribArray(vPosition);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_FLOOR);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_CUBE);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	/* For each vertex attribue specify location of data */
	glEnableVertexAttribArray(vUV);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
			0);
	glVertexAttribPointer(vUV, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData),
			(const GLvoid*) sizeof(Floor[0].Position));

//enable texture in fragment shader
	enableTexture();

//floor texture
	SetScaling(1.0, 1.0, 1.0, scaling);
	SetTranslation(0.0, -0.73, 0.0, transform);
	MultiplyMatrix(transform, scaling, ModelMatrixFloor);
	MultiplyMatrix(transform, ModelMatrixFloor, transform);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixFloor);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

//begin static pyramid

	//activate texture unit for background pyramid
	activateTexture(1, TextureIDPyramid);

	/* Enable position and UV attribute */
	glEnableVertexAttribArray(vPosition);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_PYRAMID_T);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_PYRAMID);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	/* For each vertex attribue specify location of data */
	glEnableVertexAttribArray(vUV);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
			0);
	glVertexAttribPointer(vUV, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData),
			(const GLvoid*) sizeof(vba_pyramid_texture[0].Position));

	SetScaling(3.0, 3.0, 3.0, scaling);
	SetTranslation(10.0, 5.4, -10.0, transform);
	MultiplyMatrix(transform, scaling, ModelMatrixPyramid5);
	MultiplyMatrix(transform, ModelMatrixPyramid5, transform);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrixPyramid5);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
//end static pyramid

//begin top pyramid

	//activate texture unit for top
	activateTexture(2, TextureIDTop);

	SetScaling(4.0, 1.0, 4.0, scaling);
	MultiplyMatrix(ModelMatrixPyramidTop, scaling, ModelMatrix);
	glUniformMatrix4fv(RotationUniform, 1, GL_TRUE, ModelMatrix);
	glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
//end top pyramid

//disable texture in fragment shader
	disableTexture();

//end textures

	/* Disable attributes */
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
	glDisableVertexAttribArray(vNormal);
	glDisableVertexAttribArray(vUV);

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
//float angle = 0.0;
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
	float zoom = 0;

	if (animateCamera) {
		zoom = 0.0005;

		//change zoom direction
		if (distance > -15 || distance < -20) {
			dir *= -1;
		}
		//get new camera distance
		distance += zoom * dir;
	}

//translation of the whole object
	SetTranslation(0.0, -3.0, distance, ViewMatrix);

	if (animateCamera) {
		MultiplyMatrix(ViewMatrix, rotation, ViewMatrix);
	}

	setEyeDirection(0, 3, -distance);
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
	glGenBuffers(1, &VBO_CUBE);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_CUBE);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vba_cube), vba_cube, GL_STATIC_DRAW);

//index buffer
	glGenBuffers(1, &IBO_CUBE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_CUBE);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iba_cube), iba_cube,
	GL_STATIC_DRAW);

//color buffer
	glGenBuffers(1, &CBO_CUBE);
	glBindBuffer(GL_ARRAY_BUFFER, CBO_CUBE);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cba_cube), cba_cube,
	GL_STATIC_DRAW);

//normal buffer
	glGenBuffers(1, &NBO_CUBE);
	glBindBuffer(GL_ARRAY_BUFFER, NBO_CUBE);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vba_cube), nba_cube,
	GL_STATIC_DRAW);

//Buffers for innter elements:

//vertex buffer (pyramid)
	glGenBuffers(1, &VBO_PYRAMID);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_PYRAMID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vba_pyramid), vba_pyramid,
	GL_STATIC_DRAW);

//vertex buffer (pyramid with texture coordinates)
	glGenBuffers(1, &VBO_PYRAMID_T);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_PYRAMID_T);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vba_pyramid_texture),
			vba_pyramid_texture,
			GL_STATIC_DRAW);

//index buffer
	glGenBuffers(1, &IBO_PYRAMID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_PYRAMID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iba_pyramid), iba_pyramid,
	GL_STATIC_DRAW);

//normal buffer
	glGenBuffers(1, &NBO_PYRAMID);
	glBindBuffer(GL_ARRAY_BUFFER, NBO_PYRAMID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vba_pyramid), nba_pyramid,
	GL_STATIC_DRAW);

//color buffer (red)
	glGenBuffers(1, &CBO_PYRAMID);
	glBindBuffer(GL_ARRAY_BUFFER, CBO_PYRAMID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cba_pyramid_red), cba_pyramid_red,
	GL_STATIC_DRAW);

//color buffer (green)
	glGenBuffers(1, &CBO_PYRAMID2);
	glBindBuffer(GL_ARRAY_BUFFER, CBO_PYRAMID2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cba_pyramid_green), cba_pyramid_green,
	GL_STATIC_DRAW);

//color buffer (blue)
	glGenBuffers(1, &CBO_PYRAMID3);
	glBindBuffer(GL_ARRAY_BUFFER, CBO_PYRAMID3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cba_pyramid_blue), cba_pyramid_blue,
	GL_STATIC_DRAW);

//color buffer (black)
	glGenBuffers(1, &CBO_PYRAMID4);
	glBindBuffer(GL_ARRAY_BUFFER, CBO_PYRAMID4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cba_pyramid_black), cba_pyramid_black,
	GL_STATIC_DRAW);

//color buffer (white)
	glGenBuffers(1, &CBO_PYRAMID5);
	glBindBuffer(GL_ARRAY_BUFFER, CBO_PYRAMID5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cba_floor), cba_floor, GL_STATIC_DRAW);

//color buffer (multicolor)
	glGenBuffers(1, &CBO_PYRAMID6);
	glBindBuffer(GL_ARRAY_BUFFER, CBO_PYRAMID6);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cba_pyramid_back), cba_pyramid_back,
	GL_STATIC_DRAW);

//vertex buffer for teapot
	glGenBuffers(1, &VBO_TEAPOT);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_TEAPOT);
	glBufferData(GL_ARRAY_BUFFER, data.vertex_count * 3 * sizeof(GLfloat),
			vba_teapot, GL_STATIC_DRAW);

//index buffer for teapot
	glGenBuffers(1, &IBO_TEAPOT);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_TEAPOT);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			data.face_count * 3 * sizeof(GLushort), iba_teapot, GL_STATIC_DRAW);

//normal buffer
	glGenBuffers(1, &NBO_TEAPOT);
	glBindBuffer(GL_ARRAY_BUFFER, NBO_TEAPOT);
	glBufferData(GL_ARRAY_BUFFER, data.vertex_count * 3 * sizeof(GLfloat),
			nba_teapot, GL_STATIC_DRAW);

//floor
	glGenBuffers(1, &VBO_FLOOR);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_FLOOR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Floor), Floor, GL_STATIC_DRAW);

//index buffer for floor
	glGenBuffers(1, &IBO_FLOOR);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_FLOOR);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iba_floor), iba_floor,
	GL_STATIC_DRAW);

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

void setupLighting() {

	/* Set lighing defaults*/
	setAmbientLighting(0.2);
	setShowAmbient(1);
	setShowDiffuse(1);
	setShowSpecular(1);
	setShininess(150.0);
	setStrength(5.0);

//point light 1
	setLightColor1(1, 1, 1);
	setLightPosition1(10, 7, -distance);

//point light 2
	setLightColor2(1, 0.2, 0.2);
	setLightPosition2(-10, 2, -distance);
	setAttenuation(0.2, 0.01, 0.01);
}

/******************************************************************
 *
 * SetupTexture
 *
 * This function is called to load the texture and initialize
 * texturing parameters
 *
 *******************************************************************/

void SetupTextureFloor(void) {

	/* Allocate texture container */
	Texture = malloc(sizeof(TextureDataPtr));

	int success = LoadTexture("data/chess.bmp", Texture);
	if (!success) {
		printf("Error loading texture. Exiting.\n");
		exit(-1);
	}

	/* Create texture name and store in handle */
	glGenTextures(1, &TextureIDChess);

	/* Bind texture */
	glBindTexture(GL_TEXTURE_2D, TextureIDChess);

	/* Load texture image into memory */
	glTexImage2D(GL_TEXTURE_2D, /* Target texture */
	0, /* Base level */
	GL_RGB, /* Each element is RGB triple */
	Texture->width, /* Texture dimensions */
	Texture->height, 0, /* Border should be zero */
	GL_BGR, /* Data storage format for BMP file */
	GL_UNSIGNED_BYTE, /* Type of pixel data, one byte per channel */
	Texture->data); /* Pointer to image data  */

	/* Next set up texturing parameters */

	/* Repeat texture on edges when tiling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/* Linear interpolation for magnification */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Trilinear MIP mapping for minification */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	/* Note: MIP mapping not visible due to fixed, i.e. static camera */

}

void SetupTextureTeapot(void) {

	/* Allocate texture container */
	TextureTeapot = malloc(sizeof(TextureDataPtr));

	int success = LoadTexture("data/lego.bmp", TextureTeapot);
	if (!success) {
		printf("Error loading texture. Exiting.\n");
		exit(-1);
	}

	/* Create texture name and store in handle */
	glGenTextures(1, &TextureIDTeapot);

	/* Bind texture */
	glBindTexture(GL_TEXTURE_2D, TextureIDTeapot);

	/* Load texture image into memory */
	glTexImage2D(GL_TEXTURE_2D, /* Target texture */
	0, /* Base level */
	GL_RGB, /* Each element is RGB triple */
	TextureTeapot->width, /* Texture dimensions */
	TextureTeapot->height, 0, /* Border should be zero */
	GL_BGR, /* Data storage format for BMP file */
	GL_UNSIGNED_BYTE, /* Type of pixel data, one byte per channel */
	TextureTeapot->data); /* Pointer to image data  */

	/* Next set up texturing parameters */

	/* Repeat texture on edges when tiling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/* Linear interpolation for magnification */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Trilinear MIP mapping for minification */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	/* Note: MIP mapping not visible due to fixed, i.e. static camera */

}

void SetupTexturePyramid(void) {

	/* Allocate texture container */
	TexturePyramid = malloc(sizeof(TextureDataPtr));

	int success = LoadTexture("data/floor.bmp", TexturePyramid);
	if (!success) {
		printf("Error loading texture. Exiting.\n");
		exit(-1);
	}

	/* Create texture name and store in handle */
	glGenTextures(1, &TextureIDPyramid);

	/* Bind texture */
	glBindTexture(GL_TEXTURE_2D, TextureIDPyramid);

	/* Load texture image into memory */
	glTexImage2D(GL_TEXTURE_2D, /* Target texture */
	0, /* Base level */
	GL_RGB, /* Each element is RGB triple */
	TexturePyramid->width, /* Texture dimensions */
	TexturePyramid->height, 0, /* Border should be zero */
	GL_BGR, /* Data storage format for BMP file */
	GL_UNSIGNED_BYTE, /* Type of pixel data, one byte per channel */
	TexturePyramid->data); /* Pointer to image data  */

	/* Next set up texturing parameters */

	/* Repeat texture on edges when tiling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/* Linear interpolation for magnification */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Trilinear MIP mapping for minification */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	/* Note: MIP mapping not visible due to fixed, i.e. static camera */

}

void SetupTextureTop(void) {

	/* Allocate texture container */
	TextureTop = malloc(sizeof(TextureDataPtr));

	int success = LoadTexture("data/dach.bmp", TextureTop);
	if (!success) {
		printf("Error loading texture. Exiting.\n");
		exit(-1);
	}

	/* Create texture name and store in handle */
	glGenTextures(1, &TextureIDTop);

	/* Bind texture */
	glBindTexture(GL_TEXTURE_2D, TextureIDTop);

	/* Load texture image into memory */
	glTexImage2D(GL_TEXTURE_2D, /* Target texture */
	0, /* Base level */
	GL_RGB, /* Each element is RGB triple */
	TextureTop->width, /* Texture dimensions */
	TextureTop->height, 0, /* Border should be zero */
	GL_BGR, /* Data storage format for BMP file */
	GL_UNSIGNED_BYTE, /* Type of pixel data, one byte per channel */
	TextureTop->data); /* Pointer to image data  */

	/* Next set up texturing parameters */

	/* Repeat texture on edges when tiling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/* Linear interpolation for magnification */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Trilinear MIP mapping for minification */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	/* Note: MIP mapping not visible due to fixed, i.e. static camera */

}

void SetupTexture(void) {

//setup textures
	SetupTextureFloor();
	SetupTexturePyramid();
	SetupTextureTop();
	SetupTextureTeapot();

}

/******************************************************************
 *
 * Initialize
 *
 * This function is called to initialize rendering elements, setup
 * vertex buffer objects, and to setup the vertex and fragment s0ader
 *
 *******************************************************************/

void Initialize(void) {

	int i;
	int success;

	/* Load first OBJ model */
	char* filename = "models/teapot3.obj";
	success = parse_obj_scene(&data, filename);

	if (!success)
		printf("Could not load file. Exiting.\n");

	/*  Copy mesh data from structs into appropriate arrays */
	int vert = data.vertex_count;
	int indx = data.face_count;

	vba_teapot = (GLfloat*) calloc(vert * 3, sizeof(GLfloat));
	iba_teapot = (GLushort*) calloc(indx * 3, sizeof(GLushort));

	/* Vertices */
	for (i = 0; i < vert; i++) {
		vba_teapot[i * 3] = (GLfloat) (*data.vertex_list[i]).e[0];
		vba_teapot[i * 3 + 1] = (GLfloat) (*data.vertex_list[i]).e[1];
		vba_teapot[i * 3 + 2] = (GLfloat) (*data.vertex_list[i]).e[2];
	}

	/* Indices */
	for (i = 0; i < indx; i++) {
		iba_teapot[i * 3] = (GLushort) (*data.face_list[i]).vertex_index[0];
		iba_teapot[i * 3 + 1] = (GLushort) (*data.face_list[i]).vertex_index[1];
		iba_teapot[i * 3 + 2] = (GLushort) (*data.face_list[i]).vertex_index[2];
	}

//normal buffers

//normals for cube
	nba_cube = (GLfloat*) malloc(sizeof(vba_cube));
	memset(nba_cube, 0.0f, sizeof(vba_cube));

//normals for pyramid
	nba_pyramid = (GLfloat*) malloc(sizeof(vba_pyramid));
	memset(nba_pyramid, 0.0f, sizeof(vba_pyramid));

//normals for teapot
	nba_teapot = (GLfloat*) malloc(data.vertex_count * 3 * sizeof(GLfloat));
	memset(nba_teapot, 0.0, data.vertex_count * 3 * sizeof(GLfloat));

//calculate normals
	calculateNormals(iba_cube, vba_cube, nba_cube,
			sizeof(iba_cube) / sizeof(GLushort),
			sizeof(vba_cube) / sizeof(GLfloat));

	calculateNormals(iba_pyramid, vba_pyramid, nba_pyramid,
			sizeof(iba_pyramid) / sizeof(GLushort),
			sizeof(vba_pyramid) / sizeof(GLfloat));

	calculateNormals(iba_teapot, vba_teapot, nba_teapot, indx * 3, vert * 3);

	/* Set background (clear) color to blue */
	glClearColor(0.5, 0.6, 1.0, 0.0);

	/* Enable depth testing */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	/* Setup vertex, color, and index buffer objects */
	SetupDataBuffers();

	/* Setup shaders and shader program */
	CreateShaderProgram();

	/* Setup texture */
	SetupTexture();

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

	/* Set lighing defaults*/
	setupLighting();
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

	GLfloat ambient = 0.0;
	GLint show_ambient = 0;
	GLint show_diffuse = 0;
	GLint show_specular = 0;

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

		//LIGHTING!!!

		//decrease ambient light
	case 'a':
	case 'A':
		ambient = getAmbientLighting();
		if (ambient > 0.0) {
			ambient -= 0.2;
			setAmbientLighting(ambient);
		}
		break;
		//increase ambient light
	case 's':
	case 'S':
		ambient = getAmbientLighting();
		if (ambient < 1.0) {
			ambient += 0.2;
			setAmbientLighting(ambient);
		}
		break;
		//turn on/off ambient light
	case 'v':
	case 'V':
		//get current value
		show_ambient = getShowAmbient();
		//turn on ambient light
		if (show_ambient == 0) {
			show_ambient = 1;
			//turn off ambient light
		} else {
			show_ambient = 0;
		}
		//set variable in shader
		setShowAmbient(show_ambient);
		break;
		//turn on/off diffuse light
	case 'b':
	case 'B':
		//get current value
		show_diffuse = getShowDiffuse();
		//turn on diffuse light
		if (show_diffuse == 0) {
			show_diffuse = 1;
			//turn off diffuse light
		} else {
			show_diffuse = 0;
		}
		//set variable in shader
		setShowDiffuse(show_diffuse);
		break;
		//turn on/off specular light
	case 'n':
	case 'N':
		//get current value
		show_specular = getShowSpecular();
		//turn on specular light
		if (show_specular == 0) {
			show_specular = 1;
			//turn off specular light
		} else {
			show_specular = 0;
		}
		//set variable in shader
		setShowSpecular(show_specular);
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
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Assignment 4 - MerryGoRound");

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
