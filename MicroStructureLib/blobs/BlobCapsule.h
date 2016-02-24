#ifndef BLOB_CAPSULE_H_
#define BLOB_CAPSULE_H_
#include <glm/glm.hpp>
#include "Blob.h"
namespace blobs {
	class BlobCapsule {
		private:
			Blob* m_blob_pointer;
			glm::vec3 m_end_point_1;
			glm::vec3 m_end_point_2;
			float distance_to_segment(glm::vec3 point);

		public:
			BlobCapsule();
			BlobCapsule(const BlobCapsule& copy);
			BlobCapsule& operator=(const BlobCapsule& righ);
			BlobCapsule(Blob* b, glm::vec3 end_point_1, glm::vec3 end_point_2);
			float evaluate_at(glm::vec3 x);
			glm::vec3 get_lower_limit();
			glm::vec3 get_higher_limit();
			virtual ~BlobCapsule();
	};
}

#endif
