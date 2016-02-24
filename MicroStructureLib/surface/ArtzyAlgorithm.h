#ifndef ARTZY_ALGORITHM_H_
#define ARTZY_ALGORITHM_H_

#include <vector>
#include "../digitalimage/Volume.h"
#include "../digitalimage/BinaryVolume.h"
#include "../digitalimage/BoundaryElement.h"
#include "../meshes/Mesh.h"


using namespace digitalimage;
using namespace meshes;
using namespace std;

namespace surface {
class ArtzyAlgorithm {

protected:
	BinaryVolume m_spacial_ocupation;
	vector<BoundaryElement> m_seeds;
	vector<BoundaryElement> m_boundary;
	vector<Mesh> m_components;

public:
	ArtzyAlgorithm(Volume& volume, float threshold);
	ArtzyAlgorithm(BinaryVolume& volume);
	Mesh extract_surface(bool verbose = true, bool indexing = true);
	Mesh extract_surface(BoundaryElement seed, bool verbose = true, bool indexing = true);
	vector<Mesh> get_surfaces(bool verbose = true, bool indexing = true);
	bool more_surfaces();
	BinaryVolume get_segmented_volume(bool real_coordinates = true);
private:
	void track_boundary(BoundaryElement seed);
	void remove_from_volume();
	bool verify_seed(BoundaryElement seed);
	BoundaryElement find_seed();
	BoundaryElement next_face(BoundaryElement& fly, bool first);

};

enum CASE_NUMBER {ONE, TWO, THREE};

}

#endif