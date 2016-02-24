#include <cassert>
#include <iostream>
#include "Image.h"
#include "Cell.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <vector>
#include <utility>
namespace digitalimage {

void Image::set_color_at(int index_row, int index_col, const unsigned char color[3]) {
	assert(index_row >= 0 && index_row < m_width);
	assert(index_col >= 0 && index_col < m_height);

	int real_index = 3 * (index_row + index_col * m_width);
	
	m_image_data[real_index] = color[0];
	m_image_data[real_index + 1] = color[1];
	m_image_data[real_index + 2] = color[2];
}

Image::Image() {
	m_width = 256;
	m_height = 256;
	m_image_data = new unsigned char[3 * m_width * m_height];
}

Image::Image(int width, int height) {
	assert(width > 0);
	assert(height > 0);
	m_width = width;
	m_height = height;
	m_image_data = new unsigned char[3 * m_width * m_height];
}

void Image::print_debug() {
	for (int i = 0; i < m_width; i++) {
		for (int j = 0; j < m_height; j++) {
			glm::vec3 color = get_color_at(i, j);
			std::cout << "(" << color.r << ", " << color.g << ", " << color.b << ")\t";
		}
		std::cout << std::endl;
	}
}

Image::Image(std::wstring file_name) {
	ILuint image_id = ilGenImage();
	std::wcout << L"Input image: " << file_name << std::endl;
	ilBindImage(image_id);

	if (ilLoadImage(file_name.c_str()))
	{
		//iluFlipImage();
		m_width = ilGetInteger(IL_IMAGE_WIDTH);
		m_height= ilGetInteger(IL_IMAGE_HEIGHT);
		printf("Width, Height: %d, %d\n", m_width, m_height);
		m_image_data = new unsigned char[m_width * m_width * 3];
		if (!m_image_data) {
			std::cerr << "So ti yepe!!!" << std::endl;
		}
		ilCopyPixels(0, 0, 0, m_width, m_height, 1, IL_RGB, IL_UNSIGNED_BYTE, m_image_data);
	}
	else
	{
		// failed!!!
		ILenum status = ilGetError();
		switch (status) {
		case IL_COULD_NOT_OPEN_FILE:
			std::cerr << "Cannot find or use the file" << std::endl;
			ilBindImage(0);
			ilDeleteImage(image_id);
			break;

		case IL_ILLEGAL_OPERATION:
			std::cerr << "Library has not been initializated" << std::endl;
			ilBindImage(0);
			ilDeleteImage(image_id);
			break;

		case IL_INVALID_EXTENSION:
			std::cerr << "The header and extension of the image don't match" << std::endl;
			ilBindImage(0);
			ilDeleteImage(image_id);
			break;

		case IL_INVALID_PARAM:
			std::cerr << "File name is not valid!!" << std::endl;
			ilBindImage(0);
			ilDeleteImage(image_id);
			break;

		case IL_OUT_OF_MEMORY:
			std::cerr << "Could not allocate memmory for image" << std::endl;
			ilBindImage(0);
			ilDeleteImage(image_id);
			break;

		default:
			break;
		}
	}
	

}

Image::~Image() {
	if (m_image_data != nullptr) {
		delete[] m_image_data;
	}
}

unsigned char* Image::get_pointer_to_data() {
	return m_image_data;
}

int Image::get_width() {
	return static_cast<int>(m_width);
}

int Image::get_height() {
	return static_cast<int>(m_height);
}

void Image::reset_to_color(const unsigned char color[3]) {
	for (int i = 0; i < m_width * m_height; ++i) {
		m_image_data[3 * i] = color[0];
		m_image_data[3 * i + 1] = color[1];
		m_image_data[3 * i + 2] = color[2];
	}
}

void Image::reset_to_color(glm::vec3 color) {
	for (int i = 0; i < m_width * m_height; ++i) {
		m_image_data[3 * i] = static_cast<unsigned char>(255 * color.r);
		m_image_data[3 * i + 1] = static_cast<unsigned char>(255 * color.g);
		m_image_data[3 * i + 2] = static_cast<unsigned char>(255 * color.b);
	}
}

void Image::set_color_at (int index_row, int index_col, glm::vec3 color) {
	assert(index_row >= 0 && index_row < m_width);
	assert(index_col >= 0 && index_col < m_height);

	int real_index = 3 * (index_row + index_col * m_width);
	
	m_image_data[real_index] = static_cast<unsigned char>(255 * color.r);
	m_image_data[real_index + 1] = static_cast<unsigned char>(255 * color.g);
	m_image_data[real_index + 2] = static_cast<unsigned char>(255 * color.b);
}

void Image::thresholding(float threshold) {
	assert(threshold >= 0.0f && threshold <= 1.0f);
	unsigned char threshold_value = static_cast<unsigned char>(255 * threshold);
	for (int i = 0; i < m_width * m_height; ++i) {
			m_image_data[3 * i] = m_image_data[3 * i] <= threshold_value ? 0 : 255;
			m_image_data[3 * i + 1] =  m_image_data[3 * i + 1] <= threshold_value ? 0 : 255;
			m_image_data[3 * i + 2] =  m_image_data[3 * i + 2] <= threshold_value ? 0 : 255;
	}
}

glm::vec3 Image::get_color_at(int index_row, int index_col) {
	assert(index_row >= 0 && index_row < m_width);
	assert(index_col >= 0 && index_col < m_height);

	int real_index = 3 * (index_row + index_col * m_width);
	
	glm::vec3 color;

	color.r = static_cast<float>(m_image_data[real_index]) / 255.0f;
	color.g = static_cast<float>(m_image_data[real_index + 1]) / 255.0f;
	color.b = static_cast<float>(m_image_data[real_index + 2]) / 255.0f;

	return color;
}

void Image::find_intersection_points(std::vector<std::pair<Cell, Cell>>& edges) {
	std::vector<Cell> intersection_points;

	glm::vec3 red = glm::vec3(1.0, 0.0, 0.0);
	glm::vec3 green = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 blue = glm::vec3(0.0, 0.0, 1.0);
	glm::vec3 yellow = glm::vec3(1.0, 1.0, 0.0);
	glm::vec3 cyan = glm::vec3(0.0, 1.0, 1.0);
	glm::vec3 magenta = glm::vec3(1.0, 0.0, 1.0);

	for (int i = 0; i < m_width; ++i) {
		for (int j = 0; j < m_height; ++j) {
			glm::vec3 color = get_color_at(i, j);

			if (color == white) {
				//Count number of neighbors
				int count;
				if (i == 0 || j == 0 || i == m_width - 1 || j == m_height - 1) {
					count = special_cases(i, j, white);
				} else {
					count = number_of_neighbors(i, j, white);
				}
				
				if (count == 1 || count == 3) {
					Cell pixel;
					pixel.set_world_delta(2.0f / m_width);
					pixel.set_grid_position(i, j);
					pixel.set_color(count == 1 ? yellow : red);
					set_color_at(i, j, yellow);
					intersection_points.push_back(pixel);
				}
			}
		}
	}


	for (auto pixel : intersection_points) {
		Cell end = walk_to(pixel, white, yellow, black);
		edges.push_back(std::make_pair(pixel, end));
	}

}


void Image::get_path_cells(std::vector<Cell>& center_points) {
	glm::vec3 blue = glm::vec3(0.0, 0.0, 1.0);
	for (int i = 0; i < m_width; ++i) {
		for (int j = 0; j < m_height; ++j) {
			glm::vec3 color = get_color_at(i, j);
			if (color == white) {
				Cell pixel;
				pixel.set_world_delta(2.0f / m_width);
				pixel.set_grid_position(i, j);
				pixel.set_color(blue);
				set_color_at(i, j, blue);
				center_points.push_back(pixel);
			}
		}
	}
}

//Look in the 8 neighborhood of this pixel. Dosesnt work for pixels in the edges of the image!!!
unsigned short Image::number_of_neighbors(int index_row, int index_col, glm::vec3 color) {
	assert(index_row >= 1 && index_row < m_width - 1);
	assert(index_col >= 1 && index_col < m_height - 1);
	
	unsigned short counter = 0;

	if (get_color_at(index_row - 1, index_col - 1) == color) {
		counter++;
	}
	if (get_color_at(index_row - 1, index_col) == color) {
		counter++;
	}
	if (get_color_at(index_row - 1, index_col + 1) == color) {
		counter++;
	}
	if (get_color_at(index_row, index_col - 1) == color) {
		counter++;
	}
	if (get_color_at(index_row, index_col + 1) == color) {
		counter++;
	}
	if (get_color_at(index_row + 1, index_col - 1) == color) {
		counter++;
	}
	if (get_color_at(index_row + 1, index_col) == color) {
		counter++;
	}
	if (get_color_at(index_row + 1, index_col + 1) == color) {
		counter++;
	}
	
	return counter;
}


unsigned short Image::special_cases(int index_row, int index_col, glm::vec3 color) {
	unsigned short counter = 0;
	assert(index_row == 0 || index_row == m_width - 1 || index_col == 0 || index_col == m_height - 1);

	if (index_row == 0 && index_col == 0) {
		if (get_color_at(index_row, index_col + 1) == color) counter++;
		if (get_color_at(index_row + 1, index_col) == color) counter++;
		if (get_color_at(index_row + 1, index_col + 1) == color) counter++;
	} else if (index_row == 0 && index_col == m_height - 1) {
		if (get_color_at(index_row, index_col - 1) == color) counter++;
		if (get_color_at(index_row + 1, index_col - 1) == color) counter++;
		if (get_color_at(index_row + 1, index_col) == color) counter++;
	} else if (index_row == m_width - 1 && index_col == 0) {
		if (get_color_at(index_row - 1, index_col) == color) counter++;
        if (get_color_at(index_row - 1, index_col + 1) == color) counter++;
	    if (get_color_at(index_row, index_col + 1) == color) counter++;
	} else if (index_row == m_width - 1 && index_col == m_height - 1) {
		if (get_color_at(index_row - 1, index_col - 1) == color) counter++;
        if (get_color_at(index_row - 1, index_col) == color) counter++;
        if (get_color_at(index_row, index_col - 1) == color) counter++;
	} else if (index_row == 0) {
		if (get_color_at(index_row, index_col - 1) == color) counter++;
	    if (get_color_at(index_row, index_col + 1) == color) counter++;
	    if (get_color_at(index_row + 1, index_col - 1) == color) counter++;	
	    if (get_color_at(index_row + 1, index_col) == color) counter++;
	    if (get_color_at(index_row + 1, index_col + 1) == color) counter++;
	} else if (index_col == 0) {
        if (get_color_at(index_row - 1, index_col) == color) counter++;
	    if (get_color_at(index_row + 1, index_col) == color) counter++;
	    if (get_color_at(index_row + 1, index_col + 1) == color) counter++;	
	    if (get_color_at(index_row, index_col + 1) == color) counter++;
	    if (get_color_at(index_row - 1, index_col + 1) == color) counter++;
	} else if (index_row == m_width - 1) {
		if (get_color_at(index_row, index_col - 1) == color) counter++;
	    if (get_color_at(index_row, index_col + 1) == color) counter++;
	    if (get_color_at(index_row - 1, index_col - 1) == color) counter++;	
	    if (get_color_at(index_row - 1, index_col) == color) counter++;
	    if (get_color_at(index_row - 1, index_col + 1) == color) counter++;
	} else {
		if (get_color_at(index_row - 1, index_col) == color) counter++;
	    if (get_color_at(index_row + 1, index_col) == color) counter++;
	    if (get_color_at(index_row + 1, index_col - 1) == color) counter++;	
	    if (get_color_at(index_row, index_col - 1) == color) counter++;
	    if (get_color_at(index_row - 1, index_col - 1) == color) counter++;
	}

	return counter;
}

Cell Image::walk_to(Cell start, glm::vec3 color_walk, glm::vec3 color_stop, glm::vec3 color_erase) {
	std::pair<int, int> current_location;
	current_location = std::make_pair(start.get_row(), start.get_col());
	set_color_at(current_location.first, current_location.second, color_erase);
	while (get_color_at(current_location.first, current_location.second) != color_stop) {
		//Search for a new place to move
		if ((current_location.first - 1 >= 0 && current_location.second - 1 >= 0) &&
			(get_color_at(current_location.first - 1, current_location.second - 1) == color_walk || 
			get_color_at(current_location.first - 1, current_location.second - 1) == color_stop)) {
			current_location.first--;
			current_location.second--;
		} else if (current_location.first - 1 >= 0 &&
			       (get_color_at(current_location.first - 1, current_location.second) == color_walk ||
			       get_color_at(current_location.first - 1, current_location.second) == color_stop)) {
			current_location.first--;
		} else if ((current_location.first - 1 >= 0 && current_location.second + 1 < m_height) &&
			       (get_color_at(current_location.first - 1, current_location.second + 1) == color_walk ||
			       get_color_at(current_location.first - 1, current_location.second + 1) == color_stop)) {
			current_location.first--;
			current_location.second++;
		} else if (current_location.second - 1 >= 0 &&
			       (get_color_at(current_location.first, current_location.second - 1) == color_walk ||
			       get_color_at(current_location.first, current_location.second - 1) == color_stop)) {
			current_location.second--;
		} else if (current_location.second + 1 < m_height &&
			       (get_color_at(current_location.first, current_location.second + 1) == color_walk ||
			       get_color_at(current_location.first, current_location.second + 1) == color_stop)) {
			current_location.second++;
		} else if ((current_location.first + 1 < m_width && current_location.second - 1 >= 0) &&
			       (get_color_at(current_location.first + 1, current_location.second - 1) == color_walk ||
			       get_color_at(current_location.first + 1, current_location.second - 1) == color_stop)) {
			current_location.first++;
			current_location.second--;
		} else if (current_location.first + 1 < m_width &&
			       (get_color_at(current_location.first + 1, current_location.second) == color_walk ||
			       get_color_at(current_location.first + 1, current_location.second) == color_stop)) {
			current_location.first++;
		} else if ((current_location.first + 1 < m_width && current_location.second + 1 < m_height) &&
			       (get_color_at(current_location.first + 1, current_location.second + 1) == color_walk ||
			       get_color_at(current_location.first + 1, current_location.second + 1) == color_stop)) {
			current_location.first++;
			current_location.second++;
		} else { //This is an isolated point
			set_color_at(current_location.first, current_location.second, color_stop);
		}
		
		if (get_color_at(current_location.first, current_location.second) == color_walk) {
		    set_color_at(current_location.first, current_location.second, color_erase);
		}
	}

	Cell end_of_the_path;
	end_of_the_path.set_world_delta(2.0f / m_width);
	end_of_the_path.set_grid_position(current_location.first, current_location.second);
	return end_of_the_path;
}


}
