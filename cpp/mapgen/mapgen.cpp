// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <vector>

#include "mapgen.h"
#include "diamondsquare/diamondsquare.h"
#include "mandelbrot/mandelbrot.h"
#include "static/static.h"
#include "perlinnoise/perlin.h"
#include "../log/log.h"

namespace openage {
namespace mapgen {


MapGen::MapGen(int chunk_size, MapGen::Engine engine,uint64_t seed) {
	log::log(MSG(dbg) <<
		"new MapGen:"
		" chunk size=" << chunk_size <<
		", seed=" << seed <<
		", engine id=" << engine);
	
	switch(engine) {
		case Engine::Mandelbrot :
			this->map = new mapgen::Mandelbrot(chunk_size,seed);
			break;
		case Engine::DiamondSquare :
			this->map = new mapgen::Diamondsquare(chunk_size,seed);
			break;
		case Engine::Static :
			this->map = new mapgen::Static(chunk_size,seed);
			break;
		case Engine::Perlin :
			this->map = new mapgen::Perlin(chunk_size,seed);
			break;
	}
}

MapGen::~MapGen() {
	delete(this->map);
}

int *MapGen::getMap(int32_t x, int32_t y) {
	return this->map->get_map(x,y);
}

int *MapGen::getMap() {
	return this->getMap(0,0);
}

coord::tile_delta MapGen::get_size() const {
	return this->map->size;
}

} // namespace mapgen
} // namespace openage
