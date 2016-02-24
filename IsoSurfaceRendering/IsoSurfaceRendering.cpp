#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <IL/il.h>
#include <IL/ilu.h>

#include <iostream>
#include <cstdlib>
#include <vector>
#include <cassert>

#include "shaders/ShaderHandler.h"
#include "vbo/VBOHandler.h"
#include "digitalImage/Volume.h"
#include "surface/ArtzyAlgorithm.h"
#include "digitalimage/BinaryVolume.h"
#include "digitalimage/BoundaryElement.h"
#include "meshes/Mesh.h"


// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

static int win = 0;

GLint vertex_position_loc = -1;
GLint vertex_normal_loc = -1;
GLint model_loc = -1;
GLint projection_loc = -1;
GLint view_loc = -1;
GLint perspective_projection_loc = -1;

bool mouse_dragging;
glm::vec2 mouse_start_drag;
glm::vec2 base_rotation_angles;
glm::vec2 new_rotation_angles;
GLdouble scale_perspective;
GLdouble scale_ortho;

GLboolean projection_type;

float seconds;
int window_width;
int window_height;

glm::vec3 scene_center;
glm::vec3 min_coordinates;
glm::vec3 max_coordinates;

bool use_wireframe;

//shader handler object
shaders::ShaderHandler* shader_handler_ptr;
vbo::VBOHandler* vbo_handler_ptr;

//Callback functions for freeglut
void special_keys(int key, int mouse_x, int mouse_y);
void keyboard (unsigned char key, int mouse_x, int mouse_y);
void mouse_active(int mouse_x, int mouse_y);
void mouse(int button, int state, int mouse_x, int mouse_y);
void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y);
void display();
void idle();

//program management
void create_window();
void create_callbacks();
void init_OpenGL();
void exit_program();
void init_program();

//Volume loader
digitalimage::Volume* input_volume_ptr;

int main (int argc, char* argv[]) {
	glutInit(&argc, argv); 
	create_window();
	create_callbacks();
	
	init_OpenGL();
	init_program();
	
	glutMainLoop();

	exit_program();
	return EXIT_SUCCESS;		
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window
	
	glm::mat4 M = glm::mat4(1.0);
	glm::mat4 V = glm::mat4(1.0);
	glm::mat4 P = glm::mat4(1.0);

	//Model transformation
	M = glm::rotate(M, base_rotation_angles.y + new_rotation_angles.y, glm::vec3(1.0, 0.0, 0.0));
	M = glm::rotate(M, base_rotation_angles.x + new_rotation_angles.x, glm::vec3(0.0, 1.0, 0.0));
	M = glm::translate(M, -scene_center);

	//View transformation
	float world_radious = glm::distance(min_coordinates, max_coordinates) / 2.0f;
	if (projection_type) {
		glm::vec3 eye = glm::vec3(0.0, 0.0, -2.0 * world_radious);
		glm::vec3 at = glm::vec3(0.0, 0.0, 0.0);
		glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
		V = glm::lookAt(eye, at, up);
	} else {
		V = glm::scale(V, glm::vec3(scale_ortho, scale_ortho, scale_ortho));
	}

	//Projection transformation
	if (projection_type) {
		P = glm::perspective(scale_perspective, 1.0, 1.0 * world_radious, 3.0 * world_radious);
	} else {
		double z_distance = scale_ortho > 1.0 ? 2.0 * scale_ortho : 2.0;
		P = glm::ortho(-1.0, 1.0, -1.0, 1.0, z_distance, -z_distance);
	}
	//Pass uniform variables to shaders
	if (perspective_projection_loc != -1) {
		glUniform1i(perspective_projection_loc, projection_type ? 1 : 0);
	}
	if (model_loc != -1) {
	   glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(M));
	}
	if (view_loc != -1) {
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(V));
	}
	if (projection_loc != -1) {
	   glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(P));
	}
	
	// Draw the triangles
	vbo_handler_ptr->draw(vertex_position_loc, vertex_normal_loc);

	glutSwapBuffers();
}


void init_OpenGL() {
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
	}
	cout << "Hardware specification: " << endl;
	cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "Software specification: " << endl;
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
	cout << "Using GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	int ver = glutGet(GLUT_VERSION);
	cout << "Using freeglut version: " <<  ver / 10000 << "." << (ver / 100) % 100 << "." << ver % 100 << endl;
   
	//Init DevIL for reading the input Volume
	ilInit();
    iluInit();
	//input_volume_ptr = new digitalimage::Volume(L"../Data/Cube/Cube", 3);
	//input_volume_ptr = new digitalimage::Volume(L"../Data/Cross/Cross", 5);
	//input_volume_ptr = new digitalimage::Volume(L"../Data/NonSquareSmall/NonSquareSmall", 3);
	input_volume_ptr = new digitalimage::Volume(L"../Data/SL44_C4_4bar_Data/SL44_C4_4bar", 91);
	//input_volume_ptr = new digitalimage::Volume(L"../Data/testSmall/SquareImageStackSmall", 162);
	surface::ArtzyAlgorithm segmentator(*input_volume_ptr, 0.5f);
	//BoundaryElement seed(1, 1, 1, TOP);
	//BoundaryElement seed(2, 3, 2, TOP);
	//BoundaryElement seed(1, 1, 0, BACK);
	BoundaryElement seed(339, 480, 20, RIGHT);
	//BoundaryElement seed(85, 137, 0, BACK);
	Mesh boundary = segmentator.extract_surface(seed);

	vbo_handler_ptr = new vbo::VBOHandler(boundary); 
	scene_center = vbo_handler_ptr->get_centroid();
	max_coordinates = vbo_handler_ptr->get_min_coordinates();
	min_coordinates = vbo_handler_ptr->get_max_coordinates();

	//create and load shaders
	shader_handler_ptr = new shaders::ShaderHandler("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
	shader_handler_ptr->use_program();

	// Get location to uniform variables
	model_loc = shader_handler_ptr->get_uniform_location("M");
	view_loc = shader_handler_ptr->get_uniform_location("V");
	projection_loc = shader_handler_ptr->get_uniform_location("P");
	perspective_projection_loc = shader_handler_ptr->get_uniform_location("perspective");

    // Initialize the vertex position attribute from the vertex shader
	vertex_position_loc = shader_handler_ptr->get_attrib_location("vPosition");
    
	// Initialize the vertex color
	vertex_normal_loc = shader_handler_ptr->get_attrib_location("vNormal");

	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);

	//Activate anti-aliasing
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glLineWidth(3.0);
	
	//Back face culling
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void init_program() {
	seconds = 0.0;
	base_rotation_angles = glm::vec2(0.0, 0.0);
	new_rotation_angles = glm::vec2(0.0, 0.0);
	window_width = window_height = 640;
	projection_type = true;
	scale_perspective = 40.0;
	scale_ortho = 1.0;
    use_wireframe = false;
}
	
void create_window() {
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition (5, 5);
	glutInitWindowSize (640, 640);
	win = glutCreateWindow ("Isosurface rendering");
}

void create_callbacks() {
	glutDisplayFunc(display);
	glutSpecialFunc(special_keys);
	//glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseWheelFunc(mouse_wheel);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_active);
}

void exit_program() {
	if (shader_handler_ptr) {
		delete shader_handler_ptr;
		shader_handler_ptr = nullptr;
	}
	/*if (input_volume_ptr) {
		delete input_volume_ptr;
		input_volume_ptr = nullptr;
	}*/
	if (vbo_handler_ptr) {
		delete vbo_handler_ptr;
		vbo_handler_ptr = nullptr;
	}


	glutDestroyWindow(win);
}



void idle() {
	static int last_time = 0;

	int time = glutGet(GLUT_ELAPSED_TIME);
	int elapsed = time - last_time;
	last_time = time;
	float delta_seconds = 0.001f * elapsed;	
	seconds += delta_seconds;

	//set shader time uniform variable
	shader_handler_ptr->use_program();

	/*if(Time_loc != -1) {
		glUniform1f(Time_loc, seconds);
	}*/

	glutPostRedisplay();
}

void special_keys(int key, int mouse_x, int mouse_y) {
	switch (key) {
		case GLUT_KEY_UP:
			
		break;
		
		
	}
	
	glutPostRedisplay();
}

void keyboard (unsigned char key, int mouse_x, int mouse_y) {
	switch (key) {
	    case 'q':
	    case 'Q':
	    case 27:
			exit_program();
			exit(EXIT_SUCCESS);
		break;

		case 'R':
		case 'r':
			base_rotation_angles = glm::vec2(0.0, 0.0);
			scale_perspective = 60.0;
			scale_ortho = 1.0;
		default:
		break;

		case 'p':
		case 'P':
			projection_type = (!projection_type);
		break;

		case 'w':
		case 'W':
			use_wireframe = (!use_wireframe);
			if (use_wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		break;
	}

	glutPostRedisplay();
}


void mouse_active(int mouse_x, int mouse_y) {
	glm::vec2 mouse_current;
	
	mouse_current.x = static_cast<float>(mouse_x);
	mouse_current.y = static_cast<float>(mouse_y);
	
	glm::vec2 deltas = mouse_start_drag - mouse_current;

	new_rotation_angles.x = deltas.x / glutGet(GLUT_WINDOW_WIDTH) * 180;
	new_rotation_angles.y = deltas.y / glutGet(GLUT_WINDOW_HEIGHT) * 180;

	glutPostRedisplay();
}

void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y) {
	if (projection_type) {
		if (direction > 0 && scale_perspective < 160.0) {
			scale_perspective += 5.0;
		} else if (direction < 0 && scale_perspective > 0.0) {
			scale_perspective -= 5.0;
		}
		std::cout << "Scale: " << scale_perspective << std::endl;
	} else {
		if (direction > 0 && scale_ortho < 64.0) {
			scale_ortho *= 2.0;
		} else if (direction < 0) {
			scale_ortho /= 2.0;
		}
		std::cout << "Scale: " << scale_ortho << std::endl;
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int mouse_x, int mouse_y) {
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		mouse_dragging = true;
		mouse_start_drag.x = static_cast<float>(mouse_x);
		mouse_start_drag.y = static_cast<float>(mouse_y);
	} else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
		mouse_dragging = false;
		base_rotation_angles += new_rotation_angles;
		new_rotation_angles = glm::vec2(0.0, 0.0);
	}
	glutPostRedisplay();
}
