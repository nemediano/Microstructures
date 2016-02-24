#include <cassert>
#include <iostream>
#include <fstream>
#include <queue>
#include <set>
#include "ArtzyAlgorithm.h"

namespace surface {
	
	ArtzyAlgorithm::ArtzyAlgorithm(Volume& volume, float threshold) {
		m_spacial_ocupation = BinaryVolume(volume, threshold);
	}

	ArtzyAlgorithm::ArtzyAlgorithm(BinaryVolume& volume) : m_spacial_ocupation(volume) {
		
	}

	BinaryVolume ArtzyAlgorithm::get_segmented_volume(bool real_coordinates) {
		
		//Extract a new volume using Artzy (using the next surface available)
		if (!more_surfaces()) {
			std::cerr << "No more boundaries in this data set!!" << std::endl;
			return BinaryVolume(0, 0, 0);
		}
		BoundaryElement seed = find_seed();
		track_boundary(seed);
		remove_from_volume();

		std::vector<BoundaryElement> table_left;
		std::set<BoundaryElement> table_right;
		//Fill the two list of boundaries
		for (auto face : m_boundary) {
			if (face.get_orientation() == LEFT) {
				table_left.push_back(face);
			} else if (face.get_orientation() == RIGHT) {
				table_right.insert(face);
			}
		}

		glm::ivec3 size;
		if (real_coordinates) { 
			size.x = m_spacial_ocupation.get_width();
			size.y = m_spacial_ocupation.get_height();
			size.z = m_spacial_ocupation.get_depth();
		} else { //Calculate the bounding box of this mesh
			glm::ivec3 min_coord = glm::ivec3(m_boundary.begin()->get_x(), m_boundary.begin()->get_y(), m_boundary.begin()->get_z());
			glm::ivec3 max_coord = glm::ivec3(m_boundary.begin()->get_x(), m_boundary.begin()->get_y(), m_boundary.begin()->get_z());

			for (auto face : m_boundary) {
				if (face.get_x() > max_coord.x) {
					max_coord.x = face.get_x();
				}
				if (face.get_y() > max_coord.y) {
					max_coord.y = face.get_y();
				}
				if (face.get_z() > max_coord.z) {
					max_coord.z = face.get_z();
				}

				if (face.get_x() < min_coord.x) {
					min_coord.x = face.get_x();
				}
				if (face.get_y() < min_coord.y) {
					min_coord.y = face.get_y();
				}
				if (face.get_z() < min_coord.z) {
					min_coord.z = face.get_z();
				}
			}
			size = max_coord - min_coord;
		}

		BinaryVolume volume(size.x, size.y, size.z);

		assert(table_right.size() == table_left.size());

		while (!table_left.empty()) {

			BoundaryElement current_left_face = table_left.back();
			int j = current_left_face.get_y();
			int k = current_left_face.get_z();
			auto right_face_it = table_right.begin();

			for (int i = current_left_face.get_x(); i < m_spacial_ocupation.get_width(); ++i) {

				BoundaryElement current_right_face(i, j, k, RIGHT);
				
				if (real_coordinates) {
					volume.set_true_at(i, j, k);
				} else {
					volume.set_true_at(i - size.x, j - size.y, k - size.z);
				}

				right_face_it = table_right.find(current_right_face);

				if (right_face_it != table_right.end()) {
					break;
				}

			}

			//Remove elements from lists by pairs
			table_left.pop_back();
			table_right.erase(right_face_it);
		}

		m_boundary.clear();

		return volume;
	}

	bool ArtzyAlgorithm::more_surfaces() {
		return (m_spacial_ocupation.get_white_number() != 0);
	}

	BoundaryElement ArtzyAlgorithm::find_seed() {
		int s_x, s_y, s_z;

		assert(m_spacial_ocupation.get_white_number() > 0);

		for (int k = 0; k < m_spacial_ocupation.get_depth(); ++k) {
			for (int j = 0; j < m_spacial_ocupation.get_height(); ++j) {
				for (int i = 0; i < m_spacial_ocupation.get_width(); ++i) {
					if (m_spacial_ocupation.is_true_at(i, j, k)) {
						s_x = i;
						s_y = j;
						s_z = k;
						BoundaryElement seed(s_x, s_y, s_z, LEFT);
						return seed;
					}
				}
			}
		}
		//Sentinel value not actual seed!!!
		return BoundaryElement(0, 0, 0, TOP);
	}

	Mesh ArtzyAlgorithm::extract_surface(bool verbose, bool indexing) {

		Mesh mesh;
		if (!more_surfaces()) {
			std::cerr << "No more boundaries in this data set!!" << std::endl;
			return mesh;
		}

		if (verbose) {
			wcout << L"Looking for seed" << L"\n";
		}
		BoundaryElement seed = find_seed();
		if (verbose) {
			wcout << L"Seed: (" << seed.get_x() << ", " << seed.get_y() << L", " << seed.get_z() << L") orientation: " << seed.get_orientation() << L"\n";
		}

		mesh = extract_surface(seed, verbose, indexing);


		return mesh;
	}

	void ArtzyAlgorithm::remove_from_volume() {
		if (m_boundary.empty()) {
			cerr << "Nothing to remove form this volume" << endl;
			return;
		}

		std::vector<BoundaryElement> table_left;
		std::set<BoundaryElement> table_right;
		//Fill the two sets
		for (auto face : m_boundary) {
			if (face.get_orientation() == LEFT) {
				table_left.push_back(face);
			} else if (face.get_orientation() == RIGHT) {
				table_right.insert(face);
			}
		}

		assert(table_right.size() == table_left.size());

		while (!table_right.empty()) {

			BoundaryElement current_left_face = table_left.back();
			int j = current_left_face.get_y();
			int k = current_left_face.get_z();
			auto right_face_it = table_right.begin();

			for (int i = current_left_face.get_x(); i < m_spacial_ocupation.get_width(); ++i) {
				
				BoundaryElement current_right_face(i, j, k, RIGHT);

				if (m_spacial_ocupation.is_true_at(i, j, k)) {
					m_spacial_ocupation.toggle_at(i, j, k);
				}

				right_face_it = table_right.find(current_right_face);

				if (right_face_it != table_right.end()) {
					break;
				}
		
			}

			//Remove elements from lists by pairs
			table_left.pop_back();
			table_right.erase(right_face_it);
		}

	}

	Mesh ArtzyAlgorithm::extract_surface(BoundaryElement seed, bool verbose, bool indexing) {
		ofstream output_file;
		output_file.open("statistics.txt", ios::out | ios::app);


		//This is the main part
		m_seeds.push_back(seed);
		
		Mesh mesh;
		unsigned int counter;
		BoundaryElement face = m_seeds.back();
		if (verify_seed(face)) {
			if (verbose) {
				wcout << L"Artzy's algorithm started" << L"\n";
			}
			track_boundary(face);
			if (verbose) {
				wcout << L"Artzy's algorithm finished!!" << L"\n";
				wcout << m_boundary.size() <<L" faces found." << L"\n";
				output_file << m_boundary.size() << ", ";
				wcout << L"Removing faces from binary volume..." << L"\n";
				counter = m_spacial_ocupation.get_white_number();
			}
			remove_from_volume();
			if (verbose) {
				wcout << (counter - m_spacial_ocupation.get_white_number()) << L" voxels removed" << L"\n";
				output_file << (counter - m_spacial_ocupation.get_white_number()) << std::endl;
			}
		} else {
			cerr << "Invalid seed" << endl;
		}
		output_file.close();
		
		while(!m_boundary.empty()) {
			BoundaryElement quad = m_boundary.back();
			
			glm::vec3 v1;
			glm::vec3 v2;
			glm::vec3 v3;
			glm::vec3 v4;

			switch(quad.get_orientation()) {
				case TOP:
				
					v1.x = static_cast<float>(quad.get_x() + 1);
					v1.y = static_cast<float>(quad.get_y() + 1);
					v1.z = static_cast<float>(quad.get_z());

					v2.x = static_cast<float>(quad.get_x());
					v2.y = static_cast<float>(quad.get_y() + 1);
					v2.z = static_cast<float>(quad.get_z());

					v3.x = static_cast<float>(quad.get_x());
					v3.y = static_cast<float>(quad.get_y() + 1);
					v3.z = static_cast<float>(quad.get_z() + 1);

					v4.x = static_cast<float>(quad.get_x() + 1);
					v4.y = static_cast<float>(quad.get_y() + 1);
					v4.z = static_cast<float>(quad.get_z() + 1);

				break;

				case BOTTOM:
					v1.x = static_cast<float>(quad.get_x() + 1);
					v1.y = static_cast<float>(quad.get_y());
					v1.z = static_cast<float>(quad.get_z() + 1);

					v2.x = static_cast<float>(quad.get_x());
					v2.y = static_cast<float>(quad.get_y());
					v2.z = static_cast<float>(quad.get_z() + 1);

					v3.x = static_cast<float>(quad.get_x());
					v3.y = static_cast<float>(quad.get_y());
					v3.z = static_cast<float>(quad.get_z());

					v4.x = static_cast<float>(quad.get_x() + 1);
					v4.y = static_cast<float>(quad.get_y());
					v4.z = static_cast<float>(quad.get_z());
				break;

				case LEFT:
					v1.x = static_cast<float>(quad.get_x());
					v1.y = static_cast<float>(quad.get_y() + 1);
					v1.z = static_cast<float>(quad.get_z() + 1);
					
					v2.x = static_cast<float>(quad.get_x());
					v2.y = static_cast<float>(quad.get_y() + 1);
					v2.z = static_cast<float>(quad.get_z());

					v3.x = static_cast<float>(quad.get_x());
					v3.y = static_cast<float>(quad.get_y());
					v3.z = static_cast<float>(quad.get_z());

					v4.x = static_cast<float>(quad.get_x());
					v4.y = static_cast<float>(quad.get_y());
					v4.z = static_cast<float>(quad.get_z() + 1);
				break;

				case RIGHT:
					v1.x = static_cast<float>(quad.get_x() + 1);
					v1.y = static_cast<float>(quad.get_y() + 1);
					v1.z = static_cast<float>(quad.get_z());

					v2.x = static_cast<float>(quad.get_x() + 1);
					v2.y = static_cast<float>(quad.get_y() + 1);
					v2.z = static_cast<float>(quad.get_z() + 1);

					v3.x = static_cast<float>(quad.get_x() + 1);
					v3.y = static_cast<float>(quad.get_y());
					v3.z = static_cast<float>(quad.get_z() + 1);

					v4.x = static_cast<float>(quad.get_x() + 1);
					v4.y = static_cast<float>(quad.get_y());
					v4.z = static_cast<float>(quad.get_z());
				break;

				case FRONT:
					v1.x = static_cast<float>(quad.get_x() + 1);
					v1.y = static_cast<float>(quad.get_y() + 1);
					v1.z = static_cast<float>(quad.get_z() + 1);

					v2.x = static_cast<float>(quad.get_x());
					v2.y = static_cast<float>(quad.get_y() + 1);
					v2.z = static_cast<float>(quad.get_z() + 1);

					v3.x = static_cast<float>(quad.get_x());
					v3.y = static_cast<float>(quad.get_y());
					v3.z = static_cast<float>(quad.get_z() + 1);

					v4.x = static_cast<float>(quad.get_x() + 1);
					v4.y = static_cast<float>(quad.get_y());
					v4.z = static_cast<float>(quad.get_z() + 1);
				break;

				case BACK:
					v1.x = static_cast<float>(quad.get_x());
					v1.y = static_cast<float>(quad.get_y() + 1);
					v1.z = static_cast<float>(quad.get_z());

					v2.x = static_cast<float>(quad.get_x() + 1);
					v2.y = static_cast<float>(quad.get_y() + 1);
					v2.z = static_cast<float>(quad.get_z());

					v3.x = static_cast<float>(quad.get_x() + 1);
					v3.y = static_cast<float>(quad.get_y());
					v3.z = static_cast<float>(quad.get_z());

					v4.x = static_cast<float>(quad.get_x());
					v4.y = static_cast<float>(quad.get_y());
					v4.z = static_cast<float>(quad.get_z());
				break;
			}

			mesh.add_quad(v1, v2, v3, v4);
			
			m_boundary.pop_back();
		}
		m_seeds.pop_back();

		if (indexing) {
			if (verbose) {
				wcout << L"Mesh indexing started..." << L"\n";

			}
			mesh.update();
			if (verbose) {
				wcout << L"Mesh indexing finished..." << L"\n";
			} 
		}

		return mesh;
	}

	vector<Mesh> ArtzyAlgorithm::get_surfaces(bool verbose, bool indexing) {
		//We don't need the stack of seed in the new implementation
		//We just calculate new seed, until there is no more possible seeds
		//i.e. there is no more white voxels
		m_components.clear();
		while(m_spacial_ocupation.get_white_number() != 0) {
			Mesh boundary = extract_surface(verbose, indexing);
			m_components.push_back(boundary);
		}

		return m_components;
	}

	bool ArtzyAlgorithm::verify_seed(BoundaryElement seed) {
		if (m_spacial_ocupation.is_false_at(seed.get_x(), seed.get_y(), seed.get_z())) {
			return false;
		}

		bool test;
		switch(seed.get_orientation()) {
			case TOP:
				test = m_spacial_ocupation.is_false_at(seed.get_x(), seed.get_y() + 1, seed.get_z());
			break;

			case BOTTOM:
				test = m_spacial_ocupation.is_false_at(seed.get_x(), seed.get_y() - 1, seed.get_z());
			break;

			case LEFT:
				test = m_spacial_ocupation.is_false_at(seed.get_x() - 1, seed.get_y(), seed.get_z());
			break;

			case RIGHT:
				test = m_spacial_ocupation.is_false_at(seed.get_x() + 1, seed.get_y(), seed.get_z());
			break;

			case FRONT:
				test = m_spacial_ocupation.is_false_at(seed.get_x() - 1, seed.get_y(), seed.get_z() + 1);
			break;

			case BACK:
				test = m_spacial_ocupation.is_false_at(seed.get_x() - 1, seed.get_y(), seed.get_z() - 1);
				break;
		}

		return test;
	}


	void ArtzyAlgorithm::track_boundary(BoundaryElement seed) {
		//This is the main part of the artzy algorithm
		m_boundary.clear(); //Equivalent to A_z in mi thesis notation
		queue<BoundaryElement> fly_queue; //Equivalent to Q in my thesis notation
		multiset<BoundaryElement> look_up_table; //Equivalent to L in my thesis notation
		BoundaryElement fly; //Equivalent to beta
		BoundaryElement fly_1; //Equivalent to beta 1
		BoundaryElement fly_2; //Equivalent to beta 2

		m_boundary.push_back(seed);
		fly_queue.push(seed);
		look_up_table.insert(seed);
		look_up_table.insert(seed);
		
		while (!fly_queue.empty()) {
			fly = fly_queue.front();
			fly_queue.pop();

			fly_1 = next_face(fly, true);
			fly_2 = next_face(fly, false);
			
			auto it = look_up_table.find(fly_1);
			if (it != look_up_table.end()) {
				//It's safe to do this in this version only removes one element
				look_up_table.erase(it);
			} else {
				m_boundary.push_back(fly_1);
				look_up_table.insert(fly_1);
				fly_queue.push(fly_1);
			}

			it = look_up_table.find(fly_2);
			if (it != look_up_table.end()) {
				look_up_table.erase(it);
			} else {
				m_boundary.push_back(fly_2);
				look_up_table.insert(fly_2);
				fly_queue.push(fly_2);
			}
		}


	}

	BoundaryElement ArtzyAlgorithm::next_face(BoundaryElement& fly, bool first) {

		glm::ivec3 right_top;
		glm::ivec3 right;

		//Change our orientation to cannonical Figure 1.11 using Figure 1.10
		switch(fly.get_orientation()) {
			
			case TOP:
				if (first) { //Green
					
					right_top.x = fly.get_x();
					right_top.y = fly.get_y() + 1;
					right_top.z = fly.get_z() + 1;

					right.x = fly.get_x();
					right.y = fly.get_y();
					right.z = fly.get_z() + 1;

				} else { //Blue

					right_top.x = fly.get_x() - 1;
					right_top.y = fly.get_y() + 1;
					right_top.z = fly.get_z();

					right.x = fly.get_x() - 1;
					right.y = fly.get_y();
					right.z = fly.get_z();
				}
			break;
		
			case BOTTOM:
				if (first) { //Green
					
					right_top.x = fly.get_x();
					right_top.y = fly.get_y() - 1;
					right_top.z = fly.get_z() - 1;

					right.x = fly.get_x();
					right.y = fly.get_y();
					right.z = fly.get_z() - 1;

				} else { //Blue

					right_top.x = fly.get_x() + 1;
					right_top.y = fly.get_y() - 1;
					right_top.z = fly.get_z();

					right.x = fly.get_x() + 1;
					right.y = fly.get_y();
					right.z = fly.get_z();
				}
			break;

			case LEFT:
				if (first) { //Blue
					
					right_top.x = fly.get_x() - 1;
					right_top.y = fly.get_y() - 1;
					right_top.z = fly.get_z();

					right.x = fly.get_x();
					right.y = fly.get_y() - 1;
					right.z = fly.get_z();

				} else { //Red

					right_top.x = fly.get_x() - 1;
					right_top.y = fly.get_y();
					right_top.z = fly.get_z() - 1;

					right.x = fly.get_x();
					right.y = fly.get_y();
					right.z = fly.get_z() - 1;
				}
			break;

			case RIGHT:
				if (first) { //Blue
					
					right_top.x = fly.get_x() + 1;
					right_top.y = fly.get_y() + 1;
					right_top.z = fly.get_z();

					right.x = fly.get_x();
					right.y = fly.get_y() + 1;
					right.z = fly.get_z();

				} else { //Red

					right_top.x = fly.get_x() + 1;
					right_top.y = fly.get_y();
					right_top.z = fly.get_z() + 1;

					right.x = fly.get_x();
					right.y = fly.get_y();
					right.z = fly.get_z() + 1;
				}
			break;

			case FRONT:
				if (first) { //Green
					
					right_top.x = fly.get_x();
					right_top.y = fly.get_y() - 1;
					right_top.z = fly.get_z() + 1;

					right.x = fly.get_x();
					right.y = fly.get_y() - 1;
					right.z = fly.get_z();

				} else { //Red

					right_top.x = fly.get_x() - 1;
					right_top.y = fly.get_y();
					right_top.z = fly.get_z() + 1;

					right.x = fly.get_x() - 1;
					right.y = fly.get_y();
					right.z = fly.get_z();
				}
			break;

			case BACK:
				if (first) { //Green
					
					right_top.x = fly.get_x();
					right_top.y = fly.get_y() + 1;
					right_top.z = fly.get_z() - 1;

					right.x = fly.get_x();
					right.y = fly.get_y() + 1;
					right.z = fly.get_z();

				} else { //Red

					right_top.x = fly.get_x() + 1;
					right_top.y = fly.get_y();
					right_top.z = fly.get_z() - 1;

					right.x = fly.get_x() + 1;
					right.y = fly.get_y();
					right.z = fly.get_z();
				}
			break;
		}

		//Now we have to test according to figure 1.11
		
		CASE_NUMBER current_case;
		glm::ivec3 next_voxel;

		if (m_spacial_ocupation.get_value_at(right_top.x, right_top.y, right_top.z)) {
			current_case = ONE;
			next_voxel = right_top;
		} else if (m_spacial_ocupation.get_value_at(right.x, right.y, right.z)) {
			current_case = TWO;
			next_voxel = right;
		} else {
			current_case = THREE;
			next_voxel = glm::ivec3(fly.get_x(), fly.get_y(), fly.get_z());
		}


		if (current_case == TWO) {
			return BoundaryElement(next_voxel.x, next_voxel.y, next_voxel.z, fly.get_orientation());
		}


		ORIENTATION next_orientation;

		switch(fly.get_orientation()) {
			
			case TOP:
				if (first) { //Green
					next_orientation = current_case == ONE ? BACK : FRONT;
				} else { //Blue
					next_orientation = current_case == ONE ? RIGHT : LEFT;
				}
			break;
		
			case BOTTOM:
				if (first) { //Green
					next_orientation = current_case == ONE ? FRONT : BACK;
				} else { //Blue
					next_orientation = current_case == ONE ? LEFT : RIGHT;
				}
			break;

			case LEFT:
				if (first) { //Blue
					next_orientation = current_case == ONE ? TOP : BOTTOM;
				} else { //Red
					next_orientation = current_case == ONE ? FRONT : BACK;
				}
			break;

			case RIGHT:
				if (first) { //Blue
					next_orientation = current_case == ONE ?  BOTTOM : TOP;
				} else { //Red
					next_orientation = current_case == ONE ?  BACK : FRONT;
				}
			break;

			case FRONT:
				if (first) { //Green
					next_orientation = current_case == ONE ? TOP : BOTTOM;
				} else { //Red
					next_orientation = current_case == ONE ? RIGHT : LEFT;
				}
			break;

			case BACK:
				if (first) { //Green
					next_orientation = current_case == ONE ? BOTTOM : TOP;
				} else { //Red
					next_orientation = current_case == ONE ? LEFT : RIGHT;
				}
			break;
		}

		BoundaryElement next_face(next_voxel.x, next_voxel.y, next_voxel.z, next_orientation);

		return next_face;
	}

}

