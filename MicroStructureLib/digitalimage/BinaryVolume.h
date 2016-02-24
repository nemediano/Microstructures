#ifndef BINARY_VOLUME_H_
#define BINARY_VOLUME_H_

#include "Volume.h"

namespace digitalimage {
	class BinaryVolume {
	
	public:
		BinaryVolume();
		BinaryVolume(const std::wstring& file_name, const int& width, const int& height, const int& depth);
		BinaryVolume(int cols, int rows, int slices);
		BinaryVolume(const BinaryVolume& copy);
		BinaryVolume(Volume& volume, float threshold);
		int get_width();
		int get_height();
		int get_depth();
		void set_at(int x, int y, int z, bool value);
		void set_false_at(int x, int y, int z);
		void set_true_at(int x, int y, int z);
		void toggle_at(int x, int y, int z);
		bool is_true_at(int x, int y, int z);
		bool is_false_at(int x, int y, int z);
		bool get_value_at(int x, int y, int z);
		void reset_to(bool value);
		void set_true();
		void set_false();
		int get_white_number();
		int get_black_number();
		void erode();
		void dilate();
		BinaryVolume& operator=(const BinaryVolume& rhs);
		~BinaryVolume();
		void save_to_file(std::wstring file_name);
		const BinaryVolume cropp(glm::ivec3 start, glm::ivec3 lenghts);

	private:
		bool* m_data;
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_depth;
		unsigned long int m_voxel_number;
		// m_voxel_histogram = number of white (true) voxels
		unsigned long int m_voxel_histogram;

		//Auxiliary functions
		unsigned int get_real_index(int x, int y, int z);
		void add_white();
		void remove_white();
		void add_black();
		void remove_black();

		//Auxiliary function for dilate erode
		bool need_dilate(int x, int y, int z);
		bool need_erosion(int x, int y, int z);

		bool save_image(std::wstring file_name, unsigned char* pixels);
	};
}
#endif