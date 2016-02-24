#include <iostream>
#include <fstream>

#include "QuadMesh.h"

namespace meshes {
	
	QuadMesh::QuadMesh() {
		
		m_bounding_box[0] = glm::ivec3(0, 0, 0);
		m_bounding_box[1] = glm::ivec3(0, 0, 0);

		m_seed = BoundaryElement(0, 0, 0, TOP);
		m_voxel_number = 0;

		m_size = glm::ivec3(0, 0, 0);

	}

	QuadMesh::QuadMesh(const QuadMesh& copy) {
		m_bounding_box[0] = copy.m_bounding_box[0];
		m_bounding_box[1] = copy.m_bounding_box[1];

		m_seed = copy.m_seed;

		m_faces = copy.m_faces;

		m_voxel_number = copy.m_voxel_number;
		m_size = copy.m_size;
	}

	QuadMesh::QuadMesh(const std::vector<BoundaryElement> faces) {
		assert(faces.size() != 0);
		
		m_seed = faces[0];

		m_faces.insert(faces.begin(), faces.end());

		update_measures();

	}

	QuadMesh::QuadMesh(const std::set<BoundaryElement> faces) {
		assert(faces.size() != 0);

		m_seed = *faces.begin();

		m_faces = faces;

		update_measures();
	}

	QuadMesh::~QuadMesh() {

	}

	QuadMesh& QuadMesh::operator=(const QuadMesh& rhs) {
		if (this == &rhs) {
			return *this;
		}

		m_bounding_box[0] = rhs.m_bounding_box[0];
		m_bounding_box[1] = rhs.m_bounding_box[1];

		m_seed = rhs.m_seed;

		m_faces = rhs.m_faces;

		m_voxel_number = rhs.m_voxel_number;
		m_size = rhs.m_size;
	}

	void QuadMesh::save_to_obj(const std::wstring& file_name, bool indexed) {
		std::ofstream output_file;
		//Open for output erases previous file if necessary
		output_file.open(file_name.c_str(), std::ios::out | std::ios::trunc);

		if(output_file.is_open()) {
			output_file <<"# Seed: (" << m_seed.get_x() << ", " << m_seed.get_y() << ", " 
				<< m_seed.get_z() << ") Orientation: " << m_seed.get_orientation() << "\n";
			
			if (indexed) { //Index vertices for faces

			} else { //Non indexed vertex (a lot of duplication)
				for (auto face : m_faces) {
					
					glm::ivec3 center = glm::ivec3(face.get_x(), face.get_y(), face.get_z());
					output_file << "v ";
					
					switch(face.get_orientation()) {
						case TOP:
							
							output_file << (center.x + 1) << " " << (center.y + 1) << " " << (center.z) << "\n";
							output_file << (center.x) << " " << (center.y + 1) << " " << (center.z) << "\n";
							output_file << (center.x) << " " << (center.y + 1) << " " << (center.z + 1) << "\n";
							output_file << (center.x + 1) << " " << (center.y + 1) << " " << (center.z + 1) << "\n";
							
						break;

						case BOTTOM:

							output_file << (center.x + 1) << " " << (center.y) << " " << (center.z + 1) << "\n";
							output_file << (center.x) << " " << (center.y) << " " << (center.z + 1) << "\n";
							output_file << (center.x) << " " << (center.y) << " " << (center.z) << "\n";
							output_file << (center.x + 1) << " " << (center.y) << " " << (center.z) << "\n";

						break;

						case LEFT:

							output_file << (center.x) << " " << (center.y + 1) << " " << (center.z + 1) << "\n";
							output_file << (center.x) << " " << (center.y + 1) << " " << (center.z) << "\n";
							output_file << (center.x) << " " << (center.y) << " " << (center.z) << "\n";
							output_file << (center.x) << " " << (center.y) << " " << (center.z + 1) << "\n";

						break;

						case RIGHT:

							output_file << (center.x + 1) << " " << (center.y + 1) << " " << (center.z) << "\n";
							output_file << (center.x + 1) << " " << (center.y + 1) << " " << (center.z + 1) << "\n";
							output_file << (center.x + 1) << " " << (center.y) << " " << (center.z + 1) << "\n";
							output_file << (center.x + 1) << " " << (center.y) << " " << (center.z) << "\n";

						break;

						case FRONT:

							output_file << (center.x + 1) << " " << (center.y + 1) << " " << (center.z + 1) << "\n";
							output_file << (center.x) << " " << (center.y + 1) << " " << (center.z + 1) << "\n";
							output_file << (center.x) << " " << (center.y) << " " << (center.z + 1) << "\n";
							output_file << (center.x + 1) << " " << (center.y) << " " << (center.z + 1) << "\n";

						break;

						case BACK:

							output_file << (center.x) << " " << (center.y + 1) << " " << (center.z) << "\n";
							output_file << (center.x + 1) << " " << (center.y + 1) << " " << (center.z) << "\n";
							output_file << (center.x + 1) << " " << (center.y) << " " << (center.z) << "\n";
							output_file << (center.x) << " " << (center.y) << " " << (center.z) << "\n";

						break;
					}

					output_file << "f " << (-1) << " " << (-2) << " " << (-3) << " " << (-4) << "\n";
				}
			}
			
		}

		output_file.close();
		return;
	}

	BinaryVolume QuadMesh::save_to_image(glm::ivec3 offset, bool real_coordinates) {
		BinaryVolume volume(m_size.x + 2 * offset.x, m_size.y + 2 * offset.y, m_size.z + 2 * offset.z);

		std::vector<BoundaryElement> table_left;
		std::set<BoundaryElement> table_right;

		//Count the number of voxels inside this boundary
		while (!table_left.empty()) {

			BoundaryElement current_left_face = table_left.back();
			int j = current_left_face.get_y();
			int k = current_left_face.get_z();
			auto right_face_it = table_right.begin();

			for (int i = current_left_face.get_x(); i <m_bounding_box[0].x; ++i) {

				BoundaryElement current_right_face(i, j, k, RIGHT);

				right_face_it = table_right.find(current_right_face);

				volume.set_at((i + offset.x) - m_bounding_box[1].x, (j + offset.y) - m_bounding_box[1].y, (k + offset.z) - m_bounding_box[1].z, true);

				if (right_face_it != table_right.end()) {
					break;
				} 

			}

			//Remove elements from lists by pairs
			table_left.pop_back();
			table_right.erase(right_face_it);
		}

		return volume;
	}

	bool QuadMesh::is_in_boundary(const BoundaryElement& face) {
		return (m_faces.find(face) != m_faces.end());
	}

	void QuadMesh::add_face(const BoundaryElement& face) {
		m_faces.insert(face);
	}


	void QuadMesh::update_measures() {
		
		m_bounding_box[0] = glm::ivec3(m_seed.get_x(), m_seed.get_y(), m_seed.get_z());
		m_bounding_box[1] = glm::ivec3(m_seed.get_x(), m_seed.get_y(), m_seed.get_z());

		std::vector<BoundaryElement> table_left;
		std::set<BoundaryElement> table_right;
		//Fill the two list of boundaries
		for (auto face : m_faces) {

			if (face.get_orientation() == LEFT) {
				table_left.push_back(face);
			} else if (face.get_orientation() == RIGHT) {
				table_right.insert(face);
			}

			if (face.get_x() >m_bounding_box[0].x) {
				m_bounding_box[0].x = face.get_x();
			}
			if (face.get_y() > m_bounding_box[0].y) {
				m_bounding_box[0].y = face.get_y();
			}
			if (face.get_z() > m_bounding_box[0].z) {
				m_bounding_box[0].z = face.get_z();
			}

			if (face.get_x() < m_bounding_box[1].x) {
				m_bounding_box[1].x = face.get_x();
			}
			if (face.get_y() < m_bounding_box[1].y) {
				m_bounding_box[1].y = face.get_y();
			}
			if (face.get_z() < m_bounding_box[0].z) {
				m_bounding_box[1].z = face.get_z();
			}
		}

		assert(table_left.size() == table_right.size());

		//Count the number of voxels inside this boundary
		while (!table_left.empty()) {

			BoundaryElement current_left_face = table_left.back();
			int j = current_left_face.get_y();
			int k = current_left_face.get_z();
			auto right_face_it = table_right.begin();

			for (int i = current_left_face.get_x(); i <m_bounding_box[0].x; ++i) {

				BoundaryElement current_right_face(i, j, k, RIGHT);

				right_face_it = table_right.find(current_right_face);

				m_voxel_number++;

				if (right_face_it != table_right.end()) {
					break;
				} 

			}

			//Remove elements from lists by pairs
			table_left.pop_back();
			table_right.erase(right_face_it);
		}

		m_size = m_bounding_box[0] - m_bounding_box[1];
		
	}
}