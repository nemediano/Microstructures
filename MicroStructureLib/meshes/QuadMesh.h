#ifndef QUAD_MESH_H_
#define QUAD_MESH_H_

#include <set>
#include <vector>

#include "../digitalimage/BoundaryElement.h"
#include "../digitalimage/BinaryVolume.h"

using namespace digitalimage;

namespace meshes {

class QuadMesh {
public:
	QuadMesh();
	QuadMesh(const std::vector<BoundaryElement> faces);
	QuadMesh(const std::set<BoundaryElement> faces);
	
	QuadMesh(const QuadMesh& copy);
	~QuadMesh();
	QuadMesh& operator=(const QuadMesh& rhs);
	
	void save_to_obj(const std::wstring& file_name, bool indexed = true);
	void add_face(const BoundaryElement& face);
	bool is_in_boundary(const BoundaryElement& face);
	BinaryVolume save_to_image(glm::ivec3 offset, bool real_coordinates = false);
	void update();

private:
	std::set<BoundaryElement> m_faces;
	BoundaryElement m_seed;

	unsigned int m_voxel_number;
	glm::ivec3 m_size;
	glm::ivec3 m_bounding_box[2];

	void update_measures();

};

}
#endif