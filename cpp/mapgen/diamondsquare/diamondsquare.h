// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_MAPGEN_DIAMONDSQUARE_H_
#define OPENAGE_MAPGEN_DIAMONDSQUARE_H_

#include <stddef.h>
#include <vector>

#include "../../coord/tile.h"
#include "../mapgenbase.h"

namespace openage {
namespace mapgen {

	
class Diamondsquare : public MapGenBase {
public:
	
	Diamondsquare(int chunks_per_size, int chunk_size);
	~Diamondsquare();

	int *generate();

};

} // namespace mapgen
} // namespace openage

#endif
