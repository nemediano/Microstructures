#ifndef SAMPLER_H_
#define SAMPLER_H_
#include <vector>
#include <glm/glm.hpp>
#include "../blobs/BlobCapsule.h"
#include "../blobs/BlobSphere.h"
#include "../digitalimage/Image.h"
#include "../digitalimage/Volume.h"

using namespace blobs;
using namespace digitalimage;

namespace experiment {
	class Sampler {
	private:
		glm::vec3 m_min_dimensions;
		glm::vec3 m_max_dimensions;
		vector<BlobCapsule*> m_blobs;
		vector<BlobSphere*> m_spheres;
	public:
		Sampler();
		Sampler(glm::vec3 min_dimensions, glm::vec3 max_dimensions);
		void add_blob(BlobCapsule* blob);
		void add_blob(BlobSphere* blob);
		void sample(Image& grid);
		void sample_spheres(Image& grid);
		void sample_capsules(Image& grid);
		void sample(Volume& grid);
		void sample_spheres(Volume& grid);
		void sample_capsules(Volume& grid);
		virtual ~Sampler();
	};
}
#endif
