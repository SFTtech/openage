// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_MAPGEN_HEIGHTMAP_H_
#define OPENAGE_MAPGEN_HEIGHTMAP_H_

#include <vector>

#include "../../coord/tile.h"
#include "../../rng/rng.h"

namespace openage {
namespace mapgen {

/**
 * Heightmap generation using the diamond-square algorithm
 */
class Heightmap {
public:
	Heightmap(coord::tile_delta size);

	std::vector<float> &generate();

	float &tile(int x, int y);
private:
	int side_length;

	float displace(float value, int neighbors, float displacement);

	std::vector<float> map;
	rng::RNG rng;
};

} // namespace mapgen
} // namespace openage

#endif
