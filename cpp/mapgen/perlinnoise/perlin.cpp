// Copyright 2015-2015 the openage authors. See copying.md for legal info.
#include "perlin.h"
#include <stdint.h>
#include <iostream>

namespace openage {
namespace mapgen {

Perlin::Perlin(int chunk_size, uint64_t seed)
	    :
	    MapGenBase(chunk_size, seed) {

	    /* set default parameters */
	    this->frequency = 1.0;
	    this->lacunarity = 2.0;
	    this->persistence = 0.5;
	    this->octavecount = 6;
}


int *Perlin::get_map(int32_t x, int32_t y)
{

	/* terrain engine uses diffrent coordinate system
	 * terrain chunk:
	 *           ( 1,-1)
	 *    ( 0,-1)       ( 1, 0)
	 *(-1,-1)    ( 0, 0)    ( 1, 1)
	 *    (-1, 0)       ( 0, 1)
	 *           (-1, 1)
	 * me:
	 * (-1, 1)( 0, 1)( 1. 1)
	 * (-1, 0)( 0, 0)( 1, 0)
	 * (-1,-1)( 0,-1)( 1,-1)
	 * so, we transform the system:
	 */
	int32_t t = x;
	x = y;
	y= t;

	Heightmap *map = new Heightmap(this->chunk_size);
	noise::module::Perlin noise;
	noise.SetSeed(this->seed);

	/* set parameters */
	noise.SetFrequency(  this->frequency);
	noise.SetLacunarity( this->lacunarity);
	noise.SetPersistence(this->persistence);
	noise.SetOctaveCount( this->octavecount);

	//calculate all tiles
	for(int32_t xlocal=0;xlocal<this->chunk_size;xlocal++) {
	  for(int32_t ylocal=0;ylocal<this->chunk_size;ylocal++) {

	    // chunk_size conversion to int64 is necessary
	    int64_t x64 = x * (int64_t)this->chunk_size + xlocal;
	    double  xd = (double)x64/ (double)this->scaleParameter;

	    int64_t y64 = y * (int64_t)this->chunk_size + ylocal;
	    double  yd = (double)y64/ (double)this->scaleParameter;

	    // chunks uses other Cartesian coordinate system, therfore inverse y
	    map->tile(xlocal,ylocal) = noise.GetValue(xd,this->chunk_size-yd-1,0.6);

	    // Heighmap to Terrain IDs
	    if( map->tile(xlocal,ylocal) < -0.4 ) {
		  this->map[this->chunk_size*ylocal+xlocal] = mapgen::MapGen::Terrain::Water_Dark;
	    } else if ( map->tile(xlocal,ylocal) < 0.0 ) {
		    this->map[this->chunk_size*ylocal+xlocal] = mapgen::MapGen::Terrain::Water_Light;
	    } else {
	      this->map[this->chunk_size*ylocal+xlocal] = mapgen::MapGen::Terrain::Grass_1;
	    }

	  }
	}

	return this->map.data();
}

} // namespace mapgen
} // namespace openage

