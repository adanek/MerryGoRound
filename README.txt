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
 *******************************************************************/