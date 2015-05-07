// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <vector>

#include "mapgen.h"
#include "diamondsquare/diamondsquare.h"
#include "mandelbrot/mandelbrot.h"
#include "static/static.h"
#include "../log/log.h"

namespace openage {
namespace mapgen {


MapGen::MapGen(int chunks_per_side,int chunk_size, MapGen::Engine engine) {
	switch(engine) {
		case Engine::Mandelbrot :
			this->map = new mapgen::Mandelbrot(chunks_per_side,chunk_size);
			break;
		case Engine::DiamondSquare :
			this->map = new mapgen::Diamondsquare(chunks_per_side,chunk_size);
			break;
		case Engine::Static :
			this->map = new mapgen::Static(chunks_per_side,chunk_size);
			break;
	}
}

int *MapGen::getMap(int32_t x, int32_t y) {
	this->map->generate();
	return this->map->map.data();
}

int *MapGen::getMap() {
	return this->getMap(0,0);
}

coord::tile_delta MapGen::get_size() const {
	return this->map->size;
}

} // namespace mapgen
} // namespace openage
