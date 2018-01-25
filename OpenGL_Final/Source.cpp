#/*********************************************************************/
//   source.cpp -- Implementation of OpenGL using C++				   *
//		 Author:  Adwitiya Chakraborty                                 *
//                                                                     *
//      Purpose: Evaluate OpenGL shading techniques using C++		   *
//                                                                     *
// GitHub Repo : https://github.com/adwitiya                 		   *
//		 Email : chakraad@tcd.ie									   *
//  Build Date : 24.01.2018											   *
#/*********************************************************************/
//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "maths_funcs.h" 
#include "../models/avent.h" // car mesh

#pragma warning(disable : 4996)
// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;

GLuint shaderProgramID;
mat4 global_translate_up = identity_mat4();
mat4 global_translate_down = identity_mat4();
mat4 view_global = look_at(	vec3(0.0, 0.0, 1.1), 
							vec3(0.0, 0.0, 0.0), 
							vec3(0.0, 1.0, 0.0));
unsigned int teapot_vao = 0;
int width = 1024;
int height = 768;
mat4 persp_global = identity_mat4();
char keyFunction;


GLuint loc1;
GLuint loc2;
GLfloat rotatez = 0.0f;



// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS

// Create a NULL-terminated string by reading the provided file
char* readShaderSource(const char* shaderFile) {
	FILE* fp = fopen(shaderFile, "rb"); //!->Why does binary flag "RB" work and not "R"... wierd msvc thing?

	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}
	const char* pShaderSource = readShaderSource(pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}
	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders_toon()
{
	//Start the process of setting up our shaders by creating a program ID	
	//Note: we will link all the shaders together into this ID
	shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}


	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, "../shaders/simpleVertexShader.txt", GL_VERTEX_SHADER);
	AddShader(shaderProgramID, "../shaders/toonFragmentShader.txt", GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgramID);
	return shaderProgramID;
}

GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID	
	//Note: we will link all the shaders together into this ID
	shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}


	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, "../shaders/simpleVertexShader.txt", GL_VERTEX_SHADER);
	AddShader(shaderProgramID, "../shaders/simpleFragmentShader.txt", GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgramID);
	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS

void generateObjectBufferTeapot() {
	GLuint vp_vbo = 0;

	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normals");

	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * teapot_vertex_count * sizeof(float), teapot_vertex_points, GL_STATIC_DRAW);



	GLuint vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * teapot_vertex_count * sizeof(float), teapot_normals, GL_STATIC_DRAW);


	glGenVertexArrays(1, &teapot_vao);
	glBindVertexArray(teapot_vao);


	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

}


#pragma endregion VBO_FUNCTIONS

mat4 ortho(float left, float right, float bottom, float top, float nearr, float farr)
{
	return transpose(mat4(2.0f / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
		0.0f, 0.0f, 2.0f / (nearr - farr), 0.0f,
		(left + right) / (left - right), (bottom + top) / (bottom - top), (nearr + farr) / (nearr - farr), 1.0f));
}

void display() {
	mat4 view, persp_proj, root_local, root_global;
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glMatrixMode(GL_PROJECTION); 
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	
	
	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");

	// Main Viewport 
	glViewport(0, 0, width, height);
	root_local = global_translate_up * global_translate_down;
	view = identity_mat4();
	view = view_global;
	persp_proj = perspective(50.0, (float)width / (float)height, 0.1, 50.0);
	//root_local = rotate_y_deg(root_local,rotatez);
	//root_local = scale(root_local, vec3(1.1,1.1,1.1));
	
	//root node 
	root_global = root_local;
	

	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, root_global.m);
	glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);


	// MiniMap Viewport -- Top Left		
	glViewport(0, 3* height/4 , width/4 , height/4 );
	glLoadIdentity();
	view = look_at(vec3(0.0, 15.0, -5.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
	persp_proj = ortho(-width / 1000.0, width / 1000.0, -height / 1000.0, height / 1000.0, 0.1, 100.0);	
	// update uniforms & draw		
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, root_global.m);
	// draw child for minimap
	glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, root_global.m);
	glDrawArrays(GL_TRIANGLES, 0, teapot_vertex_count);

	glutSwapBuffers();
}



void updateScene() {

	// Placeholder code, if you want to work with framerate
	// Wait until at least 16ms passed since start of last frame (Effectively caps framerate at ~60fps)
	static DWORD  last_time = 0;
	DWORD  curr_time = timeGetTime();
	float  delta = (curr_time - last_time) * 0.001f;
	if (delta > 0.03f)
		delta = 0.03f;
	last_time = curr_time;

	rotatez += 0.03f;
	// Draw the next frame
	glutPostRedisplay();
}


void init()
{

	glClearColor(0.0, 0.0, 0.0, 0.0);
	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();
	// load teapot mesh into a vertex buffer array

	generateObjectBufferTeapot();
}

// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y) {

	if (key == 'r' || key == 'R') {
		global_translate_up = rotate_y_deg(global_translate_up,2.0);
		printf("rotate_y\n");
		glutPostRedisplay();
	}
	if (key == 's' || key == 'S') {
		global_translate_down = scale(global_translate_down, vec3(0.90, 0.90, 0.90));
		printf("scaledown\n");
		glutPostRedisplay();
	}
	if (key == 'w' || key == 'W') {
		global_translate_down = scale(global_translate_down, vec3(1.1, 1.1, 1.1));
		printf("scaleup\n");
		glutPostRedisplay();
	}

		if (key == 'd' || key == 'D') {
			global_translate_up = rotate_x_deg(global_translate_up, 2.0);
			printf("scaleup\n");
			glutPostRedisplay();
		}

		if (key == 'a' || key == 'A') {
			global_translate_up = rotate_z_deg(global_translate_up, 2.0);
			printf("scaleup\n");
		    glutPostRedisplay();
		}
		if (key == 't' || key == 'T') {
			
			printf("Toon Shader Selected\n");
			glDeleteProgram(shaderProgramID);
			GLuint shaderProgramID = CompileShaders_toon();
			glutPostRedisplay();
		}
		if (key == 'b' || key == 'B') {

			printf("Blinn Phong Shader Selected\n");
			glDeleteProgram(shaderProgramID);
			GLuint shaderProgramID = CompileShaders();
			glutPostRedisplay();
		}
		if (key == 'z' || key == 'Z') {

			printf("Auto Rotate On");
			global_translate_up = rotate_y_deg(global_translate_up, rotatez);
			glutPostRedisplay();
		}
		if (key == 'q' || key == 'Q') {

			static bool wire = true;
			wire = !wire;
			glPolygonMode(GL_FRONT_AND_BACK, (wire ? GL_LINE : GL_FILL));
			glutPostRedisplay();
		}
}

// Method to handle special keys function
void keySpecial(int keyspecial, int x, int y) {

		switch (keyspecial)
		{
		case GLUT_KEY_UP:
			view_global = translate(view_global, vec3(0.0, 0.0, 0.009));
			printf("cameramoveup\n");
			glutPostRedisplay();
			break;

		case GLUT_KEY_DOWN:
			view_global = translate(view_global, vec3(0.0, 0.00, -0.009));
			printf("cameramovedown\n");
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			view_global = translate(view_global, vec3(0.009, 0.00, 0.0));
			printf("cameramoveright\n");
			glutPostRedisplay();
			break;
		case GLUT_KEY_LEFT:
			view_global = translate(view_global, vec3(-0.009, 0.0, 0.0));
			printf("cameramoveleft\n");
			glutPostRedisplay();
			break;
		}
	}



int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Different Shader Example CS7GV3");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);


	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	printf("Press T for TOON Shader\n");
	printf("Press B for Blinn Phong Shader\n");
	printf("Press Q to toggle wireframe\n");
	glutKeyboardFunc(keypress);
	glutSpecialFunc(keySpecial);
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}