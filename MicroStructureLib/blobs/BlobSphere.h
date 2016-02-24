#ifndef BLOB_SPHERE_H_
#define BLOB_SPHERE_H_
#include <glm/glm.hpp>
#include "Blob.h"
namespace blobs {
	class BlobSphere {
		private:
			Blob* m_blob_pointer;
			glm::vec3 m_center;
			float distance_to_segment(glm::vec3 point);

		public:
			BlobSphere();
			BlobSphere(const BlobSphere& copy);
			BlobSphere& operator=(const BlobSphere& righ);
			BlobSphere(Blob* b, glm::vec3 end_point_1);
			glm::vec3 get_lower_limit();
			glm::vec3 get_higher_limit();
			float evaluate_at(glm::vec3 x);
			virtual ~BlobSphere();
	};
}

#endif