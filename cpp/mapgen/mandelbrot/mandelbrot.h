// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_MAPGEN_MANDELBROT_MANDELBROT_H_
#define OPENAGE_MAPGEN_MANDELBROT_MANDELBROT_H_

#include <stddef.h>
#include <vector>

#include "../../coord/tile.h"
#include "../mapgenbase.h"

namespace openage {
namespace mapgen {

/* generates a map in shape of the Mandelbrot Set:
 * http://en.wikipedia.org/wiki/Mandelbrot_set
 */

class Mandelbrot : public MapGenBase {
public:

	Mandelbrot(int chunk_size, uint64_t seed);
	~Mandelbrot();

	int *get_map(int32_t x, int32_t y);
private:
	int mandelbrot(int x,int maxx, int y, int maxy);

};

} // namespace mapgen
} // namespace openage

#endif
