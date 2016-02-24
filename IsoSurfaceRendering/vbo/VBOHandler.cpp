#include "VBOHandler.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

namespace vbo {

	VBOHandler::VBOHandler() {
		//Generate new indexes
		glGenBuffers(1, &m_id);
		glGenBuffers(1, &m_index_id);
	}

	VBOHandler::VBOHandler(Mesh mesh) {
		//Generate new indexes
		glGenBuffers(1, &m_id);
		glGenBuffers(1, &m_index_id);

		//Copy vertex and index
		vector<glm::vec3> tmp_vertex = mesh.get_vertices();
		vector<unsigned int> tmp_indexes = mesh.get_indexes();

		//Extract some metadata from the set
		m_centroid = glm::vec3(0.0f, 0.0f, 0.0f);
		m_max_coordinates = tmp_vertex[0];
		m_min_coordinates = tmp_vertex[0];
		for (auto v : tmp_vertex) {
			m_centroid += v;
			
			if (v.x > m_max_coordinates.x) {
				m_max_coordinates.x = v.x;
			}
			if (v.y > m_max_coordinates.y) {
				m_max_coordinates.y = v.y;
			}
			if (v.z > m_max_coordinates.z) {
				m_max_coordinates.z = v.z;
			}

			if (v.x < m_min_coordinates.x) {
				m_min_coordinates.x = v.x;
			}
			if (v.y < m_min_coordinates.y) {
				m_min_coordinates.y = v.y;
			}
			if (v.z < m_min_coordinates.z) {
				m_min_coordinates.z = v.z;
			}
		}
		m_centroid /= tmp_vertex.size();

		//Create triangular faces from the mesh
		m_num_triangles = static_cast<unsigned int>(tmp_indexes.size() / 3);
		for (int i = 0; i < tmp_indexes.size(); i += 3) {
			
			//Extract triangular face coordinates
			glm::vec3 v1 = tmp_vertex[tmp_indexes[i]];
			glm::vec3 v2 = tmp_vertex[tmp_indexes[i + 1]];
			glm::vec3 v3 = tmp_vertex[tmp_indexes[i + 2]];
			
			//Calculate per face normal
			glm::vec3 n = glm::normalize(glm::cross(v3 - v2, v1 - v2));
			
			//Generate two triangles in an interleaved array
			//First triangle v1, v2, v3
			m_vertex.push_back(v1);
			m_vertex.push_back(n);
			m_vertex.push_back(v2);
			m_vertex.push_back(n);
			m_vertex.push_back(v3);
			m_vertex.push_back(n);
			
		}

		//Map the data into vbo
		int num_floats = static_cast<int>(m_vertex.size() * 3);
		int data_size = num_floats * sizeof(float);
		glBindBuffer(GL_ARRAY_BUFFER, m_id);

		glBufferData(GL_ARRAY_BUFFER, data_size, &m_vertex[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	//VBOHandler::VBOHandler(vector<glm::vec3> vertex, vector<unsigned int> indexes) {

	//}

	void VBOHandler::draw(GLint a_position_location, GLint a_normal_location) {
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		int stride = (3 + 3) * sizeof(float);
		//int stride = 0;
		glEnableVertexAttribArray(a_position_location);
		glVertexAttribPointer(a_position_location, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(a_normal_location);
		glVertexAttribPointer(a_normal_location, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(3 * sizeof(float)));
		glDrawArrays(GL_TRIANGLES, 0, m_num_triangles * 3);
		glDisableVertexAttribArray(a_position_location);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/*void VBOHandler::update(vector<glm::vec3> vertex, vector<unsigned int> indexes) {

	}*/

	VBOHandler::VBOHandler(const VBOHandler& copy) {
		
		//Allocate new memory

		//copy static members
		m_id = copy.m_id;
		m_index_id = copy.m_index_id;

		//copy dynamic members
		m_indexes = copy.m_indexes;
		m_vertex = copy.m_vertex;
		m_normals = copy.m_normals;
	}

	VBOHandler VBOHandler::operator=(const VBOHandler& rhs) {
		//Prevent self assignation
		if (this == &rhs) {
			return *this;
		}
		//Free resources
		m_vertex.clear();
		m_normals.clear();
		m_indexes.clear();

		//Allocate new memory

		//copy static members
		m_id = rhs.m_id;
		m_index_id = rhs.m_index_id;

		//copy dynamic members
		m_indexes = rhs.m_indexes;
		m_vertex = rhs.m_vertex;
		m_normals = rhs.m_normals;

		return *this;
	}
	
	VBOHandler::~VBOHandler() {
		m_indexes.clear();
		m_normals.clear();
		m_vertex.clear();
	}

	glm::vec3 VBOHandler::get_centroid() {
		return m_centroid;
	}

	glm::vec3 VBOHandler::get_max_coordinates() {
		return m_max_coordinates;
	}

	glm::vec3 VBOHandler::get_min_coordinates() {
		return m_min_coordinates;
	}
}