#include "Mesh.h"
#include <set>
#include <iostream>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
namespace meshes {
	Mesh::Mesh() : m_num_triangles(0), m_num_edges(0), m_num_vertex(0) {
		m_lower_corner = 1e10f * glm::vec3(1.0f);
		m_upper_corner = -1e10f * glm::vec3(1.0f);
		m_dimensions = glm::vec3(0.0f);
	}

	Mesh::Mesh(const Mesh& copy) {
		//Free memory. 
		m_vertices.clear();
		m_index.clear();
		//m_normals.clear();
		//Copy static members
		m_num_edges = copy.m_num_edges;
		m_num_triangles = copy.m_num_triangles;
		m_num_vertex = copy.m_num_vertex;
		m_dimensions = copy.m_dimensions;
		m_lower_corner = copy.m_lower_corner;
		m_upper_corner = copy.m_upper_corner;
		//Allocate new memory

		//Copy dynamic members in new memory
		m_vertices = copy.m_vertices;
		m_index = copy.m_index;
		//m_normals = copy.m_normals;
		m_tmp_triangles = copy.m_tmp_triangles;

		
	}

	Mesh& Mesh::operator=(const Mesh& rhs) {
		//Prevent for self assignment
		if (this == &rhs) {
			return *this;
		}
		//Free memory. 
		m_vertices.clear();
		m_index.clear();
		//m_normals.clear();
		
		//Copy static members
		m_num_edges = rhs.m_num_edges;
		m_num_triangles = rhs.m_num_triangles;
		m_num_vertex = rhs.m_num_vertex;
		//Allocate new memory

		//Copy dynamic members in new memory
		m_vertices = rhs.m_vertices;
		m_index = rhs.m_index;
		//m_normals = rhs.m_normals;
		m_tmp_triangles = rhs.m_tmp_triangles;

		return *this;
	}

	Mesh::~Mesh() {
		m_vertices.clear();
		m_index.clear();
		//m_normals.clear();
	}

	void Mesh::add_triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		m_tmp_triangles.push_back(p1);
		m_tmp_triangles.push_back(p2);
		m_tmp_triangles.push_back(p3);
		update_bb(p1);
		update_bb(p2);
		update_bb(p3);
	}

	void Mesh::add_quad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4) {
		//First triangle of the quad
		m_tmp_triangles.push_back(p1);
		m_tmp_triangles.push_back(p2);
		m_tmp_triangles.push_back(p3);
		//Second triangle of the quad
		m_tmp_triangles.push_back(p1);
		m_tmp_triangles.push_back(p3);
		m_tmp_triangles.push_back(p4);
		
		update_bb(p1);
		update_bb(p2);
		update_bb(p3);
		update_bb(p4);

	}

	void Mesh::update() {
		
		if (m_tmp_triangles.size() == 0) {
			return;
		}

		m_dimensions = m_upper_corner - m_lower_corner;

		m_vertices.clear();
		m_index.clear();

		m_num_edges = 0;
		m_num_triangles = 0;
		m_num_vertex = 0;

		bool (*comparator_function_wraper_ptr)(std::pair<glm::vec3, unsigned int> lhs, std::pair<glm::vec3, unsigned int> rhs) = comparator_function_wrapper;
		bool (*comparator_function_ptr)(glm::vec3 lhs, glm::vec3 rhs) = comparator_function;
		std::set<std::pair<glm::vec3, unsigned int>, bool (*)(std::pair<glm::vec3, unsigned int> lhs, std::pair<glm::vec3, unsigned int> rhs)> table(comparator_function_wraper_ptr);
		{
			//First get rid of repeated vertex using a set	
			std::set<glm::vec3, bool (*)(glm::vec3 lhs, glm::vec3 rhs)> tmp_set(comparator_function_ptr);
			
			for (auto vertex : m_tmp_triangles) {
				tmp_set.insert(vertex);
			}

			//Using one set to fill the other
			unsigned int index = 0;
			for (auto vertex : tmp_set) {
				table.insert(std::make_pair(vertex, index++));
				m_vertices.push_back(vertex);
			}

		}

		//Update index
		m_num_triangles = 0;
		for (int i = 0; i < m_tmp_triangles.size(); i += 3) {
			//First vertex of triangle
			auto it = table.find(std::make_pair(m_tmp_triangles[i], 0));
			if (it != table.end()) {
				m_index.push_back(it->second);
			} else {
				std::cerr << "Lost vertex!!!" << std::endl;
			}
			
			//Second vertex of the triangle
			it = table.find(std::make_pair(m_tmp_triangles[i + 1], 0));
			if (it != table.end()) {
				m_index.push_back(it->second);
			} else {
				std::cerr << "Lost vertex!!!" << std::endl;
			}

			//Third vertex of the triangle
			it = table.find(std::make_pair(m_tmp_triangles[i + 2], 0));
			if (it != table.end()) {
				m_index.push_back(it->second);
			} else {
				std::cerr << "Lost vertex!!!" << std::endl;
			}
			m_num_triangles++;
		}

		//Compute normals

		m_tmp_triangles.clear();

		m_num_vertex = static_cast<unsigned int>(m_vertices.size());
	}

	int Mesh::get_num_vertex() {
		return m_num_vertex;
	}

	int Mesh::get_num_faces() {
		return m_num_triangles;
	}

	std::vector<glm::vec3> Mesh::get_vertices() {
		return m_vertices;
	}
	
	std::vector<unsigned int> Mesh::get_indexes() {
		return m_index;
	}

	//Comparator that must be equivalent to say less than
	// i.e, is lhs < rhs
	bool comparator_function(glm::vec3 lhs, glm::vec3 rhs) {
		const float epsilon = 0.001f;
		if (fabs(lhs.x - rhs.x) >= epsilon) {
			return lhs.x < rhs.x;
		} else if (fabs(lhs.y - rhs.y) >= epsilon) {
			return lhs.y < rhs.y;
		} else if (fabs(lhs.z - rhs.z) >= epsilon) {
			return lhs.z < rhs.z;
		} else {
			return false;
		}
	}

	bool comparator_function_wrapper(std::pair<glm::vec3, unsigned int> lhs, std::pair<glm::vec3, unsigned int> rhs) {
		return comparator_function(lhs.first, rhs.first);
	}

	void Mesh::write_to_obj(std::wstring file_name, bool quads = false) const {
		std::ofstream output_file;
		//Open for output erases previous file if necessary
		output_file.open(file_name.c_str(), std::ios::out | std::ios::trunc);

		if(output_file.is_open()) {
			output_file <<"o "<< "Test name" <<"\n";
			//write all vertex
			output_file <<"# List of vertices..." <<"\n";
			for (auto v : m_vertices) {
				output_file << "v " << v.x << " " << v.y << " " << v.z << "\n";
			}

			output_file <<"# List of faces..." <<"\n";
			if (quads) {
				//Pop two triangles from face
				for (int i = 0; i < m_index.size(); i += 6) {
					//Remember obj index start at 1, not zero
					unsigned int v1 = m_index[i] + 1;
					unsigned int v2 = m_index[i + 1] + 1;
					unsigned int v3 = m_index[i + 2] + 1;
					unsigned int v4 = m_index[i + 5] + 1;
					output_file << "f " << v1 << " " << v2 << " " << v3 << " " << v4 << "\n";
				}
			} else {
				//Pop triangles
				for (int i = 0; i < m_index.size(); i += 3) {
					//Remember obj index start at 1, not zero
					unsigned int v1 = m_index[i] + 1;
					unsigned int v2 = m_index[i + 1] + 1;
					unsigned int v3 = m_index[i + 2] + 1;
					output_file << "f " << v1 << " " << v2 << " " << v3 << "\n";
				}
			}
			output_file << "\n";

		}


		output_file.close();
		return;
	}

	void Mesh::update_bb(const glm::vec3& p) {
		if (p.x < m_lower_corner.x) {
			m_lower_corner.x = p.x;
		}
		if (p.y < m_lower_corner.y) {
			m_lower_corner.y = p.y;
		}
		if (p.z < m_lower_corner.z) {
			m_lower_corner.z = p.z;
		}

		if (p.x > m_upper_corner.x) {
			m_upper_corner.x = p.x;
		}
		if (p.y > m_upper_corner.y) {
			m_upper_corner.y = p.y;
		}
		if (p.z > m_upper_corner.z) {
			m_upper_corner.z = p.z;
		}
	}

	void Mesh::update_bb() {
		m_lower_corner = 1e10f * glm::vec3(1.0f);
		m_upper_corner = -1e10f * glm::vec3(1.0f);

		for (auto v : m_vertices) {
			update_bb(v);
		}

		m_dimensions = m_upper_corner - m_lower_corner;
	}

	void Mesh::rescale_and_center(const float& factor) {
		glm::mat4 I(1.0f);
		glm::mat4 T;

		//Translate to the center of the mesh, scale and translate back
		T = glm::scale(I, factor * glm::vec3(1.0f));
		T = glm::translate(T, -0.5f * m_dimensions);

		for (auto& v : m_vertices) {
			glm::vec3 new_v = glm::vec3(T * glm::vec4(v, 1.0f));
			v = new_v;
		}

		update_bb();
	}
}