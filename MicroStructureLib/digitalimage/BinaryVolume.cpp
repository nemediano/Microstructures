#include "BinaryVolume.h"
#include <cassert>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <IL/il.h>
#include <IL/ilu.h>

namespace digitalimage {
	BinaryVolume::BinaryVolume() : m_data(nullptr), m_depth(0), m_width(0), m_height(0) {
		m_voxel_number = 0;
		m_voxel_histogram = 0;
	}

	void BinaryVolume::erode() {
	BinaryVolume copy(*this);
		for (unsigned int k = 0; k < m_depth; ++k) {
			for (unsigned int j = 0; j < m_height; ++j) {
				for (unsigned int i = 0; i < m_width; ++i) {
					//If this pixel is white
					if (copy.get_value_at(i, j, k)) {
						//Check his neighborhood
						if (copy.need_erosion(i, j, k)) {
							toggle_at(i, j, k);
						}
					}
				}
			}
		}
	}

	void BinaryVolume::dilate() {
		BinaryVolume copy(*this);
		for (unsigned int k = 0; k < m_depth; ++k) {
			for (unsigned int j = 0; j < m_height; ++j) {
				for (unsigned int i = 0; i < m_width; ++i) {
					//If this pixel is black
					if (!copy.get_value_at(i, j, k)) {
						//Check his neighborhood
						if (copy.need_dilate(i, j, k)) {
							toggle_at(i, j, k);
						}
					}
				}
			}
		}
	}

	bool BinaryVolume::need_erosion(int x, int y, int z) {
		//Look for a black pixel in neighborhood
		if (!get_value_at(x - 1, y - 1, z - 1)) {
			return true;
		}
		if (!get_value_at(x - 1, y - 1, z)) {
			return true;
		}
		if (!get_value_at(x - 1, y - 1, z + 1)) {
			return true;
		}


		if (!get_value_at(x - 1, y, z - 1)) {
			return true;
		}
		if (!get_value_at(x - 1, y, z)) {
			return true;
		}
		if (!get_value_at(x - 1, y, z + 1)) {
			return true;
		}


		if (!get_value_at(x - 1, y + 1, z - 1)) {
			return true;
		}
		if (!get_value_at(x - 1, y + 1, z)) {
			return true;
		}
		if (!get_value_at(x - 1, y + 1, z + 1)) {
			return true;
		}


		////////
		if (!get_value_at(x, y - 1, z - 1)) {
			return true;
		}
		if (!get_value_at(x, y - 1, z)) {
			return true;
		}
		if (!get_value_at(x, y - 1, z + 1)) {
			return true;
		}


		if (!get_value_at(x, y, z - 1)) {
			return true;
		}
		if (!get_value_at(x, y, z)) {
			return true;
		}
		if (!get_value_at(x, y, z + 1)) {
			return true;
		}


		if (!get_value_at(x, y + 1, z - 1)) {
			return true;
		}
		if (!get_value_at(x, y + 1, z)) {
			return true;
		}
		if (!get_value_at(x, y + 1, z + 1)) {
			return true;
		}

		///////////
		if (!get_value_at(x + 1, y - 1, z - 1)) {
			return true;
		}
		if (!get_value_at(x + 1, y - 1, z)) {
			return true;
		}
		if (!get_value_at(x + 1, y - 1, z + 1)) {
			return true;
		}


		if (!get_value_at(x + 1, y, z - 1)) {
			return true;
		}
		if (!get_value_at(x + 1, y, z)) {
			return true;
		}
		if (!get_value_at(x + 1, y, z + 1)) {
			return true;
		}


		if (!get_value_at(x + 1, y + 1, z - 1)) {
			return true;
		}
		if (!get_value_at(x + 1, y + 1, z)) {
			return true;
		}
		if (!get_value_at(x + 1, y + 1, z + 1)) {
			return true;
		}

		return false;
	}

	bool BinaryVolume::need_dilate(int x, int y, int z) {
		//Look for a white pixel in neighborhood
		if (get_value_at(x - 1, y - 1, z - 1)) {
			return true;
		}
		if (get_value_at(x - 1, y - 1, z)) {
			return true;
		}
		if (get_value_at(x - 1, y - 1, z + 1)) {
			return true;
		}


		if (get_value_at(x - 1, y, z - 1)) {
			return true;
		}
		if (get_value_at(x - 1, y, z)) {
			return true;
		}
		if (get_value_at(x - 1, y, z + 1)) {
			return true;
		}


		if (get_value_at(x - 1, y + 1, z - 1)) {
			return true;
		}
		if (get_value_at(x - 1, y + 1, z)) {
			return true;
		}
		if (get_value_at(x - 1, y + 1, z + 1)) {
			return true;
		}


		////////
		if (get_value_at(x, y - 1, z - 1)) {
			return true;
		}
		if (get_value_at(x, y - 1, z)) {
			return true;
		}
		if (get_value_at(x, y - 1, z + 1)) {
			return true;
		}


		if (get_value_at(x, y, z - 1)) {
			return true;
		}
		if (get_value_at(x, y, z)) {
			return true;
		}
		if (get_value_at(x, y, z + 1)) {
			return true;
		}


		if (get_value_at(x, y + 1, z - 1)) {
			return true;
		}
		if (get_value_at(x, y + 1, z)) {
			return true;
		}
		if (get_value_at(x, y + 1, z + 1)) {
			return true;
		}

		///////////
		if (get_value_at(x + 1, y - 1, z - 1)) {
			return true;
		}
		if (get_value_at(x + 1, y - 1, z)) {
			return true;
		}
		if (get_value_at(x + 1, y - 1, z + 1)) {
			return true;
		}


		if (get_value_at(x + 1, y, z - 1)) {
			return true;
		}
		if (get_value_at(x + 1, y, z)) {
			return true;
		}
		if (get_value_at(x + 1, y, z + 1)) {
			return true;
		}


		if (get_value_at(x + 1, y + 1, z - 1)) {
			return true;
		}
		if (get_value_at(x + 1, y + 1, z)) {
			return true;
		}
		if (get_value_at(x + 1, y + 1, z + 1)) {
			return true;
		}

		return false;
	}

	BinaryVolume::BinaryVolume(int cols, int rows, int slices) {
		assert(cols > 0);
		assert(rows > 0);
		assert(slices > 0);

		m_width = cols;
		m_height = rows;
		m_depth = slices;

		m_voxel_number = m_width * m_height * m_depth;

		m_data = new bool[m_voxel_number];
		set_false();
	}

	BinaryVolume::BinaryVolume(const BinaryVolume& copy) {
		//Handle static members
		m_depth = copy.m_depth;
		m_width = copy.m_width;
		m_height = copy.m_height;
		m_voxel_number = copy.m_voxel_number;
		m_voxel_histogram = copy.m_voxel_histogram;

		//Allocate memory
		m_data = new bool [m_voxel_number];

		//Copy dynamic members
		std::copy(copy.m_data, copy.m_data + m_voxel_number, m_data);
	}

	BinaryVolume::BinaryVolume(Volume& volume, float threshold) {
		m_depth = volume.get_depth();
		m_width = volume.get_width();
		m_height = volume.get_height();

		m_voxel_number = m_width * m_height * m_depth;

		m_data = new bool[m_voxel_number];
		set_false();

		for (unsigned int k = 0; k < m_depth; ++k) {
			for (unsigned int j = 0; j < m_height; ++j) {
				for (unsigned int i = 0; i < m_width; ++i) {
					glm::vec3 voxel_color = volume.get_color_at(i, j, k);
					float gray_value = (voxel_color.r + voxel_color.g + voxel_color.b) / 3.0f;

					int real_index = get_real_index(i, j, k);

					if (gray_value > threshold) {
						m_data[real_index] = true;
						m_voxel_histogram++;
					}
				}
			}
		}
	}

	BinaryVolume::BinaryVolume(const std::wstring& file_name, const int& width, const int& height, const int& depth) {
		using namespace std;

		assert(width > 0);
		assert(height > 0);
		assert(depth > 0);

		m_width = static_cast<unsigned int>(width);
		m_height = static_cast<unsigned int>(height);
		m_depth = static_cast<unsigned int>(depth);

		m_voxel_number = m_width * m_height * m_depth;

		m_data = new bool[m_voxel_number];
		set_false();

		ifstream input_file(file_name.c_str(), ios::in | ios::binary | ios::ate);
		char* memory_block = nullptr;

		if (input_file.is_open()) {
			auto size = input_file.tellg();
			memory_block = new char[size];
			input_file.seekg(0, ios::beg);
			input_file.read(memory_block, size);
		}
		input_file.close();

		for (unsigned int i = 0; i < m_voxel_number; ++i) {
			if (memory_block[i] != 0) {
				m_data[i] = true;
				m_voxel_histogram++;
			}
		}
		
		delete memory_block;
	}

	void BinaryVolume::set_at(int x, int y, int z, bool value) {
		unsigned int real_index = get_real_index(x, y, z);
		
		//Erase previous voxels form histogram
		if (m_data[real_index]) {
			remove_white();
		} else {
			remove_black();
		}

		m_data[real_index] = value;

		//Add new value to histogram
		if (m_data[real_index]) {
			add_white();
		} else {
			add_black();
		}

	}

	void BinaryVolume::set_false_at(int x, int y, int z) {
		unsigned int real_index = get_real_index(x, y, z);
		if (m_data[real_index]) {
			m_data[real_index] = false;
			remove_white();
		}
	}

	void BinaryVolume::set_true_at(int x, int y, int z) {
		unsigned int real_index = get_real_index(x, y, z);
		if (!m_data[real_index]) {
			m_data[real_index] = true;
			add_white();
		}
	}

	void BinaryVolume::toggle_at(int x, int y, int z) {
		unsigned int real_index = get_real_index(x, y, z);

		if (m_data[real_index]) {
			m_data[real_index] = false;
			remove_white();
		} else {
			m_data[real_index] = true;
			add_white();
		}
	}

	bool BinaryVolume::is_true_at(int x, int y, int z) {
		return get_value_at(x, y, z);
	}

	bool BinaryVolume::is_false_at(int x, int y, int z) {
		return !is_true_at(x, y, z);
	}

	bool BinaryVolume::get_value_at(int x, int y, int z) {
		if (x == -1 || y == -1 || z == -1 || x == m_width || y == m_height || z == m_depth) {
			return false;
		}
		unsigned int real_index = get_real_index(x, y, z);
		return m_data[real_index];
	}

	void BinaryVolume::reset_to(bool value) {
		for (unsigned int i = 0; i < m_voxel_number; i++) {
			m_data[i] = value;
		}

		if (value) {
			m_voxel_histogram = m_voxel_number;
		} else {
			m_voxel_histogram = 0;
		}
	}

	void BinaryVolume::set_true() {
		reset_to(true);
	}

	void BinaryVolume::set_false() {
		reset_to(false);
	}

	BinaryVolume::~BinaryVolume() {
		if (m_data) {
			delete[] m_data;
			m_data = nullptr;
		}
	}

	BinaryVolume& BinaryVolume::operator=(const BinaryVolume& rhs) {
		//Prevent self assignation
		if (this == &rhs) {
			return *this;
		}
		//Free dynamic memory
		delete [] m_data;
		//Handle static members
		m_depth = rhs.m_depth;
		m_width = rhs.m_width;
		m_height = rhs.m_height;
		m_voxel_number = rhs.m_voxel_number;
		m_voxel_histogram = rhs.m_voxel_histogram;
		
		//Allocate memory
		m_data = new bool [m_voxel_number];
		//Copy dynamic members
		std::copy(rhs.m_data, rhs.m_data + m_voxel_number, m_data);
		
		return *this;
	}

	unsigned int BinaryVolume::get_real_index(int index_col, int index_row, int index_slc) {

		assert(index_row >= 0 && index_row < static_cast<int>(m_height));
		assert(index_col >= 0 && index_col < static_cast<int>(m_width));
		assert(index_slc >= 0 && index_slc < static_cast<int>(m_depth));

		return index_col + m_width * (index_row + m_height * index_slc);
	}

	void BinaryVolume::add_white() {
		m_voxel_histogram++;
	}

	void BinaryVolume::remove_white() {
		m_voxel_histogram--;
	}

	void BinaryVolume::add_black() {
		m_voxel_histogram--;
	}

	void BinaryVolume::remove_black() {
		m_voxel_histogram++;
	}

	int BinaryVolume::get_width() {
		return static_cast<int>(m_width);
	}

	int BinaryVolume::get_height() {
		return static_cast<int>(m_height);
	}

	int BinaryVolume::get_depth() {
		return static_cast<int>(m_depth);
	}

	int BinaryVolume::get_white_number() {
		return static_cast<int>(m_voxel_histogram);
	}
	
	int BinaryVolume::get_black_number() {
		return static_cast<int>(m_voxel_number - m_voxel_histogram);
	}

	void BinaryVolume::save_to_file(std::wstring file_name) {
		if (m_data == nullptr) {
			return;
		}

		unsigned char *pixels = new unsigned char [m_width * m_height * 3];
		unsigned int real_index;

		for (int i = 1; i <= m_depth; ++i) {
			std::wstring image_name = file_name + std::to_wstring(i) + L".tif";
			
			real_index = get_real_index(0, 0, i-1);
			unsigned int k = 0;
			//Copy pixel data to array in the desired format
			for (unsigned int j = 0; j < m_width * m_height; ++j) {
				
				if (m_data[real_index + j]) {
					pixels[k++] = 255;
					pixels[k++] = 255;
					pixels[k++] = 255;
				} else {
					pixels[k++] = 0;
					pixels[k++] = 0;
					pixels[k++] = 0;
				}
				
			}
			
			save_image(image_name, pixels);
		}

		delete[] pixels;
	}

	bool BinaryVolume::save_image(std::wstring file_name, unsigned char* pixels) {
		ILuint image_id = ilGenImage();
		
		if (0 == image_id) {
			std::wcerr << L"Image: " << file_name << L" was not able to save!!!" << L"\n";
			return false;
		}

		ilBindImage(image_id);
		{
			ilTexImage(m_width, m_height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, pixels);
			ilSaveImage(file_name.c_str());
		}
		ilBindImage(0);
		ilDeleteImage(image_id);
		return true;
	}

	const BinaryVolume BinaryVolume::cropp(glm::ivec3 start, glm::ivec3 lenghts) {
		assert(start.x < m_width);
		assert(start.y < m_height);
		assert(start.z < m_depth);

		assert(lenghts.x > 0);
		assert(lenghts.y > 0);
		assert(lenghts.z > 0);

		BinaryVolume new_image(static_cast<int>(lenghts.x), static_cast<int>(lenghts.y), static_cast<int>(lenghts.z));

		for (unsigned int k = 0; k < new_image.m_depth && (k + start.z) < m_depth; ++k) {
			for (unsigned int j = 0; j < new_image.m_height && (j + start.y) < m_height; ++j) {
				for (unsigned int i = 0; i < new_image.m_width && (i + start.x) < m_width; ++i) {
					int index = new_image.get_real_index(i, j, k);
					//Copy the value of the object into new image
					new_image.m_data[index] = get_value_at(i + start.x, j + start.y, k + start.z);
					if (new_image.m_data[index]) {
						new_image.m_voxel_histogram++;
					}
				}
			}
		}

		return new_image;

	}

}