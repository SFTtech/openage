// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "tests.h"

#include "../log.h"
#include "../util/error.h"
#include "phys2.h"
#include "phys3.h"
#include "tile.h"
#include "tile3.h"
#include "chunk.h"

namespace openage {
namespace coord {
namespace tests {

/**
 * This function tests the methods of phys2.
 */
int phys2_0() {
	int stage = 0;
	
	phys2 p2{10, 10};
	phys3 expectedP3{10, 10, 0};
	
	stage += 1; // 1
	// Test convert to phys3 with default arg
	
	phys3 p3 = p2.to_phys3();
	if (not (p3 == expectedP3)) { return stage; }
	
	stage += 1; // 2
	// Test convert to phys3 with arg
	expectedP3.up = -5;
	
	p3 = p2.to_phys3(-5);
	if (not (p3 == expectedP3)) { return stage; }
	
	stage += 1; // 3
	// Test phys size just under a whole tile
	tile expectedTile;
	expectedTile.ne = 0;
	expectedTile.se = 0;
	
	p2.ne = 65535;
	p2.se = 65535;
	
	tile t = p2.to_tile();
	if (not (t == expectedTile)) { return stage; }
	
	stage += 1; // 4
	// Test phys size right at a whole tile size
	expectedTile.ne = 1;
	expectedTile.se = 2;
	
	p2.ne = 65536;
	p2.se = 131072;

	t = p2.to_tile();
	if ( not(t == expectedTile)) { return stage; }
	
	stage += 1; // 5
	// Test phys size in the middle of tile size
	expectedTile.ne = 1;
	expectedTile.se = 2;
	
	p2.ne = 65789;
	p2.se = 142311;
	
	t = p2.to_tile();
	if ( not(t == expectedTile)) { return stage; }
	
	stage += 1; // 6
	// Test get fraction
  // As seen above 65536 is a whole tile, so here test getting the
  // Fraction with the whole tiles removed
	phys2_delta expectedP2D{32768, 32768};
	
	p2.ne = 98304;
	p2.se = 98304;
	
	phys2_delta p2D = p2.get_fraction();
  if (not (p2D == expectedP2D)) { return stage; }
	
	return -1;
}

/**
 * This function tets the methods of phys2_delta.
 */
int phys2_delta_0() {
	int stage = 0;
	
	phys2_delta p2d{10, 10};
	phys3_delta expectedP3D{10, 10, 0};
	
	stage += 1; // 1
	// Test convert to phys3 with default arg
	
	phys3_delta p3d = p2d.to_phys3();
	if (not (p3d == expectedP3D)) { return stage; }
	
	stage += 1; // 2
	// Test convert to phys3 with arg
	expectedP3D.up = -5;
	
	p3d = p2d.to_phys3(-5);
	if (not (p3d == expectedP3D)) { return stage; }
	
	return -1;
}

/**
 * This function tests the methods of tile.
 */
int tile_0() {
	int stage = 0;
	
	tile3 expectedT3{10, 10, 0};
	tile t{10, 10};
	
	stage += 1; // 1
	// Test to_tile3 with default arg
	
	tile3 t3 = t.to_tile3();
	if (not (t3 == expectedT3)) { return stage; }
	
	stage += 1; // 2
	// Test to_tile3 with arg
	
	expectedT3.up = 10;
	
	t3 = t.to_tile3(10);
	if (not (t3 == expectedT3)) { return stage; }
	
	stage += 1; // 3
	// Test to_phys2 with default arg
	
	phys2 expectedP2{32768, 32768};
	
	t.ne = 0;
	t.se = 0;
	
	phys2 p2 = t.to_phys2();
	if (not (p2 == expectedP2)) { return stage; }
	
	stage += 1; // 4
	// Test to_phys with default arg and non zero tile
	
	expectedP2.ne = 98304;
	expectedP2.se = 98304;
  
	t.ne = 1;
	t.se = 1;
	
	p2 = t.to_phys2();
	if (not (p2 == expectedP2)) { return stage; }
	
	stage += 1; // 5
	// Test to_phys with arg
	
	expectedP2.ne = 16384;
	expectedP2.se = 16384;
	
	t.ne = 0;
	t.se = 0;
	
	p2 = t.to_phys2({16384, 16384});
	if (not (p2 == expectedP2)) { return stage; }
	
	stage += 1; // 6
	// Test to_chunk just below whole chunck size
	
	chunk expectedChunk{0, 0};
	
	t.ne = 15;
	t.se = 15;
	
	chunk c = t.to_chunk();
	if ( not(c == expectedChunk)) { return stage; }
	
	stage += 1; // 7
	// Test to_chunk just at whole chunk size
	
	expectedChunk.ne = 1;
	expectedChunk.se = 1;
	
	t.ne = 16;
	t.se = 16;
	
	c = t.to_chunk();
	if (not (c == expectedChunk)) { return stage; }
	
	stage += 1; // 8
	// Test to_chunk over a whole chunk size

	expectedChunk.ne = 1;
	expectedChunk.se = 1;
	
	t.ne = 20;
	t.se = 20;
	
	c = t.to_chunk();
	if (not (c == expectedChunk)) { return stage; }
	
	stage += 1; // 9
	// Test get_pos_on_chunk
  // Same thing as phys2.get_fraction
	
	tile_delta expectedTD{32768, 32768};

  t.ne = 98304;
  t.se = 98304;
	
	tile_delta tD = t.get_pos_on_chunk();
  if (not (tD == expectedTD)) { return stage; }

	return -1;
}

/**
 * This function tests convert back and forth between tile and phys2.
 */
int tile_and_phys2_0() {
	int stage = 0;
	
	tile expectedTile{1, 1};
	tile t{1, 1};
	
	stage += 1; // 1
	// Test convert tile to phys2 and back
	
	tile other_t = t.to_phys2().to_tile();
	if (not (other_t == expectedTile)) { return stage; }
	
	stage += 1; // 2
	// Test convert phys2 to tile and back, phys under whole tile size
	
	phys2 expectedP2{0, 0};
	phys2 p2{65535, 65535};
	
	phys2 otherP2 = p2.to_tile().to_phys2({0, 0});
	if (not (otherP2 == expectedP2)) { return stage; }
	
	stage += 1; // 3
	// Test convert phys2 to tile and back, phys at whole tile size
	
	expectedP2.ne = 65536;
	expectedP2.se = 65536;
	
	p2.ne = 65536;
	p2.se = 65536;
	
	otherP2 = p2.to_tile().to_phys2({0, 0});
	if (not (otherP2 == expectedP2)) { return stage; }
	
	return -1;
}

/**
 * Top level function for coord testing.
 */
void coord() {
	int ret;
	const char *testname;
	
	if ((ret = phys2_0()) != -1) {
		testname = "phys2 test";
		goto out;
	}
	
	if ((ret = phys2_delta_0()) != -1) {
		testname = "phys2_delta test";
		goto out;
	}
	
	if ((ret = tile_0()) != -1) {
		testname = "tile test";
		goto out;
	}
	
	if ((ret = tile_and_phys2_0()) != -1) {
		testname = "tile and phys2 test";
		goto out;
	}
	
	return;

out:
	log::err("%s failed at stage %d", testname, ret);
	throw "failed coord tests";
}

} // namespace tests
} // namespace coord
} // namespace openage
