#include "TextureHandler.h"
#include "digitalimage/Image.h"

#include <cassert>
#include <iostream>

using namespace digitalimage;
namespace image {
	
	TextureHandler::TextureHandler() : m_data(nullptr), m_width(256), m_height(256) {
	    glGenTextures(1, &m_texture_id);
	}

	TextureHandler::TextureHandler(std::wstring file_name) {
		Image* image = new Image(file_name);
		
		m_width = image->get_width();
		m_height = image->get_height();

		m_data = image->get_pointer_to_data();

		glGenTextures(1, &m_texture_id);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glBindTexture(GL_TEXTURE_2D, m_texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_data);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//Just to be safe that no texture is bound
		glBindTexture(GL_TEXTURE_2D, 0);

		delete image;
	}

	TextureHandler::TextureHandler(int width, int height) {
		assert(width > 0);
		assert(height > 0);
		
		glGenTextures(1, &m_texture_id);

		m_height = height;
		m_width = width;
		
		m_data = new unsigned char[3 * m_width * m_height];

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glBindTexture(GL_TEXTURE_2D, m_texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//Just to be safe that no texture is bound
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	TextureHandler::~TextureHandler() {
		m_width = m_height = 0;
		if (m_data) {
			delete m_data;
		}
		glDeleteTextures(1, &m_texture_id);
	}

	bool TextureHandler::update(unsigned char* new_data) {
		assert(new_data != nullptr);
		if (m_data) {
			delete m_data;
		}
		m_data = new unsigned char[3 * m_width * m_height];
		memcpy(m_data, new_data, sizeof(unsigned char) * 3 * m_width * m_height);
		//After copyng new texture
		/*std::wcout << "Data sended to the GPU: [" << m_width << "x" << m_height << "]" << std::endl;
		for (unsigned int i = 0; i < m_width * m_height * 3; ++i) {
			std::wcout << "m_data[" << i << "] = " << m_data[i] << std::endl;
		}
		std::wcout << std::endl;*/

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, m_texture_id);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, m_data);
		return true;
	}

	void TextureHandler::bind() {
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindTexture(GL_TEXTURE_2D, m_texture_id);
	}

	bool TextureHandler::open_gl_error() {
		bool is_there_error = false;
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << err << std::endl;
			is_there_error = true;
		}
		return is_there_error;
	}

	int TextureHandler::get_width() {
		return m_width;
	}

	int TextureHandler::get_height() {
		return m_height;
	}

}