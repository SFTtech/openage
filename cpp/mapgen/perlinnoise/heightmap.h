// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_MAPGEN_PERLINNOISE_HEIGHTMAP_H_
#define OPENAGE_MAPGEN_PERLINNOISE_HEIGHTMAP_H_

#include <stdint.h>
#include <vector>


class Heightmap {
public:
	Heightmap(uint32_t size);
	float &tile(int x, int y);
	void scale(float value);
	void add(float value);
	uint64_t countOverThreshold(float threshold);
	uint64_t tilesCount();
	uint64_t x;
	uint64_t y;

private:
	std::vector<float> map;
};

#endif
