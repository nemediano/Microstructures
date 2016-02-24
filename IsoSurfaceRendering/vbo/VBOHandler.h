#ifndef VBO_HANDLER_H_
#define VBO_HANDLER_H_

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

#include "meshes/Mesh.h"
#include "../shaders/ShaderHandler.h"

#include <string>
#include <vector>
using namespace std;
using namespace meshes;

//This Handler can only use interleaved vertex
//And produces no indexed meshes

namespace vbo {
	class VBOHandler {
	protected:
		GLuint m_id;
		GLuint m_index_id;
		vector<glm::vec3> m_vertex;
		vector<glm::vec3> m_normals;
		vector<unsigned int> m_indexes;
		glm::vec3 m_centroid;
		glm::vec3 m_min_coordinates;
		glm::vec3 m_max_coordinates;
		unsigned int m_num_triangles;
	public:
		VBOHandler();
		VBOHandler(Mesh mesh);
		//VBOHandler(vector<glm::vec3> vertex, vector<unsigned int> indexes);
		VBOHandler(const VBOHandler& copy);
		VBOHandler operator=(const VBOHandler& rhs);
		glm::vec3 get_centroid();
		glm::vec3 get_max_coordinates();
		glm::vec3 get_min_coordinates();
		~VBOHandler();
		void draw(GLint a_position_location, GLint a_normal_location);
		//void update(vector<glm::vec3> vertex, vector<unsigned int> indexes);
	};
}
#endif