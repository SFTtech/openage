// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_MAPGEN_MAPGEN_H_
#define OPENAGE_MAPGEN_MAPGEN_H_

#include <stddef.h>
#include <vector>

#include "../coord/tile.h"
#include "mapgenbase.h"

namespace openage {
namespace mapgen {

// TODO this should be a parameter to allow the correct map sizes
// For example, tiny, small and medium maps are of size 120x120, 144x144 and 168x168
// There's no chunk size which divides all these sizes evenly

class MapGen {
public:
	enum Engine {
		DiamondSquare,
		Mandelbrot,
		Perlin,
		Static,
	};

	//TODO create enum for all Terrain tiles
	enum Terrain : int {
		Dirt_1 = 2,
		Grass_1 = 0,
		Water_Light = 1,
		Water_Dark = 14,
	};

	MapGen(int chunk_size, MapGen::Engine enginge,uint64_t seed);
	~MapGen();

	int *getMap();
	int *getMap(int32_t x, int32_t y);
	coord::tile_delta get_size() const;

	// TODO define a method for returning placed objects,
	// like Town Center, Villagers, Scouts, Gold, Wood...
	// std::vector<objects> getObjects();


private:
	mapgen::MapGenBase  *map;
};


} // namespace mapgen
} // namespace openage

#endif
