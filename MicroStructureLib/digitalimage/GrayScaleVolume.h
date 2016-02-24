#ifndef GRAY_SCALE_VOLUME_H_
#define GRAY_SCALE_VOLUME_H_

#include <cassert>
#include "Volume.h"

namespace digitalimage {

template <typename numType>
class GrayScaleVolume {
public:
	GrayScaleVolume();
	GrayScaleVolume(int width, int height, int depth);
	GrayScaleVolume(const GrayScaleVolume& copy);
	GrayScaleVolume& operator=(const GrayScaleVolume& rhs);
	GrayScaleVolume(const Volume& volume);
	//GrayScaleVolume(std::wstring fileName);
	void set_at(int x_index, int y_index, int z_index, numType value);
	void reste_to(numType value);
	numType get_at(int x_index, int y_index, int z_index);
	~GrayScaleVolume();

private:
	
	numType* m_data;
	numType m_max_value;
	numType m_min_value;

	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_depth;

	numType conver_to(const float& value);
};

//Dig how to do the template specialization
template <typename numType>
numType GrayScaleVolume<numType>::conver_to(const float& value) {
	double result = 0.0;
	if (value > 1.0) {
		result = 1.0;
	} else if (value < 0.0) {
		result = 0.0;
	}

	return static_cast<numType>(result);
}

template <typename numType>
GrayScaleVolume<numType>::GrayScaleVolume() : m_width(0), m_height(0), m_depth(0), m_data(nullptr) {
	
}

template <typename numType>
GrayScaleVolume<numType>::GrayScaleVolume(const Volume& volume) {
	m_width = static_cast<unsigned int>(volume.get_width());
	m_height = static_cast<unsigned int>(volume.get_height());
	m_depth = static_cast<unsigned int>(volume.get_depth());

	for (int i = 0; i < m_width; ++i) {
		for (int j = 0; j < m_height; ++j) {
			for (int k = 0; k < m_depth; ++k) {
				glm::vec3 color = volume.get_color_at(i, j, k);
				float gray_level = 0.2989.0f * color.r + 0.5870.0f * color.g + 0.1140.0f * color.b;
				set_color_at(i, j, k, convert_to(gray_level));
			}
		}
	}
}

template <typename numType>
GrayScaleVolume<numType>::GrayScaleVolume(int width, int height, int depth) {
	assert(width > 0);
	assert(height > 0);
	assert(depth > 0);

	m_width = static_cast<unsigned int>(width);
	m_height = static_cast<unsigned int>(height);
	m_depth = static_cast<unsigned int>(depth);

	m_data = new numType[m_width * m_height * m_depth];

	reste_to(static_cast<numType>(0));
}

template <typename numType>
GrayScaleVolume<numType>::GrayScaleVolume(const GrayScaleVolume& copy) {
	m_width = copy.m_width;
	m_height = copy.m_height;
	m_depth = copy.m_depth;

	m_data = new numType[m_width * m_height * m_depth];

	std::copy(copy.m_data, copy.m_data + (m_width * m_height * m_depth), m_data);
}

template <typename numType>
GrayScaleVolume<numType>& GrayScaleVolume<numType>::operator=(const GrayScaleVolume<numType>& rhs) {
	if (this == &rhs) {
		return *this;
	}

	delete [] m_data;

	m_width = rhs.m_width;
	m_height = rhs.m_height;
	m_depth = rhs.m_depth;

	m_data = new numType[m_width * m_height * m_depth];

	std::copy(rhs.m_data[0], rhs.m_data[m_width * m_height * m_depth], m_data[0]);
	
	return this;
}

template <typename numType>
void GrayScaleVolume<numType>::set_at(int x_index, int y_index, int z_index, numType value) {
	assert(x_index >= 0);
	assert(static_cast<unsigned int>(x_index) < m_width);
	assert(y_index >= 0);
	assert(static_cast<unsigned int>(y_index) < m_height);
	assert(z_index >= 0);
	assert(static_cast<unsigned int>(z_index) < m_depth);
	
	int real_index = x_index + m_width * (y_index + m_depth * z_index);
	m_data[real_index] = value;

	if (value > m_max_value) {
		m_max_value = value;
	}

	if (value < m_min_value) {
		m_min_value = value;
	}

}

template <typename numType>
numType GrayScaleVolume<numType>::get_at(int x_index, int y_index, int z_index) {
	assert(x_index >= 0);
	assert(static_cast<unsigned int>(x_index) < m_width);
	assert(y_index >= 0);
	assert(static_cast<unsigned int>(y_index) < m_height);
	assert(z_index >= 0);
	assert(static_cast<unsigned int>(z_index) < m_depth);

	int real_index = x_index + m_width * (y_index + m_depth * z_index);
	return m_data[real_index];
}

template <typename numType>
GrayScaleVolume<numType>::~GrayScaleVolume() {
	delete[] m_data;
}

template <typename numType>
void GrayScaleVolume<numType>::reste_to(numType value) {
	for (unsigned int i = 0; i < m_width * m_height * m_depth; ++i) {
		m_data[i] = value;
	}
}

}
#endif