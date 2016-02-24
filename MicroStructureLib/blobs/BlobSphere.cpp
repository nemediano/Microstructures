#include "BlobSphere.h"

namespace blobs {
	
	BlobSphere::BlobSphere() {
		m_blob_pointer = nullptr;
		m_center = glm::vec3();
	}

	BlobSphere::BlobSphere(const BlobSphere& copy) {
		m_blob_pointer = copy.m_blob_pointer;
		m_center = copy.m_center;
	}

	BlobSphere& BlobSphere::operator=(const BlobSphere& righ) {
		if (this == &righ) {
			return *this;
		}
		m_blob_pointer = righ.m_blob_pointer;
		m_center = righ.m_center;
		return *this;
	}

	BlobSphere::BlobSphere(Blob* b, glm::vec3 center) {
		assert(b != nullptr);
		m_blob_pointer = b;
		m_center = center;
	}

	float BlobSphere::evaluate_at(glm::vec3 x) {
		return m_blob_pointer->fblob(glm::distance(x, m_center));
	}

	glm::vec3 BlobSphere::get_lower_limit() {
		glm::vec3 low_limit;
		for (int i = 0; i < 3; ++i) {
			low_limit[i] = m_center[i] - m_blob_pointer->getA();
		}
		return low_limit;
	}

	glm::vec3 BlobSphere::get_higher_limit() {
		glm::vec3 high_limit;
		for (int i = 0; i < 3; ++i) {
			high_limit[i] = m_center[i] + m_blob_pointer->getA();
		}
		return high_limit;
	}

	BlobSphere::~BlobSphere() {
	}
	
}
