#include "terrain.h"
//so how the terrain will work is as a set of 9 terrain "chunks". Basically each time we check where the camera center is
// and then we do divide by 1024 each coordinate which gives us the integer coodinates.Once we get that 
// coordinate, we basically draw the neighbouring 8 chunks.