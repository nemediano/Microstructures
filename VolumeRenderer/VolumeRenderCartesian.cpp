/**************************************************************************
*        FILE NAME:  VolumeRenderCartesian.cpp
*
* ONE LINE SUMMARY:
*        Tested on NVIDIA Quadro FX3000
*
* DESCRIPTION:
*        Demonstrates use of 3D textures for volume rendering.
*        Does not require Cg or programmable GPU. Only requires
*        GL_EXT_texture3D OpenGL extension.
*
*    Thilaka Sumanaweera
*    (C) 2002 Siemens Medical Solutions USA, Inc, Ultrasound Division
*    Thilaka.Sumanaweera@siemens.com
*    05/16/2003
*
*****************************************************************************
* REVISION HISTORY:
* Rev     	When      	Who         	What
* A	  	 16May03        sumane		    Created.
****************************************************************************/
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>

#include <IL/il.h>
#include <IL/ilu.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <windows.h>


#include "digitalimage/Volume.h"

//Mouse camera control
bool mouse_dragging;
glm::vec2 mouse_start_drag;
//glm::vec2 base_rotation_angles;
glm::vec2 new_rotation_angles;

GLubyte *image;
GLuint texName;
GLuint listName;
int iWidth;
int iHeight;
int iDepth;
float alpha = -85.0f;
float theta =  90.0f;
float gamma =   0.0f;
float scale =   0.7f;
char step_p = 1;
//Number of slices
int slices = 256;

using namespace std;

void init_OpenGL();
unsigned char* apply_transfer_function(digitalimage::Volume* vol, bool put_frame);
void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y);
void mouse_active(int mouse_x, int mouse_y);
void mouse(int button, int state, int mouse_x, int mouse_y);

/*************************[MAN-BEG]*******************************************
*
* NAME:
*   ReadHeader
*
* DESCRIPTION:
*
*   This reads in the header of the 3D data volume
*
* FORMAL PARAMETERS:
*   prefix contains the file prefix.
*
* RETURNS:
*   w, h, and d are width, height and depth of the volume
*
* REVISION HISTORY:
* Rev     When      Who         What
* A	   16May03      sumane		Created.
**************************[MAN-END]******************************************/
void ReadHeader(char *prefix, int &w, int &h, int &d)
{
	char file[300];
	ifstream ifile;

	sprintf(file, "%s.hdr", prefix);
	cerr << file << endl;
//	ifile.open(file, ios::in | ios::nocreate);
	ifile.open(file, ios::in);

	if (ifile.fail()) {
		cerr << "Could not open " << file << endl;
		exit(0);
	}
	ifile >> w;
	ifile >> h;
	ifile >> d;
	ifile.close();
}

/*************************[MAN-BEG]*******************************************
*
* NAME:
*   ReadVolume
*
* DESCRIPTION:
*
*   This reads in the 3D data volume
*
* FORMAL PARAMETERS:
*   prefix contains the file prefix.
*
* RETURNS:
*
* REVISION HISTORY:
* Rev     When      Who         What
* A	   16May03      sumane		Created.
**************************[MAN-END]******************************************/
void ReadVolume(char *prefix)
{
	char file[100];
	ifstream ifile;
	
	ReadHeader(prefix, iWidth, iHeight, iDepth);
	cerr << "Volume size: [" << iWidth << "x" << iHeight << "x" << iDepth << "]" << endl;
	
	sprintf(file, "%s.img", prefix);
	int Size = iWidth*iHeight*iDepth;

	GLubyte *image_ = new GLubyte [Size];
	image           = new GLubyte [Size*4];

	ifile.open(file, ios::in | ios::binary);
	if (ifile.fail()) {
		cerr << "Could not open " << file << endl;
		exit(0);
	}
	ifile.read((char *)image_, Size);
	ifile.close();

	// Put a box around the volume so that we can see the outline 
	// of the data.
	//
	GLubyte *ptr = image_;
	int i, j, k;
    for (i = 0; i < iDepth; i++) {
        for (j = 0; j < iHeight; j++) {
            for (k = 0; k < iWidth; k++) {
				if (((i < 4) && (j < 4)) ||
					((j < 4) && (k < 4)) ||
					((k < 4) && (i < 4)) ||
					((i < 4) && (j >  iHeight-5)) ||
					((j < 4) && (k > iWidth-5)) ||
					((k < 4) && (i > iDepth-5)) ||
					((i > iDepth-5) && (j >  iHeight-5)) ||
					((j >  iHeight-5) && (k > iWidth-5)) ||
					((k > iWidth-5) && (i > iDepth-5)) ||
					((i > iDepth-5) && (j < 4)) ||
					((j >  iHeight-5) && (k < 4)) ||
					((k > iWidth-5) && (i < 4))) {
					*ptr = 110;
				}
				ptr++;
			}
		}
	}

	ptr = image_;
	GLubyte *qtr = image;
	GLubyte v, val;
	for (i = 0; i < Size; i++) {
		v = *(ptr++);
		//Dealing from signed to unsigned byte?? -Jorge
		val = (v < 64) ? 0 : v - 64;
		val = val >> 1;
		*(qtr++) = val;
		*(qtr++) = ((float)val)*0.93f;
		*(qtr++) = ((float)val)*0.78f;
		*(qtr++) = val;
	}
	delete [] image_;
}

/*************************[MAN-BEG]*******************************************
*
* NAME:
*   SetDisplayList
*
* DESCRIPTION:
*
*   Creates a display list containing all the enclosing rectangles 
*   corresponding to the cutplanes.
*
* FORMAL PARAMETERS:
*   None
*
* RETURNS:
*   None
*
* REVISION HISTORY:
* Rev     When      Who         What
* A	   16May03      sumane		Created.
**************************[MAN-END]******************************************/
void SetDisplayList(void)
{
   float dist;
   int i;
   int nHSlices     = 1.3f*(sqrt(float(iWidth*iWidth + iHeight*iHeight + iDepth*iDepth)))/0.5f;
   int nSlices      = 2*nHSlices + 1;
   int nElements    = nSlices*4;
   GLfloat *vPoints = new GLfloat [3*nElements];
   GLfloat *ptr     = vPoints;
   float dDist      = sqrt(3.0f)/nSlices;
   float s          = 0.5;
   
   for (i = -nHSlices; i <= nHSlices; i++) {
	   dist     = i*dDist;

	   *(ptr++) = -s;
	   *(ptr++) = -s;
	   *(ptr++) =  dist;

	   *(ptr++) =  s;
	   *(ptr++) = -s;
	   *(ptr++) =  dist;

	   *(ptr++) =  s;
	   *(ptr++) =  s;
	   *(ptr++) =  dist;

	   *(ptr++) = -s;
	   *(ptr++) =  s;
	   *(ptr++) =  dist;
   }

   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, vPoints);
   listName = glGenLists(1);
   glNewList(listName, GL_COMPILE);
   glDrawArrays(GL_QUADS, 0, nElements); 
   glEndList();
   delete [] vPoints;
}

//Nasty overload to fix to my data
void SetDisplayList(int slices) {
	float dist;
   int i;
   int nHSlices     = slices;
   int nSlices      = 2*nHSlices + 1;
   int nElements    = nSlices*4;
   GLfloat *vPoints = new GLfloat [3*nElements];
   GLfloat *ptr     = vPoints;
   float dDist      = sqrt(3.0f)/nSlices;
   float s          = 0.5;
   
   for (i = -nHSlices; i <= nHSlices; i++) {
	   dist     = i*dDist;

	   *(ptr++) = -s;
	   *(ptr++) = -s;
	   *(ptr++) =  dist;

	   *(ptr++) =  s;
	   *(ptr++) = -s;
	   *(ptr++) =  dist;

	   *(ptr++) =  s;
	   *(ptr++) =  s;
	   *(ptr++) =  dist;

	   *(ptr++) = -s;
	   *(ptr++) =  s;
	   *(ptr++) =  dist;
   }

   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, vPoints);
   listName = glGenLists(1);
   glNewList(listName, GL_COMPILE);
   glDrawArrays(GL_QUADS, 0, nElements); 
   glEndList();
   delete [] vPoints;
}

/*************************[MAN-BEG]*******************************************
*
* NAME:
*   Init
*
* DESCRIPTION:
*
*   Initializes OpenGL state machine
*
* FORMAL PARAMETERS:
*   None
*
* RETURNS:
*   None
*
* REVISION HISTORY:
* Rev     When      Who         What
* A	   16May03      sumane		Created.
**************************[MAN-END]******************************************/
void Init(void)
{    
   using namespace digitalimage;
   //ReadVolume("../Data/CT/CTData");
   //ReadVolume("../Data/SL44_C4_4barRaw");
   
   //Dont forget to call this ones before constructor!!!
   ilInit();
   iluInit();
   //Volume* input_volume = new Volume(L"../Data/SL44_C4_4bar_Data/SL44_C4_4bar", 91);
   //Volume* input_volume = new Volume(L"../Data/Otsu_SingleRegion/ThresholdedOtsu_Cropped", 362);
   Volume* input_volume = new Volume(L"D:/KaimoHU/projects/BatterySimulation/Data/GraphiteTomoData/SL43_C5_1c5bar_Data/SL43_C5_1c5bar", 184);
   //Volume* input_volume = new Volume(L"../Data/test_case/test_case2/w_", 2);
   //Volume* input_volume = new Volume(L"../Data/test_case/test_case4/t_", 4);
   //Volume* input_volume = new Volume(L"../Data/test_case/lines/lines", 9);
   //Volume* input_volume = new Volume(L"../Data/testSmall/SquareImageStackSmall", 185);
   //Volume* input_volume = new Volume(L"../Data/Ns/Ns_", 3);
   //Volume* input_volume = new Volume(L"../Data/TestCaseNs/TestCaseNs-", 5);

   glClearColor (0.15, 0.15, 0.15, 0.0);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glGenTextures(1, &texName);
   glBindTexture(GL_TEXTURE_3D, texName);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   // For automatic texture coordinate generation
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
   glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
   glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

   //glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, iWidth, iHeight, iDepth, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
   //unsigned char* buffer = input_volume->get_pointer_to_data_rgba();
   unsigned char* buffer = apply_transfer_function(input_volume, false);
   glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, input_volume->get_width(), input_volume->get_height(), input_volume->get_depth(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)buffer);
   iWidth = input_volume->get_width();
   iHeight = input_volume->get_height();
   iDepth = input_volume->get_depth();

   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T);
   glEnable(GL_TEXTURE_GEN_R);
   glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);

   //SetDisplayList(slices);
   SetDisplayList();

   // Try to save the file to raw data file format
   input_volume->save_to_medfile(L"D:/KaimoHU/projects/BatterySimulation/Data/GraphiteTomoData/SL43_C5_1c5bar_Data/SL43_C5_1c5bar");

   delete input_volume;
   delete[] buffer;
}

unsigned char* apply_transfer_function(digitalimage::Volume* vol, bool put_frame) {
	
	unsigned char* temp_buffer = new unsigned char[4 * vol->get_voxel_number()];

	float red_factor = 1.0f;
	float green_factor = 1.0f;
	float blue_factor = 1.0f;
	float alpha_factor = 0.0f;

	glm::vec3 white = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 black = glm::vec3(0.0, 0.0, 0.0);

	for (int k = 0; k < vol->get_depth(); ++k)
	{
		for (int i = 0; i < vol->get_width(); ++i) 
		{
			for (int j = 0; j < vol->get_height(); ++j) 
			{
				glm::vec3 voxel_color = vol->get_color_at(i, j, k);

				int real_index_rgba = 4 * (i + vol->get_width() * (j + vol->get_height() * k));
				if (voxel_color == black) 
				{
					temp_buffer[real_index_rgba] = 0;
					temp_buffer[real_index_rgba + 1] = 0;
					temp_buffer[real_index_rgba + 2] = 0;
					temp_buffer[real_index_rgba + 3] = 64;
				} 
				else if (voxel_color == white) 
				{
					temp_buffer[real_index_rgba]     = 255;
					temp_buffer[real_index_rgba + 1] = 255;
					temp_buffer[real_index_rgba + 2] = 255;
					temp_buffer[real_index_rgba + 3] = 64;
				} 
				else 
				{
					//something very bad happen!
					cerr << "Abort!! abort!!" << endl;
				}
				
				/*int real_index_rgba = 4 * (i + vol->get_width() * (j + vol->get_height() * k));
				temp_buffer[real_index_rgba] = static_cast<unsigned char>(255.0f * red_factor * voxel_color.r);
				temp_buffer[real_index_rgba + 1] = static_cast<unsigned char>(255.0f * green_factor * voxel_color.g);
				temp_buffer[real_index_rgba + 2] = static_cast<unsigned char>(255.0f * blue_factor * voxel_color.b);
				temp_buffer[real_index_rgba + 3] = static_cast<unsigned char>((alpha_factor / 3.0f) * (voxel_color.r + voxel_color.g + voxel_color.b));*/
				
				if (put_frame) 
				{
					int frame_thickness = 3;
					if (((i < frame_thickness) && (j < frame_thickness)) ||
						((j < frame_thickness) && (k < frame_thickness)) ||
						((k < frame_thickness) && (i < frame_thickness)) ||
						((i < frame_thickness) && (j >  vol->get_height() - frame_thickness + 1)) ||
						((j < frame_thickness) && (k > vol->get_width() - frame_thickness + 1)) ||
						((k < frame_thickness) && (i > vol->get_depth()  -frame_thickness + 1)) ||
						((i > vol->get_depth() - frame_thickness + 1) && (j >  vol->get_height() - frame_thickness + 1)) ||
						((j >  vol->get_height() - frame_thickness + 1) && (k > vol->get_width() - frame_thickness + 1)) ||
						((k > vol->get_width() - frame_thickness + 1) && (i > vol->get_depth() - frame_thickness + 1)) ||
						((i > vol->get_depth() - frame_thickness + 1) && (j < frame_thickness)) ||
						((j >  vol->get_height() - frame_thickness + 1) && (k < frame_thickness)) ||
						((k > vol->get_width() - frame_thickness + 1) && (i < frame_thickness))) {

					temp_buffer[real_index_rgba] = 255;
					temp_buffer[real_index_rgba + 1] = 0;
					temp_buffer[real_index_rgba + 2] = 0;
					temp_buffer[real_index_rgba + 3] = 255;
					}
				}
			}
		}
	}
	return temp_buffer;
}

/*************************[MAN-BEG]*******************************************
*
* NAME:
*   Display
*
* DESCRIPTION:
*
*   Display routine for OpenGL
*
* FORMAL PARAMETERS:
*   None
*
* RETURNS:
*   None
*
* REVISION HISTORY:
* Rev     When      Who         What
* A	   16May03      sumane		Created.
**************************[MAN-END]******************************************/
void Display(void)
{
   glEnable(GL_TEXTURE_3D);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   glBindTexture(GL_TEXTURE_3D, texName);

   glClear(GL_COLOR_BUFFER_BIT);

   // Clip planes
   static GLdouble eqx0[4] = { 1.0, 0.0, 0.0, 0.0};
   static GLdouble eqx1[4] = {-1.0, 0.0, 0.0, 1.0};
   static GLdouble eqy0[4] = {0.0,  1.0, 0.0, 0.0};
   static GLdouble eqy1[4] = {0.0, -1.0, 0.0, 1.0};
   static GLdouble eqz0[4] = {0.0, 0.0,  1.0, 0.0};
   static GLdouble eqz1[4] = {0.0, 0.0, -1.0, 1.0};
   
   // Define equations for automatic texture coordinate generation
   static GLfloat x[] = {1.0, 0.0, 0.0, 0.0};
   static GLfloat y[] = {0.0, 1.0, 0.0, 0.0};
   static GLfloat z[] = {0.0, 0.0, 1.0, 0.0};

   
   glPushMatrix ();
   glScalef(scale, scale, scale);

   glPushMatrix ();

   // Transform the viewing direction
   glRotatef(-gamma, 0.0, 0.0, 1.0);
   glRotatef(theta + new_rotation_angles.x, 0.0, 1.0, 0.0);
   glRotatef(alpha + new_rotation_angles.y, 1.0, 0.0, 0.0);
   glTranslatef(-0.5, -0.5, -0.5);

   // Take a copy of the model view matrix now shove it in to the GPU 
   // buffer for later use in automatic texture coord generation.
   glTexGenfv(GL_S, GL_EYE_PLANE, x);
   glTexGenfv(GL_T, GL_EYE_PLANE, y);
   glTexGenfv(GL_R, GL_EYE_PLANE, z);

   // Take a copy of the model view matrix and shove it in to the GPU
   // buffer for later use in clipping planes.
   glClipPlane(GL_CLIP_PLANE0, eqx0);
   glClipPlane(GL_CLIP_PLANE1, eqx1);

   glClipPlane(GL_CLIP_PLANE2, eqy0);
   glClipPlane(GL_CLIP_PLANE3, eqy1);

   glClipPlane(GL_CLIP_PLANE4, eqz0);
   glClipPlane(GL_CLIP_PLANE5, eqz1);
   
   glPopMatrix ();

   // Enable clip planes
   glEnable(GL_CLIP_PLANE0);
   glEnable(GL_CLIP_PLANE1);

   glEnable(GL_CLIP_PLANE2);
   glEnable(GL_CLIP_PLANE3);

   glEnable(GL_CLIP_PLANE4);
   glEnable(GL_CLIP_PLANE5);
   
   // Render enclosing rectangles
   glCallList(listName);
 
   glPopMatrix ();
   glutSwapBuffers();

   glDisable(GL_TEXTURE_3D);
}

/*************************[MAN-BEG]*******************************************
*
* NAME:
*   Idle
*
* DESCRIPTION:
*
*   Idle routine for OpenGL
*
* FORMAL PARAMETERS:
*   None
*
* RETURNS:
*   None
*
* REVISION HISTORY:
* Rev     When      Who         What
* A	   16May03      sumane		Created.
**************************[MAN-END]******************************************/
void Idle(void)
{
	static clock_t thisClock, startClock = clock();
	static int numTimes = 0;
	int avgCount        = 20;
	float duration;

	if (avgCount == numTimes) {
		thisClock = clock();
		duration = (float)(thisClock - startClock)/ CLOCKS_PER_SEC;
		startClock = thisClock;
		numTimes = 0;
		//cerr << "Frame Rate = " << ((float)avgCount)/duration << endl;
	}

	Display();

	if (!step_p) {
		theta += 5.0f;
		theta = (theta >= 360.0f) ? 0.0f : theta;
	}

	numTimes++;
}

/*************************[MAN-BEG]*******************************************
*
* NAME:
*   Reshape
*
* DESCRIPTION:
*
*   Reshape routine for OpenGL
*
* FORMAL PARAMETERS:
*   w and h are width and height of the window
*
* RETURNS:
*   None
*
* REVISION HISTORY:
* Rev     When      Who         What
* A	   16May03      sumane		Created.
**************************[MAN-END]******************************************/
void Reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   GLfloat size_ = 1.0;
   GLfloat aspect = (GLfloat) h/(GLfloat) w;
   glOrtho(-size_, size_, -size_*aspect, size_*aspect, -2.0 * size_, 2.0 * size_);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

/*************************[MAN-BEG]*******************************************
*
* NAME:
*   PrintHelp
*
* DESCRIPTION:
*
*   Prints a help line
*
* FORMAL PARAMETERS:
*   None
*
* RETURNS:
*   None
*
* REVISION HISTORY:
* Rev     When      Who         What
* A	   16May03      sumane		Created.
**************************[MAN-END]******************************************/
void PrintHelp(void)
{
	cerr << endl;
	cerr << "Help:" << endl;
	cerr << endl;
	cerr << "x/X: Increase or decrease Alpha" << endl;
	cerr << "y/Y: Increase or decrease Theta" << endl;
	cerr << "g/G: Increase or decrease Gamma" << endl;
	cerr << "z/Z: Zoom in or out" << endl;
	cerr << "s  : Stop/start automatic rotation" << endl;
	cerr << "ESC: Quit" << endl;
	cerr << endl;
}

/*************************[MAN-BEG]*******************************************
*
* NAME:
*   Keyboard
*
* DESCRIPTION:
*
*   Handles keybord commands
*
* FORMAL PARAMETERS:
*   key is the key pressed by the user.
*   (x, y) are the coordinates of the mouse
*
* RETURNS:
*   None
*
* REVISION HISTORY:
* Rev     When      Who         What
* A	   16May03      sumane		Created.
**************************[MAN-END]******************************************/
void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	  case 'h':
	      PrintHelp();
	      break;
	  case 'x':
		  alpha += 10.0f;
		  alpha = (alpha >= 360.0f) ? 0.0f : alpha;
		  cerr << "Alpha: " << alpha << endl;
		  break;
	  case 'X':
		  alpha -= 10.0f;
		  alpha = (alpha < 0.0f) ? 360.0f + alpha : alpha;
		  cerr << "Alpha: " << alpha << endl;
		  break;
	  case 'y':
		  theta += 10.0f;
		  theta = (theta >= 360.0f) ? 0.0f : theta;
		  cerr << "Theta: " << theta << endl;
		  break;
	  case 'Y':
		  theta -= 10.0f;
		  theta = (theta < 0.0f) ? 360.0f + theta : theta;
		  cerr << "Theta: " << theta << endl;
		  break;
	  case 'g':
		  gamma += 10.0f;
		  gamma = (gamma >= 360.0f) ? 0.0f : gamma;
		  cerr << "Gamma: " << gamma << endl;
		  break;
	  case 'G':
		  gamma -= 10.0f;
		  gamma = (gamma < 0.0f) ? 360.0f + gamma : gamma;
		  cerr << "Gamma: " << gamma << endl;
		  break;
	  case 'z':
		  scale *= 0.9f;
		  cerr << "ZoomFactor: " << scale << endl;
		  break;
	  case 'Z':
		  scale *= 1.1f;
		  cerr << "ZoomFactor: " << scale << endl;
		  break;
	  case 's':
		  step_p = !step_p;
		  break;
      case 27:
		  delete [] image;
         exit(0);
         break;
      case '+':
		  slices++;
		  cout << "Slices = " << slices << endl;
		  SetDisplayList(slices);
         break;
      case '-':
		  slices--;
		  if (slices < 1) {
			  slices = 1;
		  }
		  cout << "Slices = " << slices << endl;
		  SetDisplayList(slices);
         break;
   }
}

/*************************[MAN-BEG]*******************************************
*
* NAME:
*   main
*
* DESCRIPTION:
*
* This is the main program that starts the app.
*
* FORMAL PARAMETERS:
*  none
*
* RETURNS:
*   None
*
* REVISION HISTORY:
* Rev     When      Who         What
* A	   16May03      sumane		Created.
**************************[MAN-END]******************************************/
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(640, 480);
   glutInitWindowPosition(100, 100);
   glutCreateWindow(argv[0]);
   
   init_OpenGL();

   Init();
   glutDisplayFunc(Display);
   glutReshapeFunc(Reshape);
   glutIdleFunc(Idle);
   glutKeyboardFunc (Keyboard);
   glutMouseWheelFunc(mouse_wheel);
   glutMouseFunc(mouse);
   glutMotionFunc(mouse_active);


   glutMainLoop();

   return 0; 
}


void init_OpenGL() {
   GLenum error = glewInit();
   if (error != GLEW_OK) {
	   /* Something failed at loading an extension */
	   cerr << "Error: " << glewGetErrorString(error) << endl;
	   exit(EXIT_FAILURE);
   }
   cout << "Hardware specification: " << endl;
   cout << "GPU: " << glGetString(GL_RENDERER) << endl;
   cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
   cout << "Software specification: " << endl;
   cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
   cout << "Using GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
   int ver = glutGet(GLUT_VERSION);
   cout << "Using freeglut version: " <<  ver / 10000 << "." << (ver / 100) % 100 << "." << ver % 100 << endl;
   cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;

   if (glewGetExtension("GL_EXT_texture3D")) 
   {
		cout << "GL_EXT_texture3D supported!" << endl;
   }
   else 
   {
		cerr << "GL_EXT_texture3D unsupported!" << endl;
		exit(EXIT_FAILURE);
   }
}

void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y) {
	
	if (direction > 0) 
	{
		scale *= 1.1;
	} else if (direction < 0) 
	{
		scale *= 0.9;
	}
	std::cout << "ZoomFactor: " << scale << std::endl;
	
	glutPostRedisplay();
}

void mouse_active(int mouse_x, int mouse_y)
{
	glm::vec2 mouse_current;
	
	mouse_current.x = static_cast<float>(mouse_x);
	mouse_current.y = static_cast<float>(mouse_y);
	
	glm::vec2 deltas = mouse_start_drag - mouse_current;

	new_rotation_angles.x = -deltas.x / glutGet(GLUT_WINDOW_WIDTH) * 180;
	new_rotation_angles.y = deltas.y / glutGet(GLUT_WINDOW_HEIGHT) * 180;

	glutPostRedisplay();
}

void mouse(int button, int state, int mouse_x, int mouse_y) 
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) 
	{
		mouse_dragging = true;
		mouse_start_drag.x = static_cast<float>(mouse_x);
		mouse_start_drag.y = static_cast<float>(mouse_y);
	} 
	else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) 
	{
		mouse_dragging = false;
		//base_rotation_angles += new_rotation_angles;
		
		theta += new_rotation_angles.x;
		alpha += new_rotation_angles.y;

		new_rotation_angles = glm::vec2(0.0, 0.0);
	}
	glutPostRedisplay();
}
