#ifndef VOLUME_GEOMETRY_H_
#define VOLUME_GEOMETRY_H_

const unsigned int NUM_FACES = 6;
const unsigned int NUM_VERTICES = 12;

float vertices[3 * NUM_VERTICES] = {
	//First Quad
	-1.0f,  1.0f, -1.0f,  //0
	-1.0f, -1.0f, -1.0f,  //1
	1.0f, -1.0f, -1.0f,   //2
	1.0f,  1.0f, -1.0f,   //3
	 //Second Quad
	 -1.0f, -1.0f, 1.0f,  //4
	 -1.0f, -1.0f, -1.0f, //5
	 -1.0f, 1.0f, -1.0f,  //6
	 -1.0f, 1.0f,  1.0f,  //7
	 //Third Quad
	 //Remember that y coordinate is inverted in volume vs world coordinates
	-1.0f, 1.0f,  1.0f,  //8
	-1.0f, 1.0f, -1.0f,  //9
	 1.0f, 1.0f, -1.0f,  //10
	 1.0f, 1.0f, 1.0f   //11
};

//const float texture_coordinates[2 * NUM_VERTICES] = {
//	0.0, 0.0,
//	0.0, 1.0,
//	1.0, 1.0,
//	1.0, 0.0,
//
//	0.0, 0.0,
//	0.0, 1.0,
//	1.0, 1.0,
//	1.0, 0.0,
//
//	0.0, 0.0,
//	0.0, 1.0,
//	1.0, 1.0,
//	1.0, 0.0
//};


const float texture_coordinates[2 * NUM_VERTICES] = {
	//First Quad
	0.0, 0.0, //0
	0.0, 1.0, //1
	1.0, 1.0, //2
	1.0, 0.0, //3
	//Second Quad
	1.0, 1.0, //4
	0.0, 1.0, //5
	0.0, 0.0, //6
	1.0, 0.0, //7
	//Third Quad
	0.0, 1.0, //8
	0.0, 0.0, //9
	1.0, 0.0, //10
	1.0, 1.0  //11
};

const unsigned int faces[3 * NUM_FACES] = {
	//First Quad
	0, 1, 2,
	2, 3, 0,
	//Second Quad
	4, 5, 6,
	6, 7, 4,
	//Third Quad
	8, 9, 10,
	10, 11, 8,
};

#endif