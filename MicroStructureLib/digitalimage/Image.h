#ifndef IMAGE_H_
#define IMAGE_H_
#include<string>
#include<vector>
#include<glm/glm.hpp>
#include "Cell.h"
#include<IL/il.h>

namespace digitalimage {

class Image {
private:
	unsigned short m_width;
	unsigned short m_height;
	unsigned char* m_image_data;
	unsigned short number_of_neighbors(int index_row, int index_col, glm::vec3 color);
	unsigned short special_cases(int index_row, int index_col, glm::vec3 color);
public:
	Image();
	Image(std::wstring file_name);
	Image(int width, int height);
	~Image();
	int get_width();
	int get_height();
	void print_debug();
	unsigned char* get_pointer_to_data();
	void set_color_at(int index_row, int index_col, const unsigned char color[3]);
	void set_color_at(int index_row, int index_col, glm::vec3 color);
	glm::vec3 get_color_at(int index_row, int index_col);
	void reset_to_color(const unsigned char color[3]);
	void reset_to_color(glm::vec3 color);
	void thresholding(float threshold);
	void find_intersection_points(std::vector<std::pair<Cell, Cell>>& edges);
	void get_path_cells(std::vector<Cell>& center_points);
	Cell walk_to(Cell start, glm::vec3 color_walk, glm::vec3 color_stop, glm::vec3 color_erase);
};

const unsigned char b_black[3] = {0, 0, 0};
const unsigned char b_white[3] = {255, 255, 255};
const glm::vec3 black = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);

}
#endif
