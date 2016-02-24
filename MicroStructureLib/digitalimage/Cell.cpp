#include "Cell.h"
#include "Image.h"
namespace digitalimage {

	Cell::Cell() {
		m_delta_world = 0.0f;
		m_position_grid[0] = 0;
		m_position_grid[1] = 0;
		m_position_grid[2] = 0;
		m_value = glm::vec3();
	}
	
	Cell::Cell(glm::vec3 grid, float delta, glm::vec3 color) {
		assert(grid.x >= 0.0f && grid.y >= 0.0f && grid.z >= 0.0f);
		assert(delta > 0.0f);
		assert(color.x >= 0.0f && color.y >= 0.0f && color.z >= 0.0f);
		m_delta_world = delta;
		m_position_grid[0] = static_cast<int>(grid.x);
		m_position_grid[1] = static_cast<int>(grid.y);
		m_position_grid[2] = static_cast<int>(grid.z);
		m_value = color;
	}

	Cell::Cell(glm::vec3 grid) {
		assert(grid.x >= 0.0f && grid.y >= 0.0f && grid.z >= 0.0f);
		m_delta_world = 0.0f;
		m_position_grid[0] = static_cast<int>(grid.x);
		m_position_grid[1] = static_cast<int>(grid.y);
		m_position_grid[2] = static_cast<int>(grid.z);
		m_value = black;
	}

	Cell::~Cell() {
	}

	glm::vec3 Cell::get_grid_position() {
		return glm::vec3(static_cast<float>(m_position_grid[0]), static_cast<float>(m_position_grid[1]), static_cast<float>(m_position_grid[2]));
	}

	void Cell::set_grid_position(int row, int col) {
		assert(row >= 0);
		assert(col >= 0);
		m_position_grid[0] = static_cast<unsigned int>(row);
		m_position_grid[1] = static_cast<unsigned int>(col);
		m_position_grid[2] = 0;
	}

	glm::vec3 Cell::get_world_position() {
		glm::vec3 world_pos;
		
		world_pos.x = m_delta_world * m_position_grid[0] + m_delta_world / 2.0f;
		world_pos.y = m_delta_world * m_position_grid[1] + m_delta_world / 2.0f;
		world_pos.z = m_delta_world * m_position_grid[2] + m_delta_world / 2.0f;

		return world_pos;
	}

	int Cell::get_row() {
		return m_position_grid[0];
	}

	int Cell::get_col() {
		return m_position_grid[1];
	}

	void Cell::set_world_delta(float delta) {
		assert(delta > 0.0f);
		m_delta_world = delta;
	}

	void Cell::set_color(glm::vec3 color) {
		assert(color.x >= 0.0f && color.y >= 0.0f && color.z >= 0.0f);
		m_value = color;
	}

	glm::vec3 Cell::get_color() {
		return m_value;
	}

}
