#ifndef TEXTURE_HANDLER_H_
#define TEXTURE_HANDLER_H_

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <string>


namespace image {
class TextureHandler {

public:
	TextureHandler();
	TextureHandler(std::wstring file_name);
	TextureHandler(int width, int height);
	~TextureHandler();
	bool update(unsigned char* new_data);
	void bind();
	bool open_gl_error();
	int get_width();
	int get_height();

private:
	unsigned char* m_data;
	unsigned int m_width;
	unsigned int m_height;
	GLuint m_texture_id;
};
}
#endif