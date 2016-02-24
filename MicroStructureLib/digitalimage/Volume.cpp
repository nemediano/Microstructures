#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <IL/il.h>
#include <IL/ilu.h>

#include "Volume.h"

namespace digitalimage {

int Volume::get_voxel_number()
{
	return static_cast<int>(m_voxel_number);
}

unsigned char* Volume::get_data_plane(int index, plane orientation)	// get the data from a certain plane
{
	unsigned char* data;
	assert(index >= 0);
	if (orientation == XY_PLANE)
	{
		assert(index < m_depth);
		//std::wcout << "XY plane z=" << index << std::endl;
		for (int j = 0; j < m_height; ++j) 
		{
			for (int i = 0; i < m_width; ++i)
			{
				glm::vec3 color = get_color_at(i, j, index);
				int real_index_image = 3 * (i + j * m_width);
				int red = m_plane_xy[real_index_image] = static_cast<unsigned char>(255 * color.r);
				int green = m_plane_xy[real_index_image + 1] = static_cast<unsigned char>(255 * color.g);
				int blue = m_plane_xy[real_index_image + 2] = static_cast<unsigned char>(255 * color.b);
				//std::wcout << "x=" << i << " y=" << j << " L=(" << red << ", " << green << ", "<< blue << ")"<< std::endl;
			}
		}
		data = m_plane_xy;
	} 
	else if (orientation == XZ_PLANE) 
	{
		assert(index < m_height);
		//std::wcout << "XZ plane y=" << index << std::endl;
		for (int k = 0; k < m_depth; ++k)
		{
			for (int i = 0; i < m_width; ++i)
			{
				glm::vec3 color = get_color_at(i, index, k);
				int real_index_image = 3 * (i + k * m_width);
				int red = m_plane_xz[real_index_image] = static_cast<unsigned char>(255 * color.r);
				int green = m_plane_xz[real_index_image + 1] = static_cast<unsigned char>(255 * color.g);
				int blue = m_plane_xz[real_index_image + 2] = static_cast<unsigned char>(255 * color.b);
				//std::wcout << "x=" << i << " z=" << k << " L=(" << red << ", " << green << ", "<< blue << ")"<< std::endl;
			}
		}
		data = m_plane_xz;
	} 
	else if (orientation == YZ_PLANE) 
	{
		assert(index < m_width);
		//std::wcout << "YZ plane x=" << index << std::endl;
		//Here is an error
		for (int k = 0; k < m_depth; ++k) 
		{
			for (int j = 0; j < m_height; ++j)
			{
				glm::vec3 color = get_color_at(index, j, k);
				int real_index_image = 3 * (k + j * m_depth);
				int red = m_plane_yz[real_index_image] = static_cast<unsigned char>(255 * color.r);
				int green = m_plane_yz[real_index_image + 1] = static_cast<unsigned char>(255 * color.g);
				int blue = m_plane_yz[real_index_image + 2] = static_cast<unsigned char>(255 * color.b);
				//std::wcout << "y=" << j << " z=" << k << " L=(" << red << ", " << green << ", "<< blue << ")"<< std::endl;
			}
		}
		data = m_plane_yz;
	} 
	else {
		data = nullptr;
		std::cerr << "Invalid plane option!!" << std::endl;
	}

	return data;
}

void Volume::set_color_at(int index_row, int index_col, int index_slc, const unsigned char color[3]) 
{
	int real_index = get_real_index(index_col, index_row, index_slc);
	
	m_volume_data[real_index] = color[0];
	m_volume_data[real_index + 1] = color[1];
	m_volume_data[real_index + 2] = color[2];
}

void Volume::set_color_at (int index_col, int index_row, int index_slc, glm::vec3 color) 
{
	int real_index = get_real_index(index_col, index_row, index_slc);
	
	m_volume_data[real_index] = static_cast<unsigned char>(255 * color.r);
	m_volume_data[real_index + 1] = static_cast<unsigned char>(255 * color.g);
	m_volume_data[real_index + 2] = static_cast<unsigned char>(255 * color.b);
}

glm::vec3 Volume::get_color_at(int index_col, int index_row, int index_slc)
{
	int real_index = get_real_index(index_col, index_row, index_slc);
	glm::vec3 color;

	color.r = static_cast<float>(m_volume_data[real_index]) / 255.0f;
	color.g = static_cast<float>(m_volume_data[real_index + 1]) / 255.0f;
	color.b = static_cast<float>(m_volume_data[real_index + 2]) / 255.0f;

	return color;
}

int Volume::get_real_index(int index_col, int index_row, int index_slc) 
{
	assert(index_row >= 0 && index_row < m_height);
	assert(index_col >= 0 && index_col < m_width);
	assert(index_slc >= 0 && index_slc < m_depth);

	return 3 * (index_col + m_width * (index_row + m_height * index_slc));
}

Volume::Volume()		// the default size is shown as follows
{
	m_width = 256;
	m_height = 256;
	m_depth = 256;
	m_voxel_number = m_width * m_height * m_depth;

	m_volume_data = new unsigned char[3 * m_voxel_number];
	m_plane_xy = new unsigned char[3 * m_width * m_height];
	m_plane_xz = new unsigned char[3 * m_width * m_depth];
	m_plane_yz = new unsigned char[3 * m_height * m_depth];
}

Volume::Volume(int width, int height, int depth) 
{
	assert(width > 0);
	assert(height > 0);
	assert(depth > 0);

	m_width = width;
	m_height = height;
	m_depth = depth;
	m_voxel_number = m_width * m_height * m_depth;

	m_volume_data = new unsigned char[3 * m_voxel_number];
	m_plane_xy = new unsigned char[3 * m_width * m_height];
	m_plane_xz = new unsigned char[3 * m_width * m_depth];
	m_plane_yz = new unsigned char[3 * m_height * m_depth];
}

Volume::Volume(std::wstring base_file_name, unsigned int staks, bool natural) 
{
	assert(staks > 0);
	std::wcout << L"Input Volume: " << base_file_name << std::endl;
	std::wcout << L"Number of stacks: " << staks << std::endl;
	bool first_one = true;
	for (unsigned int count = 1; count <= staks; ++count) 
	{
		ILuint image_id = ilGenImage();
		ilBindImage(image_id);
		std::wstring file_name = generate_name(base_file_name, count);	// get the real filename
		std::wcout << L"File name: " << file_name << std::endl;
		if (ilLoadImage(file_name.c_str()))	
		{
			ILenum status = ilGetError();
			if (first_one) 
			{
				//iluFlipImage();
				m_width = ilGetInteger(IL_IMAGE_WIDTH);
				m_height= ilGetInteger(IL_IMAGE_HEIGHT);
				m_depth = ilGetInteger(IL_IMAGE_DEPTH);
				//printf("Width, Height, Depth: %d, %d, %d\n", m_width, m_height, m_depth);
				m_volume_data = new unsigned char[m_width * m_width * staks * 3];
				first_one = false;
			}
			int width = ilGetInteger(IL_IMAGE_WIDTH);
			int height= ilGetInteger(IL_IMAGE_HEIGHT);
			if (width == m_width && height == m_height) 
			{
				ilCopyPixels(0, 0, 0, m_width, m_height, m_depth, IL_RGB, IL_UNSIGNED_BYTE, &m_volume_data[m_width * m_height * (count - 1) * 3]);
			} 
			else 
			{
				std::wcerr << L"Image: " << file_name << L" has incorrect dimensions" << std::endl;
			}
		} 
		else 
		{
			// failed!!!
			ILenum status = ilGetError();
			switch (status) 
			{
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
		ilDeleteImage(image_id);
	}
	m_depth = staks;
	m_voxel_number = m_width * m_height * m_depth;

	m_plane_xy = new unsigned char[3 * m_width * m_height];
	m_plane_xz = new unsigned char[3 * m_width * m_depth];
	m_plane_yz = new unsigned char[3 * m_height * m_depth];
	//Just to see how data is stored
	/*std::wcout << "Data in array after read from files:" << std::endl;
	for (int i = 0; i < 3 * m_voxel_number; i += 3) {
		std::wcout << "Voxel " << i / 3 << ": L=(" << m_volume_data[i] << ", "
			<< m_volume_data[i + 1] << ", " << m_volume_data[i + 2] << ")" << std::endl;
	}*/
}

std::ostream& operator<< (std::ostream &os, const Volume &vol) 
{
	return os << "Volume dimensions: [" << vol.m_width << ", " << vol.m_height << ", " << vol.m_depth << "]" << std::endl << "Number of voxels: " << vol.m_voxel_number;
}


Volume::Volume(const Volume& copy) {
	//Allocate new memory
	m_volume_data = new unsigned char[copy.m_voxel_number * 3];
	m_plane_xy = new unsigned char[3 * copy.m_width * copy.m_height];
	m_plane_xz = new unsigned char[3 * copy.m_width * copy.m_depth];
	m_plane_yz = new unsigned char[3 * copy.m_height * copy.m_depth];
	//Copy new member value
	m_depth = copy.m_depth;
	m_width = copy.m_width;
	m_height = copy.m_height;
	m_voxel_number = copy.m_voxel_number;

	std::copy(copy.m_volume_data, copy.m_volume_data + (copy.m_voxel_number * 3), m_volume_data);
	std::copy(copy.m_plane_xy, copy.m_plane_xy + (3 * copy.m_width * copy.m_height), m_plane_xy);
	std::copy(copy.m_plane_xz, copy.m_plane_xz + (3 * copy.m_width * copy.m_depth), m_plane_xz);
	std::copy(copy.m_plane_yz, copy.m_plane_yz + (3 * copy.m_height * copy.m_depth), m_plane_yz);
}

Volume& Volume::operator=(const Volume& rhs) {
	//Prevent self reference
	if (this == &rhs) {
		return *this;
	}
	//Free memory
	delete[] m_volume_data;
	delete[] m_plane_xy;
	delete[] m_plane_xz;
	delete[] m_plane_yz;
	//Allocate new memory
	m_volume_data = new unsigned char[rhs.m_voxel_number * 3];
	m_plane_xy = new unsigned char[3 * rhs.m_width * rhs.m_height];
	m_plane_xz = new unsigned char[3 * rhs.m_width * rhs.m_depth];
	m_plane_yz = new unsigned char[3 * rhs.m_height * rhs.m_depth];
	//Copy new member value
	m_depth = rhs.m_depth;
	m_width = rhs.m_width;
	m_height = rhs.m_height;
	m_voxel_number = rhs.m_voxel_number;
	std::copy(rhs.m_volume_data, rhs.m_volume_data + (rhs.m_voxel_number * 3), m_volume_data);
	std::copy(rhs.m_plane_xy, rhs.m_plane_xy + (3 * rhs.m_width * rhs.m_height), m_plane_xy);
	std::copy(rhs.m_plane_xz, rhs.m_plane_xz + (3 * rhs.m_width * rhs.m_depth), m_plane_xz);
	std::copy(rhs.m_plane_yz, rhs.m_plane_yz + (3 * rhs.m_height * rhs.m_depth), m_plane_yz);
	//Return object reference
	return *this;
}

Volume::~Volume() {
	if (m_volume_data != nullptr) {
		delete[] m_volume_data;
	}

	if (m_plane_xy != nullptr) {
		delete[] m_plane_xy;
	}

	if (m_plane_xz != nullptr) {
		delete[] m_plane_xz;
	}

	if (m_plane_yz != nullptr) {
		delete[] m_plane_yz;
	}
	m_width = 0;
	m_height = 0;
	m_depth = 0;
}

unsigned char* Volume::get_pointer_to_data() {
	return m_volume_data;
}

int Volume::get_width() {
	return static_cast<int>(m_width);
}

int Volume::get_height() {
	return static_cast<int>(m_height);
}

int Volume::get_depth() {
	return static_cast<int>(m_depth);
}

void Volume::reset_to_color(const unsigned char color[3])
{
	for (int i = 0; i < m_width * m_height * m_depth; ++i)
	{
		m_volume_data[3 * i] = color[0];
		m_volume_data[3 * i + 1] = color[1];
		m_volume_data[3 * i + 2] = color[2];
	}
}

void Volume::reset_to_color(glm::vec3 color)
{
	for (int i = 0; i < m_width * m_height * m_depth; ++i) 
	{
		m_volume_data[3 * i] = static_cast<unsigned char>(255 * color.r);
		m_volume_data[3 * i + 1] = static_cast<unsigned char>(255 * color.g);
		m_volume_data[3 * i + 2] = static_cast<unsigned char>(255 * color.b);
	}
}

void Volume::thresholding(float threshold)
{
	assert(threshold >= 0.0f && threshold <= 1.0f);
	unsigned char threshold_value = static_cast<unsigned char>(255 * threshold);
	for (int i = 0; i < m_width * m_height * m_depth; ++i)
	{
			m_volume_data[3 * i] = m_volume_data[3 * i] <= threshold_value ? 0 : 255;
			m_volume_data[3 * i + 1] =  m_volume_data[3 * i + 1] <= threshold_value ? 0 : 255;
			m_volume_data[3 * i + 2] =  m_volume_data[3 * i + 2] <= threshold_value ? 0 : 255;
	}
}

//Look in the 8 neighborhood of this pixel. Dosesnt work for pixels in the edges of the image!!!
unsigned short Volume::number_of_neighbors(int index_row, int index_col, int index_slc, glm::vec3 color)
{
	assert(index_row >= 1 && index_row < m_width - 1);
	assert(index_col >= 1 && index_col < m_height - 1);
	assert(index_slc >= 1 && index_slc < m_depth - 1);
	
	unsigned short counter = 0;
	return counter;
}


unsigned short Volume::special_cases(int index_row, int index_col, int index_slc, glm::vec3 color) 
{
	unsigned short counter = 0;
	assert(index_row == 0 || index_row == m_width - 1 || index_col == 0 || index_col == m_height - 1 || index_slc == 0 || index_slc == m_depth - 1);

	return counter;
}


std::wstring Volume::generate_name(std::wstring base_file_name, unsigned int counter) 
{
	std::wostringstream file_name;
	std::wstring number_prefix;
	if (counter < 10) { //Substraction is necesary since counter preincrements
		number_prefix = L"000";
	} else if (counter < 100) {
		number_prefix = L"00";
	} else if (counter < 1000) {
		number_prefix = L"0";
	}

	file_name << base_file_name << number_prefix << counter << L".tif";
	return file_name.str();
}

// back up
void Volume::save_to_raw(std::wstring file_name)
{
	std::ofstream output_header;
	std::ofstream output_data;
	output_header.open(file_name + L".hdr", std::ios::out);
	if (output_header)
	{
		output_header << m_width << " " << m_height << " " << m_depth;
		output_header.close();
	}

	//Create temporary buffer since in Bedrich program data is 4 channel
	//RGBA
	//unsigned char *temp_buffer = new unsigned char[4 * m_voxel_number];
	//for (int k = 0; k < m_depth; ++k) {
	//for (int j = 0; j < m_height; ++j) {
	//for (int i = 0; i < m_width; ++i) {
	//glm::vec3 voxel_color = get_color_at(i, j, k);
	//int real_index = 4 * (i + m_width * (j + m_depth * k));
	//temp_buffer[real_index] = static_cast<unsigned char>(255 * voxel_color.r);
	//temp_buffer[real_index + 1] = static_cast<unsigned char>(255 * voxel_color.g);
	//temp_buffer[real_index + 2] = static_cast<unsigned char>(255 * voxel_color.b);
	//temp_buffer[real_index + 3] = static_cast<unsigned char>((255.0f / 3.0f) * (voxel_color.r + voxel_color.g + voxel_color.b));
	//}
	//}
	//}

	//NO, it comes out that it was a GrayScaleImage
	unsigned char *temp_buffer = new unsigned char[m_voxel_number];
	for (int k = 0; k < m_depth; ++k)
	{
		for (int j = 0; j < m_height; ++j)
		{
			for (int i = 0; i < m_width; ++i)
			{
				glm::vec3 voxel_color = get_color_at(i, j, k);
				int real_index = i + m_width * (j + m_depth * k);
				temp_buffer[real_index] = static_cast<unsigned char>((255.0f / 3.0f) * (voxel_color.r + voxel_color.g + voxel_color.b));
			}
		}
	}

	output_data.open(file_name + L".img", std::ios::out | std::ios::binary);
	if (output_data)
	{
		//output_data.write((char *)temp_buffer, 4 * m_voxel_number * sizeof(unsigned char));
		output_data.write((char *)temp_buffer, m_voxel_number * sizeof(unsigned char));
		output_data.close();
	}

	delete[] temp_buffer;
}

void Volume::save_to_medfile(std::wstring file_name) 
{
	std::ofstream output_header;
	std::ofstream output_data;
	output_header.open(file_name + L".mhd", std::ios::out);
	if (output_header) 
	{
		output_header << "NDims = 3" << std::endl;
		output_header << "DimSize = " << m_width << " " << m_height << " " << m_depth << std::endl;
		output_header << "ElementType = MET_UCHAR" << std::endl;
		// FIXME: the spacing should be specified specially
		output_header << "ElementSpacing = 0.0127651 0.0127389 0.0128079" << std::endl;
		output_header << "ElementByteOrderMSB = False" << std::endl;
		// construct the raw file name
		std::wstring::size_type index = file_name.find_last_of(L"//");
		std::wstring raw_file_name = file_name.substr(index + 1);
		raw_file_name = raw_file_name + L".raw";
		unsigned len = raw_file_name.size() * 4;
		setlocale(LC_CTYPE, "");
		char *p = new char[len];
		wcstombs(p, raw_file_name.c_str(), len);
		std::string raw_file_name_string(p);
		delete[] p;
		output_header << "ElementDataFile = " << raw_file_name_string << std::endl;
		output_header.close();
	}

	//Create temporary buffer since in Bedrich program data is 4 channel
	//RGBA
	/*unsigned char *temp_buffer = new unsigned char[4 * m_voxel_number];
	for (int k = 0; k < m_depth; ++k) {
		for (int j = 0; j < m_height; ++j) {
			for (int i = 0; i < m_width; ++i) {
				glm::vec3 voxel_color = get_color_at(i, j, k);
				int real_index = 4 * (i + m_width * (j + m_depth * k));
				temp_buffer[real_index    ] = static_cast<unsigned char>(255 * voxel_color.r);
				temp_buffer[real_index + 1] = static_cast<unsigned char>(255 * voxel_color.g);
				temp_buffer[real_index + 2] = static_cast<unsigned char>(255 * voxel_color.b);
				temp_buffer[real_index + 3] = static_cast<unsigned char>((255.0f / 3.0f) * (voxel_color.r + voxel_color.g + voxel_color.b));
			}
		}
	}*/

	//NO, it comes out that it was a GrayScaleImage
	unsigned char *temp_buffer = new unsigned char[m_voxel_number];
	for (int k = 0; k < m_depth; ++k)
	{
		for (int j = 0; j < m_height; ++j)
		{
			for (int i = 0; i < m_width; ++i)
			{
				glm::vec3 voxel_color = get_color_at(i, j, k);
				int real_index = i + m_width * (j + m_depth * k);
				temp_buffer[real_index] = static_cast<unsigned char>((255.0f / 3.0f) * (voxel_color.r + voxel_color.g + voxel_color.b));
			}
		}
	}

	output_data.open(file_name + L".raw", std::ios::out | std::ios::binary);
	if (output_data)
	{
		output_data.write((char *)temp_buffer, m_voxel_number * sizeof(unsigned char));
		output_data.close();
	}

	delete[] temp_buffer;
}

void Volume::print_debug() {
	/*for (int index = 0; index < 3 * m_voxel_number; ++index) {
		std::wcout << index << " = " << m_volume_data[index] << std::endl;
	}*/
	for (int k = 0; k < m_depth; k++) {
		for (int i = 0; i < m_width; i++) {
			for (int j = 0; j < m_height; j++) {
				glm::vec3 color = get_color_at(i, j, k);
				unsigned char red = static_cast<unsigned char>(255 * color.r);
				unsigned char green = static_cast<unsigned char>(255 * color.g);
				unsigned char blue = static_cast<unsigned char>(255 * color.b);
				std::wcout << "x=" << i << " y=" << j << " z=" << k << " L =(" << red << ", " << green << ", " << blue << ")" << std::endl;
			}
		}
	}
}

unsigned char* Volume::get_pointer_to_data_rgba()
{
	unsigned char *temp_buffer = new unsigned char[4 * m_voxel_number];
	for (int k = 0; k < m_depth; ++k)
	{
		for (int i = 0; i < m_width; ++i) 
		{
			for (int j = 0; j < m_height; ++j)
			{
				glm::vec3 voxel_color = get_color_at(i, j, k);
				int real_index_rgba = 4 * (i + m_width * (j + m_height * k));
				temp_buffer[real_index_rgba] = static_cast<unsigned char>(255.0f * voxel_color.r);
				temp_buffer[real_index_rgba + 1] = static_cast<unsigned char>(255.0f * voxel_color.g);
				temp_buffer[real_index_rgba + 2] = static_cast<unsigned char>(255.0f * voxel_color.b);
				temp_buffer[real_index_rgba + 3] = static_cast<unsigned char>((1.0f / 3.0f) * (voxel_color.r + voxel_color.g + voxel_color.b));
			}
		}
	}
	return temp_buffer;
}
}
