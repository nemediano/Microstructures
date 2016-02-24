#ifndef VOLUME_H_
#define VOLUME_H_

#include<ostream>
#include<string>
#include<vector>
#include<glm/glm.hpp>
#include<IL/il.h>

namespace digitalimage 
{
	enum plane {XY_PLANE, XZ_PLANE, YZ_PLANE};

	class Volume {
		private:
			unsigned short m_width;
			unsigned short m_height;
			unsigned short m_depth;
			unsigned char* m_volume_data;
			unsigned int m_voxel_number;
			// Save memory for three image planes
			unsigned char* m_plane_xy;
			unsigned char* m_plane_xz;
			unsigned char* m_plane_yz;

			unsigned short number_of_neighbors(int index_row, int index_col, int index_slc, glm::vec3 color);
			unsigned short special_cases(int index_row, int index_col, int index_slc, glm::vec3 color);
			std::wstring generate_name(std::wstring base_file_name, unsigned int counter);
			int get_real_index(int index_col, int index_row, int index_slc);
		public:
			Volume();
			Volume(std::wstring base_file_name, unsigned int staks, bool natural = true);
			Volume(int width, int height, int depth);
			
			// Saving to stupid Bedrich SW format
			void save_to_raw(std::wstring file_name);
			// Saving to the standard binary UNC meta image data
			void save_to_medfile(std::wstring file_name);
			void print_debug();
			~Volume();
			Volume(const Volume& copy);
			Volume& operator=(const Volume& rhs);
			int get_width();
			int get_height();
			int get_depth();
			int get_voxel_number();
			unsigned char* get_pointer_to_data();
			unsigned char* get_pointer_to_data_rgba();
			unsigned char* get_data_plane(int index, plane orientation);
			void set_color_at(int index_row, int index_col, int index_slc, const unsigned char color[3]);
			void set_color_at(int index_row, int index_col, int index_slc, glm::vec3 color);
			glm::vec3 get_color_at(int index_row, int index_col, int index_slc);
			void reset_to_color(const unsigned char color[3]);
			void reset_to_color(glm::vec3 color);
			void thresholding(float threshold);
			friend std::ostream& operator<< (std::ostream &os, const Volume &vol);
	};
}
#endif
