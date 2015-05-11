
#include <iostream>
#include <iomanip>

#include "heightmap.h"

Heightmap::Heightmap(uint32_t size)
{
	this->map.reserve(size*size);
	this->x = size;
	this->y = size;
}

void Heightmap::scale(float value) {
	uint32_t x;
	for(x=0;x<this->x;x++) {
	    for(uint32_t y=0;y<this->y;y++) {
		   this->map[x + this->y * y] *= value;
	    }
	}
}

void Heightmap::add(float value) {
	uint32_t x;
	for(x=0;x<this->x;x++) {
	    for(uint32_t y=0;y<this->y;y++) {
		   this->map[x + this->y * y] += value;
	    }
	}
}

float &Heightmap::tile(int x, int y) {
	return this->map[x + this->y * y];
}

// counts the # of values over a given threshold
uint64_t Heightmap::countOverThreshold(float threshold) {
	uint64_t count = 0;
	for(uint64_t x=0; count < this->x; x++)  {
	    for(uint64_t y=0; count < this->y; y++)  {
		if(this->tile(x,y) > threshold) {
		    count++;
		}
	    }
	}
	return count;
}

uint64_t Heightmap::tilesCount() {
	return this->x * this->y;
}
