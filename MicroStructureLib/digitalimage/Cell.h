#ifndef CELL_H_
#define CELL_H_
#include<string>
#include<glm/glm.hpp>

namespace digitalimage {

class Cell {
private:
	unsigned int m_position_grid[3];
	float m_delta_world;
	glm::vec3 m_value;
	
public:
	Cell();
	Cell(glm::vec3 grid, float delta, glm::vec3 color);
	Cell(glm::vec3 grid);
	~Cell();
	glm::vec3 get_grid_position();
	void set_grid_position(int row, int col);
	glm::vec3 get_world_position();
	int get_row();
	int get_col();
	void set_world_delta(float delta);
	void set_color(glm::vec3 color);
	glm::vec3 get_color();
};

}
#endif
