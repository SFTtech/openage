// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_ENGINE_PERLIN_H_
#define OPENAGE_ENGINE_PERLIN_H_

#include <stdint.h>
#include <libnoise/noise.h>

#include "heightmap.h"
#include "../mapgen.h"
#include "../mapgenbase.h"

namespace openage {
namespace mapgen {

class Perlin : public MapGenBase {

public:

	Perlin(int chunk_size, uint64_t seed);

	int *get_map(int32_t x, int32_t y);

private:
	float frequency;
	float lacunarity;
	float persistence;
	uint16_t octavecount;
	static const uint64_t scaleParameter = 20;

};

} // namespace mapgen
} // namespace openage

#endif
