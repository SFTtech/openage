// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <vector>

#include "mandelbrot.h"
#include "../../log/log.h"
#include "../mapgen.h"
#include "../mapgenbase.h"

namespace openage {
namespace mapgen {


Mandelbrot::Mandelbrot(int chunks_per_side, int chunk_size)
	:
	MapGenBase(chunks_per_side,chunk_size) {}

	
int *Mandelbrot::generate() {
  
	for (int y = 0; y < this->size.se; y++) {
		for (int x = 0; x < this->size.ne; x++) {
			int index = this->size.ne * y + x;
			if(mandelbrot(x,this->size.se,y,this->size.ne) == 0) {
				this->map[index] = MapGen::Terrain::Dirt_1;
			} else {
				this->map[index] = MapGen::Terrain::Water_Dark;
			}
		}
	}

	return this->map.data();
}

int Mandelbrot::mandelbrot(int x,int maxx, int y, int maxy) {
	/* x.y Transformation to -2 .. 1 */
	double cx = -2.0 + 3.0 * x/maxx;
	double cy = -1.0 + 2.0 * y/maxy;
	
	double zx = 0.0, zy = 0.0;
	double zx2 = 0.0, zy2 = 0.0;
	
	int iteration = 0;
	int max_iteration = 999;

	while ( zx*zx + zy*zy <= (2*2) && iteration < max_iteration )
	{
		zy = 2.0 * zx * zy + cy;
		zx = zx2 - zy2 + cx;
		zx2 = zx * zx;
		zy2 = zy * zy;
		iteration++;
	}
	if ( iteration == max_iteration ) {
		return 0;
	}
	return 1;
}

} // namespace mapgen
} // namespace openage
