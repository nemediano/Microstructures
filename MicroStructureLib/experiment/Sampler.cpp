#include <algorithm>
#include "Sampler.h"

namespace experiment {
	Sampler::Sampler(glm::vec3 min_dimensions, glm::vec3 max_dimensions) {
		m_min_dimensions = min_dimensions;
		m_max_dimensions = max_dimensions;
	}

	Sampler::~Sampler() {
		m_blobs.clear();
	}

	void Sampler::add_blob(BlobCapsule* blob) {
		m_blobs.push_back(blob);
	}

	void Sampler::add_blob(BlobSphere* blob) {
		m_spheres.push_back(blob);
	}

	void Sampler::sample(Image& grid) {
		grid.reset_to_color(black);
		
		if (m_blobs.size() > 0) {
			sample_capsules(grid);
		}

		if (m_spheres.size() > 0) {
			sample_spheres(grid);
		}
	}

	void Sampler::sample(Volume& grid) {
		grid.reset_to_color(black);
		
		if (m_blobs.size() > 0) {
			sample_capsules(grid);
		}

		if (m_spheres.size() > 0) {
			sample_spheres(grid);
		}
	}

	void Sampler::sample_spheres(Volume& grid) {

	}

	void Sampler::sample_capsules(Volume& grid) {

	}

	void Sampler::sample_spheres(Image& grid) {
		glm::vec3 sampling_delta = m_max_dimensions - m_min_dimensions;
		sampling_delta.x /= grid.get_width();
		sampling_delta.y /= grid.get_height();
		sampling_delta.z /= 1;
		glm::vec3 half_sampling_delta = 0.5f * sampling_delta;

		//Try to separate the float point from vector operations
		//Auxiliary memory to hold scalar field
		double** temporary_storage = new double* [grid.get_width()];
		for (int i = 0; i < grid.get_width(); ++i) {
			temporary_storage[i] = new double[grid.get_height()];
		}

		for (int i = 0; i < grid.get_width(); ++i) {
			for (int j = 0; j < grid.get_height(); ++j) {
				temporary_storage[i][j] = 0.0;
			}
		}

		for (auto blob : m_spheres) {
			glm::vec3 start = blob->get_lower_limit() - m_min_dimensions - half_sampling_delta;
			glm::vec3 stop = blob->get_higher_limit() - m_min_dimensions - half_sampling_delta;
			glm::vec3 color = white;
			//precompute indexes
			int start_i = max(0, static_cast<int>(start.x / sampling_delta.x));
			int start_j = max(0, static_cast<int>(start.y / sampling_delta.y));
			int stop_i = min(grid.get_width(), static_cast<int>(stop.x / sampling_delta.x));
			int stop_j = min(grid.get_height(), static_cast<int>(stop.y / sampling_delta.y));

			for (int i = start_i; i < stop_i; ++i) {
				for (int j = start_j; j < stop_j; ++j) {
					glm::vec3 indexes = glm::vec3(static_cast<float>(i), static_cast<float>(j), 0.0f);
					glm::vec3 voxel_center = indexes * sampling_delta + half_sampling_delta + m_min_dimensions;
					temporary_storage[i][j] += blob->evaluate_at(voxel_center);
				}
			}
		}

		for (int i = 0; i < grid.get_width(); ++i) {
			for (int j = 0; j < grid.get_height(); ++j) {
				if (temporary_storage[i][j] > 1.0) {
					temporary_storage[i][j] = 1.0;
				}
				glm::vec3 base_color = white;
				glm::vec3 color = static_cast<float>(temporary_storage[i][j]) * base_color;
				grid.set_color_at(i, j, color);
			}
		}

		//Delete temporary buffer
		for (int i = 0; i < grid.get_width(); ++i) {
			delete[] temporary_storage[i];
		}
		delete[] temporary_storage;
	}

	void Sampler::sample_capsules(Image& grid) {
		glm::vec3 sampling_delta = m_max_dimensions - m_min_dimensions;
		sampling_delta.x /= grid.get_width();
		sampling_delta.y /= grid.get_height();
		sampling_delta.z /= 1;
		glm::vec3 half_sampling_delta = 0.5f * sampling_delta;

		//Try to separate the float point from vector operations
		//Auxiliary memory to hold scalar field
		double** temporary_storage = new double* [grid.get_width()];
		for (int i = 0; i < grid.get_width(); ++i) {
			temporary_storage[i] = new double[grid.get_height()];
		}

		for (int i = 0; i < grid.get_width(); ++i) {
			for (int j = 0; j < grid.get_height(); ++j) {
				temporary_storage[i][j] = 0.0;
			}
		}

		for (auto blob : m_blobs) {
			glm::vec3 start = blob->get_lower_limit() - m_min_dimensions - half_sampling_delta;
			glm::vec3 stop = blob->get_higher_limit() - m_min_dimensions - half_sampling_delta;
			glm::vec3 color = white;
			//precompute indexes
			int start_i = max(0, static_cast<int>(start.x / sampling_delta.x));
			int start_j = max(0, static_cast<int>(start.y / sampling_delta.y));
			int stop_i = min(grid.get_width(), static_cast<int>(stop.x / sampling_delta.x));
			int stop_j = min(grid.get_height(), static_cast<int>(stop.y / sampling_delta.y));

			for (int i = start_i; i < stop_i; ++i) {
				for (int j = start_j; j < stop_j; ++j) {
					glm::vec3 indexes = glm::vec3(static_cast<float>(i), static_cast<float>(j), 0.0f);
					glm::vec3 voxel_center = indexes * sampling_delta + half_sampling_delta + m_min_dimensions;
					temporary_storage[i][j] += blob->evaluate_at(voxel_center);
				}
			}
		}

		for (int i = 0; i < grid.get_width(); ++i) {
			for (int j = 0; j < grid.get_height(); ++j) {
				if (temporary_storage[i][j] > 1.0) {
					temporary_storage[i][j] = 1.0;
				}
				glm::vec3 base_color = white;
				glm::vec3 color = static_cast<float>(temporary_storage[i][j]) * base_color;
				grid.set_color_at(i, j, color);
			}
		}

		//Delete temporary buffer
		for (int i = 0; i < grid.get_width(); ++i) {
			delete[] temporary_storage[i];
		}
		delete[] temporary_storage;
	}
}
