#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "blobs/Blob.h"
#include "digitalimage/Volume.h"
#include "shaders/ShaderHandler.h"
#include "volume/VolumeGeometry.h"
//Temporary including
#include "shaders/ShadersSource.h"
#include "digitalimage/Image.h"
#include "image/TextureHandler.h"
#include "blobs/BlobCapsule.h"
#include "experiment/Sampler.h"
#include "digitalimage/GrayScaleVolume.h"

#include <IL/il.h>
#include <IL/ilu.h>


#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))
using namespace experiment;
using namespace blobs;
using namespace std;
using namespace image;

using namespace std;
static int win = 0;
Image* texture;
Volume* input_volume;

image::TextureHandler* texture_1;
image::TextureHandler* texture_2;
image::TextureHandler* texture_3;

//Shaders handler object
shaders::ShaderHandler* shader_handler_ptr;
//Shaders variables location
//Atrib
GLint position_location;
GLint texture_coordnates_location;
//Uniform
GLint texture_location;
GLint model_view_location;
GLint projection_location;
GLint translation_location;
//Program drawing options
bool projection_type;
bool render_plane_x;
bool render_plane_y;
bool render_plane_z;
//Mouse handling facilities
bool mouse_dragging;
glm::vec2 mouse_start_drag;
glm::vec2 base_rotation_angles;
glm::vec2 new_rotation_angles;
GLdouble scale_perspective;
GLdouble scale_ortho;

//opengl vertex buffer objects
GLuint vertex_buffer = 0;
GLuint texture_coordinates_buffer = 0;
GLuint index_buffer = 0;

//Plane number
unsigned int x_plane;
unsigned int y_plane;
unsigned int z_plane;
unsigned int num_planes_x;
unsigned int num_planes_y;
unsigned int num_planes_z;
glm::vec3 displacement;
glm::vec3 delta;

//Program managment
void exit_glut();
//void change_plane(int new_plane, plane orientation);

//Callback functions
void special_keys(int key, int mouse_x, int mouse_y);
void keyboard (unsigned char key, int mouse_x, int mouse_y);
void mouse_active(int mouse_x, int mouse_y);
void mouse(int button, int state, int mouse_x, int mouse_y);
void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y);
void display();

//Initializers
void create_glut_window();
void create_glut_callbacks();
void init_OpenGL();
void create_geometry();
void init_program();

//Temporary image loading
void create_image();

int main (int argc, char* argv[]) {
	glutInit(&argc, argv); 
	create_glut_window();
	create_glut_callbacks();
	init_program();
	//ilInit();
	init_OpenGL();
	glutMainLoop();

	exit_glut();
	return EXIT_SUCCESS;		
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader_handler_ptr->use_program();

	glm::mat4 M = glm::mat4(1.0);
	glm::mat4 V = glm::mat4(1.0);
	glm::mat4 Proj = glm::mat4(1.0);

	//Model transformation
	M = glm::rotate(M, base_rotation_angles.y + new_rotation_angles.y, glm::vec3(1.0, 0.0, 0.0));
	M = glm::rotate(M, base_rotation_angles.x + new_rotation_angles.x, glm::vec3(0.0, 1.0, 0.0));

	//View transformation
	double world_radious = 1.7;
	if (projection_type) {
		glm::vec3 eye = glm::vec3(0.0, 0.0, -2.0 * world_radious);
		glm::vec3 at = glm::vec3(0.0, 0.0, 0.0);
		glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
		V = glm::lookAt(eye, at, up);
	} else {
		V = glm::scale(M, glm::vec3(scale_ortho, scale_ortho, scale_ortho));
	}

	//Projection transformation
	if (projection_type) {
		Proj = glm::perspective(scale_perspective, 1.0, world_radious, 3.0 * world_radious);
	} else {
		double z_distance = scale_ortho > 1.0 ? scale_ortho * world_radious : world_radious;
		Proj = glm::ortho(-1.0, 1.0, -1.0, 1.0, -z_distance, z_distance);
	}
	//Pass uniform variables to shaders
	if (model_view_location != -1) {
	   glUniformMatrix4fv(model_view_location, 1, GL_FALSE, glm::value_ptr(V * M));
	}
	if (projection_location != -1) {
	   glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(Proj));
	}
	

	position_location = shader_handler_ptr->get_attrib_location("position_attribute");
	texture_coordnates_location = shader_handler_ptr->get_attrib_location("texture_coordinates_attribute");
	
	glEnableVertexAttribArray(position_location);
	glEnableVertexAttribArray(texture_coordnates_location);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(position_location, 3, GL_FLOAT, false, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, texture_coordinates_buffer);
	glVertexAttribPointer(texture_coordnates_location, 2, GL_FLOAT, false, 0, 0);

	//Render plane x - y or plane z
	if (render_plane_z) {
		if (translation_location != -1) {
			glUniform3fv(translation_location, 1, glm::value_ptr(displacement * glm::vec3(0.0f, 0.0f, 1.0f)));
		}
		texture_1->bind();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	}
	
	//Render plane y - z or plane x
	if (render_plane_x) {
		if (translation_location != -1) {
		   glUniform3fv(translation_location, 1, glm::value_ptr(displacement * glm::vec3(1.0f, 0.0f, 0.0f)));
		}
		texture_2->bind();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, BUFFER_OFFSET(6 * sizeof(int)));
	}

	//Render plane x - z or plane y
	if (render_plane_y) {
		if (translation_location != -1) {
		   //Remember that y coordinate is inverted in volume vs world coordinates
		   glUniform3fv(translation_location, 1, glm::value_ptr(displacement * glm::vec3(0.0f, -1.0f, 0.0f)));
		}
		texture_3->bind();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, BUFFER_OFFSET(12 * sizeof(int)));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableVertexAttribArray(position_location);
	glDisableVertexAttribArray(texture_coordnates_location);

	glutSwapBuffers();
}

void create_geometry () {
   glGenBuffers(1, &vertex_buffer);
   glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * NUM_VERTICES, vertices, GL_STATIC_DRAW);

   glGenBuffers(1, &texture_coordinates_buffer);
   glBindBuffer(GL_ARRAY_BUFFER, texture_coordinates_buffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * NUM_VERTICES, texture_coordinates, GL_STATIC_DRAW);

   glGenBuffers(1, &index_buffer);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3 * NUM_FACES, faces, GL_STATIC_DRAW);

}

void init_OpenGL() {
   GLenum error = glewInit();
   if (GLEW_OK != error) {
	   /* Something failed at loading an extension */
	   cerr << "Error: " << glewGetErrorString(error) << endl;
	   exit(EXIT_FAILURE);
   }
   cout << "Hardware specification: " << endl;
   cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
   cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
   cout << "Software specification: " << endl;
   cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;
   cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
   cout << "Using GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
   int ver = glutGet(GLUT_VERSION);
   cout << "Using freeglut version: " <<  ver / 10000 << "." << (ver / 100) % 100 << "." << ver % 100 << endl;

   ilInit();
   iluInit();
	
   glClearColor(0.15f, 0.15f, 0.15f, 1.0);
   glEnable(GL_DEPTH_TEST);
  
   //create_image();

   //create vertex buffers for vertex coords, and tex coords
   create_geometry();
   //Lab computer
   shader_handler_ptr = new shaders::ShaderHandler("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
   shader_handler_ptr->use_program();
   
   //create and load volume
   //input_volume = new Volume(L"../Data/test_case/test_case2/w_", 2);
   //input_volume = new Volume(L"../Data/test_case/test_case4/t_", 4);
   //input_volume = new Volume(L"../Data/test_case/lines/lines", 9);
   //input_volume = new Volume(L"../Data/SL44_C4_4bar_Data/SL44_C4_4bar", 91);
   input_volume = new Volume(L"../Data/SL43_C5_1c5bar/SL43_C5_1c5bar_Data", 184);
   //input_volume = new Volume(L"../Data/Otsu_SingleRegion/ThresholdedOtsu_Cropped", 362);
   //input_volume = new Volume(L"../Data/Cross/Cross", 5);
   //input_volume = new Volume(L"../Data/testSmall/SquareImageStackSmall", 162);
   //input_volume = new Volume(L"../Data/NonSquareSmall/NonSquareSmall", 3);
   //input_volume = new Volume(L"../Data/sl47_NMC5_1c5bar/sl47_NMC5_1c5bar", 155);
   //input_volume = new Volume(L"../Data/SL51_LCO5_3bar/fullTH", 132);
   //input_volume = new Volume(L"../Data/Ns/Ns_", 3);
   //input_volume = new Volume(L"../Data/test_case/s_", 8);
   //input_volume->print_debug();
   //input_volume->save_to_raw(L"../Data/SL44_C4_4barRaw");
   //cout << *input_volume << endl;

   num_planes_x = input_volume->get_width();
   num_planes_y = input_volume->get_height();
   num_planes_z = input_volume->get_depth();
   x_plane = num_planes_x / 2;
   y_plane = num_planes_y / 2;
   z_plane = num_planes_z / 2;

   delta = glm::vec3(2.0f / num_planes_x, 2.0f / num_planes_y, 2.0f / num_planes_z);

   displacement = glm::vec3(delta.x * x_plane + delta.x / 2.0f, delta.y * y_plane + delta.y / 2.0f, delta.z * z_plane + delta.z / 2.0f);

   //Create texture for the three orthogonal planes.
   texture_1 = new TextureHandler(num_planes_x, num_planes_y);
   texture_2 = new TextureHandler(num_planes_z, num_planes_y);
   texture_3 = new TextureHandler(num_planes_x, num_planes_z);
   //Plane extraction from volume
   texture_1->update(input_volume->get_data_plane(z_plane, XY_PLANE));
   texture_2->update(input_volume->get_data_plane(x_plane, YZ_PLANE));
   texture_3->update(input_volume->get_data_plane(y_plane, XZ_PLANE));

   //set texture unit for uniform sampler variable
   texture_location = shader_handler_ptr->get_uniform_location("texture_map");
   if(texture_location != -1) {
      glUniform1i(texture_location, 0);
   }

   // Get location to uniform variables form cammera
   model_view_location = shader_handler_ptr->get_uniform_location("VM");
   projection_location = shader_handler_ptr->get_uniform_location("Proj");
   translation_location = shader_handler_ptr->get_uniform_location("displacement");

   glUseProgram(0);
}
	
void create_glut_window() {
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition (5, 5);
	int border_with = glutGet(GLUT_WINDOW_BORDER_WIDTH);
	int border_height = glutGet(GLUT_WINDOW_BORDER_HEIGHT);
	glutInitWindowSize (1000 + border_with, 1000 + border_height);
	win = glutCreateWindow ("Battery simulation project");
}

void create_glut_callbacks() {
	glutDisplayFunc(display);
	glutSpecialFunc(special_keys);
	glutKeyboardFunc(keyboard);
	glutMouseWheelFunc(mouse_wheel);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_active);
}

void exit_glut() {
	if (shader_handler_ptr) {
		delete shader_handler_ptr;
		shader_handler_ptr = nullptr;
	}

	if (input_volume) {
		delete input_volume;
		input_volume = nullptr;
	}

	if (texture_1) {
		delete texture_1;
		texture_1 = nullptr;
	}

	if (texture_2) {
		delete texture_2;
		texture_2 = nullptr;
	}

	if (texture_3) {
		delete texture_3;
		texture_3 = nullptr;
	}

	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &texture_coordinates_buffer);
	glDeleteBuffers(1, &index_buffer);
	
	glutDestroyWindow(win);
	exit(EXIT_SUCCESS);
}

void init_program() {
	scale_ortho = 1.0f;
	base_rotation_angles = glm::vec2(0.0, 0.0);
	new_rotation_angles = glm::vec2(0.0, 0.0);
	scale_perspective = 23.0;
	projection_type = false;
	render_plane_x = true;
	render_plane_y = true;
	render_plane_z = true;
}

void special_keys(int key, int mouse_x, int mouse_y) {
	switch (key) {
		//Remember that y coordinate is inverted between world and volume
		case GLUT_KEY_DOWN:
			if (y_plane < num_planes_y - 1) {
				++y_plane;
				texture_3->update(input_volume->get_data_plane(y_plane, XZ_PLANE));
				displacement.y = y_plane * delta.y + delta.y / 2.0f;
			}
		break;
		//Remember that y coordinate is inverted between world and volume
		case GLUT_KEY_UP:
			if (y_plane > 0) {
				--y_plane;
				texture_3->update(input_volume->get_data_plane(y_plane, XZ_PLANE));
				displacement.y = y_plane * delta.y + delta.y / 2.0f;
			}
		break;

		case GLUT_KEY_LEFT:
			if (x_plane > 0) {
				--x_plane;
				texture_2->update(input_volume->get_data_plane(x_plane, YZ_PLANE));
				displacement.x = x_plane * delta.x + delta.x / 2.0f;
			}
		break;

		case GLUT_KEY_RIGHT:
			if (x_plane < num_planes_x - 1) {
				++x_plane;
				texture_2->update(input_volume->get_data_plane(x_plane, YZ_PLANE));
				displacement.x = x_plane * delta.x + delta.x / 2.0f;
			}
		break;

		case GLUT_KEY_PAGE_DOWN:
			if (z_plane > 0) {
				--z_plane;
				texture_1->update(input_volume->get_data_plane(z_plane, XY_PLANE));
				displacement.z = z_plane * delta.z + delta.z / 2.0f;
			}
		break;

		case GLUT_KEY_PAGE_UP:
			if (z_plane < num_planes_z - 1) {
				++z_plane;
				texture_1->update(input_volume->get_data_plane(z_plane, XY_PLANE));
				displacement.z = z_plane * delta.z + delta.z / 2.0f;
			}
		break;
	}
	
	glutPostRedisplay();
}

void keyboard (unsigned char key, int mouse_x, int mouse_y) {
	switch (key) {
       case '1':
		   render_plane_z = !render_plane_z;
	   break;

	   case '2':
		   render_plane_x = !render_plane_x;
	   break;

	   case '3':
		   render_plane_y = !render_plane_y;
	   break;

	   case 'q':
	   case 'Q': 
	   case 27:
			exit_glut();
	   break;

	   case 'r':
	   case 'R':
		   init_program();
	   break;

	   case 'x':
	   case 'X':		  
		   base_rotation_angles = glm::vec2(45.0, 0.0);
		   new_rotation_angles = glm::vec2(0.0, 0.0);
	   break;

	   case 'y':
	   case 'Y':
		   base_rotation_angles = glm::vec2(0.0, 45.0);
		   new_rotation_angles = glm::vec2(0.0, 0.0);
	   break;


	   case 'z':
	   case 'Z':
		   base_rotation_angles = glm::vec2(0.0, 0.0);
		   new_rotation_angles = glm::vec2(0.0, 0.0);
	   break;

	   case 'p':
	   case 'P':
		   projection_type = (!projection_type);
       break;

		default:
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
		} else if (direction < 0 && scale_perspective > 10.0) {
			scale_perspective -= 5.0;
		}
		std::cout << "Scale: " << scale_perspective << std::endl;
	} else {
		if (direction > 0 && scale_ortho < 64.0) {
			scale_ortho *= 2.0;
		} else if (direction < 0 && scale_ortho > 0.125) {
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
		//cout << "base: " << base_rotation_angles.x << ", " << base_rotation_angles.y << endl;
		//cout << "new: " << new_rotation_angles.x << ", " << new_rotation_angles.y << endl;
	}
	glutPostRedisplay();
}

void create_image() {
	Volume* input_volume = new Volume(L"../../Cross.tif", 5);
	//input_volume = new Volume(L"../../TestCase2/TestCase-", 8);
	//input_volume->save_to_raw(L"../../SL44_C4_4bar_Data/SL44_C4_4barRaw");
	Image* input_image = new Image(L"../../skeleton.tif");
	//Image* input_image = new Image(L"../../test_case/TestCase-0003.tif");
	
	//Image* input_image = new Image(L"../TestCase.tif");
	std::vector<Cell> sphere_centers;
	input_image->get_path_cells(sphere_centers);
	//std::vector<std::pair<Cell, Cell>> blob_edges;
	//input_image->find_intersection_points(blob_edges);


	//texture = new Image(input_image->get_width(), input_image->get_height());
	//Create Blob and sampler
	Blob my_blob(0.04f, 20.0f);
	//Blob my_blob(1.5f);
	glm::vec3 minimun_world = glm::vec3(-1.0, -1.0, -1.0);
	glm::vec3 maximun_world = glm::vec3( 1.0,  1.0,  1.0);
	Sampler sampler(minimun_world, maximun_world);

	//Create spheres
	texture = new Image(512, 512);
	BlobSphere* spheres = new BlobSphere[sphere_centers.size()];
	int i = 0;
	for (auto center : sphere_centers) {
		//Remember that I need to translate to the min_world, however in 2D this is not world min because z needs to be 0.0
		spheres[i] = BlobSphere(&my_blob, center.get_world_position() + glm::vec3(-1.0, -1.0, 0.0));
		sampler.add_blob(&spheres[i++]);
	}


	//BlobCapsule* capsules = new BlobCapsule[blob_edges.size()];
	//int i = 0;
	//for (auto edge : blob_edges) {
	//	if (edge.first.get_world_position() != edge.second.get_world_position()) {
	//		//Remember that I need to translate to the min_world, however in 2D this is not world min because z needs to be 0.0
	//		capsules[i] = BlobCapsule(&my_blob, edge.first.get_world_position() + glm::vec3(-1.0, -1.0, 0.0), edge.second.get_world_position() + glm::vec3(-1.0, -1.0, 0.0));
	//		sampler.add_blob(&capsules[i++]);
	//	}
	//}
	/*glm::vec3 origin = glm::vec3(0.0, 0.0, 0.0);
	const unsigned short N = 8;
	BlobCapsule capsules[N];
	glm::mat4 identity;
	float angle = 360.0f / static_cast<float>(N);
	glm::vec3 axis_of_rotation = glm::vec3(0.0, 0.0, 1.0);
	for (int i = 0; i < N; ++i) {
		glm::vec4 result = glm::rotate(identity, angle * i, axis_of_rotation) * glm::vec4(0.0, 0.5, 0.0, 1.0);
		glm::vec3 p = glm::vec3(result.x, result.y, result.z);
		capsules[i] = BlobCapsule(&my_blob, origin, p);
		sampler.add_blob(&capsules[i]);
	}*/
	sampler.sample(*texture);
	texture->thresholding(0.5f);
	//delete [] capsules;
	delete [] spheres;
	delete input_image;
	delete input_volume;
}
