#ifndef SAMPLER_H_
#define SAMPLER_H_
#include <vector>
#include <glm/glm.hpp>

namespace meshes {

	class Mesh {

	private:
		unsigned int m_num_vertex;
		unsigned int m_num_triangles;
		unsigned int m_num_edges;
		std::vector<glm::vec3> m_vertices;
		std::vector<glm::vec3> m_normals;
		std::vector<unsigned int> m_index;
		std::vector<glm::vec3> m_tmp_triangles;
		//std::vector<glm::vec3> m_tmp_quads;
		glm::vec3 m_lower_corner;
		glm::vec3 m_upper_corner;
		glm::vec3 m_dimensions;
		void update_bb(const glm::vec3& p);
		void update_bb();
	public:
		Mesh();
		Mesh(const Mesh& copy);
		void add_triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
		void add_quad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);
		Mesh& operator=(const Mesh& rhs);
		virtual ~Mesh();
		int get_num_vertex();
		int get_num_faces();
		void update();
		std::vector<glm::vec3> get_vertices();
		std::vector<unsigned int> get_indexes();
		void rescale_and_center(const float& factor);
		void write_to_obj(std::wstring file_name, bool quads) const;
	};

	bool comparator_function(glm::vec3 lhs, glm::vec3 rhs);
	bool comparator_function_wrapper(std::pair<glm::vec3, unsigned int> lhs, std::pair<glm::vec3, unsigned int> rhs);
}
#endif
