#include <cassert>
#include <algorithm>
#include "BlobCapsule.h"

namespace blobs {

	BlobCapsule::BlobCapsule() {
		m_blob_pointer = nullptr;
		m_end_point_1 = glm::vec3();
		m_end_point_2 = glm::vec3();
	}

	BlobCapsule::BlobCapsule(const BlobCapsule& copy) {
		m_blob_pointer = copy.m_blob_pointer;
		m_end_point_1 = copy.m_end_point_1;
		m_end_point_2 = copy.m_end_point_2;
	}

	BlobCapsule::BlobCapsule(Blob* blob_pointer, glm::vec3 end_point_1, glm::vec3 end_point_2) {
		assert(end_point_1 != end_point_2);
		m_blob_pointer = blob_pointer;
		m_end_point_1 = end_point_1;
		m_end_point_2 = end_point_2;
	}
	
	float BlobCapsule::evaluate_at(glm::vec3 x) {
		return m_blob_pointer->fblob(distance_to_segment(x));
	}

	glm::vec3 BlobCapsule::get_lower_limit() {
		glm::vec3 low_limit;
		for (int i = 0; i < 3; ++i) {
			low_limit[i] = min(m_end_point_1[i], m_end_point_2[i]);
			low_limit[i] -= m_blob_pointer->getA();
		}
		return low_limit;
	}

	glm::vec3 BlobCapsule::get_higher_limit() {
		glm::vec3 high_limit;
		for (int i = 0; i < 3; ++i) {
			high_limit[i] = max(m_end_point_1[i], m_end_point_2[i]);
			high_limit[i] += m_blob_pointer->getA();
		}
		return high_limit;
	}

	BlobCapsule::~BlobCapsule() {

	}

	float BlobCapsule::distance_to_segment(glm::vec3 point) {
		const float segment_lenght = glm::distance(m_end_point_1, m_end_point_2);

		if (segment_lenght == 0.0) {
			return glm::distance(point, m_end_point_1);
		}
		//Projection of the point
		const float t = glm::dot(point - m_end_point_1, m_end_point_2 - m_end_point_1) / (segment_lenght * segment_lenght);

		if (t < 0.0) {
			return glm::distance(point, m_end_point_1);
		} else if (t > 1.0) {
			return glm::distance(point, m_end_point_2);
		} else {
			const glm::vec3 projection = m_end_point_1 + t * (m_end_point_2 - m_end_point_1);
			return glm::distance(point, projection);
		}

	}

	BlobCapsule& BlobCapsule::operator=(const BlobCapsule& right_hand_side) {
		if (this == &right_hand_side) {
			return *this;
		}
		m_blob_pointer = right_hand_side.m_blob_pointer;
		m_end_point_1 = right_hand_side.m_end_point_1;
		m_end_point_2 = right_hand_side.m_end_point_2;
		return *this;
	}
}
