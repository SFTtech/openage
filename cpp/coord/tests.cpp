// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <unistd.h>
#include "../log/log.h"
#include "../util/error.h"
#include "../util/file.h"
#include "phys2.h"
#include "phys3.h"
#include "tile.h"
#include "tile3.h"
#include "chunk.h"
#include "camgame.h"
#include "camhud.h"
#include "../engine.h"
#include "../console/console.h"

namespace openage {
namespace coord {
namespace tests {

/**
 * This function tests fixed point numbers (the FixedPoint class)
 */
int fixed_point_0() {
	phys_t expected;
	phys_t results;

	int stage = 0;
	if (phys_t{3} + phys_t{5} != phys_t{8}) { return stage; }

	stage = 1;
	if (phys_t{1} * phys_t{1} != phys_t{1}) { return stage; }

	stage = 2;
	results = phys_t{2};
	results *= phys_t{5};
	if (results != phys_t{10}) { return stage; }

	stage = 3;
	if (phys_t{12} / phys_t{4} != phys_t{3}) { return stage; }

	stage = 4;
	if (phys_t{1} / phys_t{2} != phys_t::from_raw(32768)) { return stage; }

	stage = 5;
	// Test calculations with 2-powers (1/2, 1/4 and 1/8 etc. should be fine)
	if (phys_t{0.5f} * phys_t{3} != phys_t{1.5f}) { return stage; }

	stage = 6;
	if (phys_t{7} - 1 != phys_t{4} + 2) { return stage; }

	return -1;
}

/**
 * This function tests the methods of phys2.
 */
int phys2_0() {
	int stage = 0;
	
	phys2 p{10, 10};
	phys3 expected_p3{10, 10, 0};
	
	stage += 1; // 1
	// Test convert to phys3 with default arg
	
	phys3 results_p3 = p.to_phys3();
	if (not (results_p3 == expected_p3)) { return stage; }
	
	stage += 1; // 2
	// Test convert to phys3 with arg
	expected_p3.up = -5;
	
	results_p3 = p.to_phys3(-5);
	if (not (results_p3 == expected_p3)) { return stage; }
	
	stage += 1; // 3
	// Test phys size just under a whole tile
	tile expected_tile{0, 0};
	
	p = {phys_t::from_raw(65535), phys_t::from_raw(65535)};
	
	tile results_tile = p.to_tile();
	if (not (results_tile == expected_tile)) { return stage; }
	
	stage += 1; // 4
	// Test phys size right at a whole tile size
	expected_tile = {1, 2};
	
	p = {phys_t::from_raw(65536), phys_t::from_raw(131072)};

	results_tile = p.to_tile();
	if ( not(results_tile == expected_tile)) { return stage; }
	
	stage += 1; // 5
	// Test phys size in the middle of tile size
	expected_tile = {1, 2};
	
	p = {phys_t::from_raw(65789), phys_t::from_raw(142311)};
	
	results_tile = p.to_tile();
	if ( not(results_tile == expected_tile)) { return stage; }
	
	stage += 1; // 6
	// Test get fraction
	// As seen above 65536 is a whole tile, so here test getting the
	// Fraction with the whole tiles removed
	phys2_delta expected_p2d{phys_t::from_raw(32768), phys_t::from_raw(32768)};
	
	p = {phys_t::from_raw(98304), phys_t::from_raw(98304)};
	
	phys2_delta results_p2d = p.get_fraction();
	if (not (results_p2d == expected_p2d)) { return stage; }
	
	return -1;
}

/**
 * This function tets the methods of phys2_delta.
 */
int phys2_delta_0() {
	int stage = 0;
	
	phys2_delta pd{10, 10};
	phys3_delta expected_p3d{10, 10, 0};
	
	stage += 1; // 1
	// Test convert to phys3 with default arg
	
	phys3_delta results_p3d = pd.to_phys3();
	if (not (results_p3d == expected_p3d)) { return stage; }
	
	stage += 1; // 2
	// Test convert to phys3 with arg
	expected_p3d.up = -5;
	
	results_p3d = pd.to_phys3(-5);
	if (not (results_p3d == expected_p3d)) { return stage; }
	
	return -1;
}

/**
 * This function tests the methods of tile.
 */
int tile_0() {
	int stage = 0;
	
	tile3 expected_t3{10, 10, 0};
	tile t{10, 10};
	
	stage += 1; // 1
	// Test to_tile3 with default arg
	
	tile3 results_t3 = t.to_tile3();
	if (not (results_t3 == expected_t3)) { return stage; }
	
	stage += 1; // 2
	// Test to_tile3 with arg
	
	expected_t3.up = 10;
	
	results_t3 = t.to_tile3(10);
	if (not (results_t3 == expected_t3)) { return stage; }
	
	stage += 1; // 3
	// Test to_phys2 with default arg
	
	phys2 expected_p2{phys_t::from_raw(32768), phys_t::from_raw(32768)};
	
	t = {0, 0};
	
	phys2 results_p2 = t.to_phys2();
	if (not (results_p2 == expected_p2)) { return stage; }
	
	stage += 1; // 4
	// Test to_phys with default arg and non zero tile
	
	expected_p2 = {phys_t::from_raw(98304), phys_t::from_raw(98304)};
  
	t = {1, 1};
	
	results_p2 = t.to_phys2();
	if (not (results_p2 == expected_p2)) { return stage; }
	
	stage += 1; // 5
	// Test to_phys with arg
	
	expected_p2 = {phys_t::from_raw(16384), phys_t::from_raw(16384)};
	
	t = {0, 0};
	
	results_p2 = t.to_phys2({phys_t::from_raw(16384), phys_t::from_raw(16384)});
	if (not (results_p2 == expected_p2)) { return stage; }
	
	stage += 1; // 6
	// Test to_chunk just below whole chunck size
	
	chunk expected_chunk{0, 0};
	
	t = {15, 15};
	
	chunk results_chunk = t.to_chunk();
	if ( not(results_chunk == expected_chunk)) { return stage; }
	
	stage += 1; // 7
	// Test to_chunk just at whole chunk size
	
	expected_chunk = {1, 1};
	
	t = {16, 16};
	
	results_chunk = t.to_chunk();
	if (not (results_chunk == expected_chunk)) { return stage; }
	
	stage += 1; // 8
	// Test to_chunk over a whole chunk size

	expected_chunk = {1, 1};
	
	t = {20, 20};
	
	results_chunk = t.to_chunk();
	if (not (results_chunk == expected_chunk)) { return stage; }
	
	stage += 1; // 9
	// Test get_pos_on_chunk
	// Similar to phys2.get_fraction
	
	tile_delta expected_td{4, 4};

	t = {20, 20};
	
	tile_delta results_td = t.get_pos_on_chunk();
	if (not (results_td == expected_td)) { return stage; }

	return -1;
}

/**
 * This function tests the methods of tile_delta
 */
int tile_delta_0() {
	int stage = 0;
	// Test tile2_delta to tile3_delta with default arg

	tile3_delta expected_t3d{10, 10, 0};
	tile_delta t2d{10, 10};

	tile3_delta results_t3d = t2d.to_tile3();
	if( not(results_t3d == expected_t3d)) { return stage; }

	stage += 1; // 1
	// Test tile2_delta to tile3_delta with arg

	expected_t3d = {10, 10, 10};

	results_t3d = t2d.to_tile3(10);
	if( not(results_t3d == expected_t3d)) { return stage; }

	stage += 1; // 2
	// Test converting to tile

	tile expected_tile{10, 10};

	tile results_tile = t2d.to_tile();
	if( not(results_tile == expected_tile)) { return stage; }

	return -1;
}

/**
 * This function tests convert back and forth between tile and phys2.
 */
int tile_and_phys2_0() {
	int stage = 0;
	
	tile expected_tile{1, 1};
	tile t{1, 1};
	
	stage += 1; // 1
	// Test convert tile to phys2 and back
	
	tile results_tile = t.to_phys2().to_tile();
	if (not (results_tile == expected_tile)) { return stage; }
	
	stage += 1; // 2
	// Test convert phys2 to tile and back, phys under whole tile size
	
	phys2 expected_p2{0, 0};
	phys2 p2{phys_t::from_raw(65535), phys_t::from_raw(65535)};
	
	phys2 results_p2 = p2.to_tile().to_phys2({0, 0});
	if (not (results_p2 == expected_p2)) { return stage; }
	
	stage += 1; // 3
	// Test convert phys2 to tile and back, phys at whole tile size
	
	expected_p2 = {phys_t::from_raw(65536), phys_t::from_raw(65536)};
	p2 = {phys_t::from_raw(65536), phys_t::from_raw(65536)};
	
	results_p2 = p2.to_tile().to_phys2({0, 0});
	if (not (results_p2 == expected_p2)) { return stage; }
	
	return -1;
}

/**
 * This functions tests the methods of chunk
 */
int chunk_0() {
	int stage = 0;

	tile expected_tile{0, 0};
	chunk c{0, 0};
	tile_delta td{0, 0};

	tile results_tile = c.to_tile(td);
	if (not (results_tile == expected_tile)) { return stage; }

	stage += 1; // 1

	expected_tile = {16, 16};
	c = {1, 1};

	results_tile = c.to_tile(td);
	if( not(results_tile == expected_tile)) { return stage; }

	stage += 1; // 2

	expected_tile = {24, 24};
	c = {1, 1};
	td = {8, 8};

	results_tile = c.to_tile(td);
	if ( not(results_tile == expected_tile)) { return stage; }

	return -1;
}

/**
 * This function tests the methods of phys3.
 */
int phys3_0() {
	int stage = 0;
	// Test the up component getting cut off

	coord_data* engine_coord_data{ Engine::get_coord_data() };

	phys2 expected_p2{10, 10};
	phys3 p3{10, 10, 10};

	phys2 results_p2 = p3.to_phys2();
	if( not(results_p2 == expected_p2)) { return stage; }
	
	stage += 1; // 1
	// Test to camgame
	
	// Get the current camgame_phys position
	phys3 gameP3 = engine_coord_data->camgame_phys;

	// Since we know we start centered out camgame should be
	// at 0, 0
	camgame expected_c{0, 0};
	
	camgame results_c = gameP3.to_camgame();
	if( not(results_c == expected_c)) { return stage; } 
	
	stage += 1; // 2
	// Test the change in camgame_phys causes a camgame change
	
	// the base camgame_phy pos is {655360, 655360, 0}
	// by adding one tile(65536 to each component of camgame_phys
	// we should see -96 in x from moving one ne and one se
	// then -24 from moving one up
	expected_c = {-96, -24};
	engine_coord_data->camgame_phys = {phys_t::from_raw(720896), phys_t::from_raw(720896), phys_t::from_raw(65536)};
	
	results_c = gameP3.to_camgame();
	if( not(results_c == expected_c)) { return stage; } 

	stage += 1; // 3
	// Testing phys3 to tile3 just under a whole tile size

	tile3 expected_tile{0, 0, 0}; 
	p3 = {0.99f, 0.99f, 0.99f};

	tile3 results_tile = p3.to_tile3();
	if (not(results_tile == expected_tile)) { return stage; }

	stage += 1; // 4
	// Testing phys3 to tile3 at whole tile sizes

	expected_tile = {1, 2, 3};
	p3 = {phys_t::from_raw(65536), phys_t::from_raw(131072), phys_t::from_raw(196608)};

	results_tile = p3.to_tile3();
	if( not(results_tile == expected_tile)) { return stage; }

	stage += 1; // 5
	// Testing phys3 to tile3 above whole tile sizes

	expected_tile = {1, 2, 3};
	p3 = {phys_t::from_raw(85641), phys_t::from_raw(171072), phys_t::from_raw(205465)};

	results_tile = p3.to_tile3();
	if( not(results_tile == expected_tile)) { return stage; }

	stage += 1; // 6
	// Testing phys3 get_fraction just below whole tile size

	phys3_delta expected_p3d{phys_t::from_raw(65535), phys_t::from_raw(65535), phys_t::from_raw(65535)};
	p3 = {phys_t::from_raw(65535), phys_t::from_raw(131071), phys_t::from_raw(196607)};

	phys3_delta results_p3d = p3.get_fraction();
	if( not(results_p3d == expected_p3d)) { return stage; }

	stage += 1; // 7
	// Testing phys3 get_fraction at whole tile size

	expected_p3d = {0, 0, 0};
	p3 = {phys_t::from_raw(65536), phys_t::from_raw(131072), phys_t::from_raw(196608)};

	results_p3d = p3.get_fraction();
	if( not(results_p3d == expected_p3d)) { return stage; }
	
	stage += 1; // 8
	// Testing phys3 get_fraction above whole tile size

	expected_p3d = {phys_t::from_raw(1), phys_t::from_raw(1), phys_t::from_raw(1)};
	p3 = {phys_t::from_raw(65537), phys_t::from_raw(131073), phys_t::from_raw(196609)};

	results_p3d = p3.get_fraction();
	if( not(results_p3d == expected_p3d)) { return stage; }

	return -1;
}

/**
 * This function tests the methods of phys3_delta.
 */
int phys3_delta_0() {
	int stage = 0;
	// Moving 1 tile se or ne is equivalent to 48 on x, up has no influence on x
	// Moving 1 tile up is equivalent to 24 on y, se and ne are also 24 but subtract from each other

	// phys3_delta to camgame_delta 1 tile ne
	camgame_delta expected_cd{48, 24};
	phys3_delta p3d{1, 0, 0};
	
	camgame_delta results_cd = p3d.to_camgame();
	if( not(results_cd == expected_cd)) { return stage; }

	stage += 1; // 1
	// Testing that se has the same effect on x and negative on y

	expected_cd = {48, -24};
	p3d = {0, 1, 0};

	results_cd = p3d.to_camgame();
	if( not(results_cd == expected_cd)) { return stage; }

	stage += 1; // 2
	// Testing that up effects y only

	expected_cd = {0, 24};
	p3d = {0, 0, 1};

	results_cd = p3d.to_camgame();
	if( not(results_cd == expected_cd)) { return stage; }

	stage += 1; // 3
	// Testing ne and se add on x and cancel on y

	expected_cd = {96, 0};
	p3d = {1, 1, 0};

	results_cd = p3d.to_camgame();
	if( not(results_cd == expected_cd)) { return stage; }

	stage += 1; // 4
	// Testing phys3_delta to phys2_delta
	
	phys2_delta expected_p2d{10, 10};
	p3d = {10, 10, 10};

	phys2_delta results_p2d = p3d.to_phys2();
	if( not(results_p2d == expected_p2d)) { return stage; }

	return -1;
}

/**
 * This function tests the methods of tile3.
 */
int tile3_0() {
	int stage = 0;
	// Tests up portion getting cut off

	tile expected_t2{10, 10};
	tile3 t3{10, 10, 10};

	tile results_t2 = t3.to_tile();
	if( not(results_t2 == expected_t2)) { return stage; }

	stage += 1; // 1
	// Tests tile3 to phys3 with default arg

	phys3 expected_p3{0.5f, 0.5f, 0};
	t3 = {0, 0, 0};

	phys3 results_p3 = t3.to_phys3();
	if( not(results_p3 == expected_p3)) { return stage; }

	stage += 1; // 2
	// Tests tile3 to phys3 with 0's for arg

	expected_p3 = {0, 0, 0};

	results_p3 = t3.to_phys3({0, 0 ,0});
	if( not(results_p3 == expected_p3)) { return stage; }

	stage += 1; // 3
	// Test tile3 to phys3 with args

	expected_p3 = {1, 2, 3};
	t3 = {1, 2, 3};

	results_p3 = t3.to_phys3({0, 0, 0});
	if( not(results_p3 == expected_p3)) { return stage; }

	return -1;
}

/**
 * This function tests the methods for camgame.
 */
int camgame_0() {
	int stage = 0;
	// Test camgame to phys3 with default arg and 0,0 camgame
	// to show we get the engines camgame_phys back

	coord_data* engine_coord_data{ Engine::get_coord_data() };

	// Reset camgame_phys to its normal starting value
	engine_coord_data->camgame_phys = {10, 10, 0};
	phys3 expected_p3 = engine_coord_data->camgame_phys;
	camgame c{0, 0};

	phys3 results_p3 = c.to_phys3();
	if( not(results_p3 == expected_p3)) { return stage; }
	
	stage += 1; // 1
	// Test camgame to phys3 with default arg and x=1 for camagame

	// This is the base camgame_phys position {655360, 655360, 0}
	// plus ((1 pixel / 48(px per tile)) * 65536 phys per tile ) / 2
	// for both the ne and se components since we only added x.
	// Also this get rounded down so we end up adding 682 to ne and se
	expected_p3 = {phys_t::from_raw(656042), phys_t::from_raw(656042), 0};
	c = {1, 0};

	results_p3 = c.to_phys3();
	if( not(results_p3 == expected_p3)) { return stage; }

	stage += 1; // 2
	// Test camgame to phys3 with default arg and y=1 for camgame

	// This is the base camgame_phys position {655360, 655360, 0}
	// plus ((1 pixel / 24(px per tile)) * 65536 phys per tile ) / 2
	// this add to ne and subracted from se
	// Also this get rounded down so we end up adding 1365 to ne and se
	expected_p3 = {phys_t::from_raw(656725), phys_t::from_raw(653995), 0};
	c = {0, 1};
	
	results_p3 = c.to_phys3();
	if( not(results_p3 == expected_p3)) { return stage; }

	stage += 1; // 3
	// Test camgame to phys3 with a passed arg

	// Here we start with the same ne and se ad before but we add/subract
	// the passed arg divided by 2. The passed arg is also applied directly
	// to the up component
	expected_p3 = {phys_t::from_raw(656726), phys_t::from_raw(653994), phys_t::from_raw(2)};

	results_p3 = c.to_phys3(phys_t::from_raw(2));
	if( not(results_p3 == expected_p3)) { return stage; }

	stage += 1; // 4
	// Test camgame to window, camgame at 0,0

	// The default window is 800x600 so our window offset is 400,300
	window expected_w{400, 300};
	c = {0, 0};

	window results_w = c.to_window();
	if( not(results_w == expected_w)) { return stage; }

	stage += 1; // 5
	// Test camgame to window, camgame at 100, 100

	expected_w = {500, 200};
	c = {100, 100};

	results_w = c.to_window();
	if( not(results_w == expected_w)) { return stage; }

	return -1;
}

/**
 * This function tests the methods of camgame_delta.
 */
int camgame_delta_0() {
	int stage = 0;
	// Test camgame_delta to phys3_delta with default arg
	phys3_delta expected_p3d{0, 0, 0};
	camgame_delta cd{0, 0};

	phys3_delta results_p3d = cd.to_phys3();
	if( not(results_p3d == expected_p3d)) { return stage; }
	
	stage += 1; // 1
	// Test camgame_delta to phys_delta with default arg and x = 1

	// This is ((1 pixel / 48(px per tile)) * 65536 phys per tile ) / 2
	// for both the ne and se components since we only added x.
	// Also this get rounded down so we end up adding 682 to ne and se
	expected_p3d = {phys_t::from_raw(682), phys_t::from_raw(682), 0};
	cd = {1, 0};

	results_p3d = cd.to_phys3();
	if( not(results_p3d == expected_p3d)) { return stage; }

	stage += 1; // 2
	// Test camgame_delta to phys3_delta with default arg and y = 1

	// This is ((1 pixel / 24(px per tile)) * 65536 phys per tile ) / 2
	// this add to ne and subracted frim se
	// Also this get rounded down so we end up adding 1365 to ne and se
	expected_p3d = {phys_t::from_raw(1365), phys_t::from_raw(-1365), 0};
	cd = {0, 1};

	results_p3d = cd.to_phys3();
	if( not(results_p3d == expected_p3d)) { return stage; }

	stage += 1; // 3
	// Test camgame_delta to phys3_delta with arg and y = 1

	// Here we start with the same ne and se ad before but we add/subract
	// the passed arg divided by 2. The passed arg is also applied directly
	// to the up component
	expected_p3d = {phys_t::from_raw(1366), phys_t::from_raw(-1366), phys_t::from_raw(2)};
	
	results_p3d = cd.to_phys3(phys_t::from_raw(2));
	if( not(results_p3d == expected_p3d)) { return stage; }

	stage += 1; // 4
	// Test camgame_delta to window_delta

	window_delta expected_wd{0, 0};
	cd = {0, 0};

	window_delta results_wd = cd.to_window();
	if( not(results_wd == expected_wd)) { return stage; }

	stage += 1; // 5
	// Test camgame_delta to window_delta, show y flips
	
	expected_wd = {100, -100};
	cd = {100, 100};

	results_wd = cd.to_window();
	if( not(results_wd == expected_wd)) { return stage; }

	return -1;
}

/**
 * This function tests the methods of camhud.
 */
int camhud_0() {
	int stage = 0;
	// Test camhud to window

	// The center is the one place camhud and window are the same
	window expected_w{400, 300};
	camhud c{400, 300};

	window results_w = c.to_window();
	if( not(results_w == expected_w)) { return stage; }
	
	stage += 1; // 1
	// Test camhud to window
	
	// camhud is always the same x as window but the height of the
	// screen below window
	expected_w = {0, 600};
	c = {0, 0};

	results_w = c.to_window();
	if( not(results_w == expected_w)) { return stage; }

	stage += 1; // 2
	// Test camhud to term
	
	// TODO The expected results for this was created by just seeing
	// What the results acually were. Need to investigate the bottomleft
	// and top right of console as a toptight that is the camhud type
	// is negative in its y and thus would be off the screen

	console::Console con;

	window size{80*con.charsize.x, 25*con.charsize.y};
	con.on_resize(size);

	stage += 1; //  3

	term expected_t{1, -54};
	c = {12, 6};
	
	term results_t = c.to_term(&con);
	if( not(results_t == expected_t)) { return stage; }

	return -1;
}

/**
 * This function tests the methods of camhud_delta.
 */
int camhud_delta_0() {
	int stage = 0;
	// Test camhud_delta to window_delta, show y is flipped
	// TODO The expected results for this was created by just seeing
	// What the results acually were. Need to investigate the bottomleft
	// and top right of console as a toptight that is the camhud type
	// is negative in its y and thus would be off the screen

	window_delta expected_wd{100, -100};
	camhud_delta cd{100, 100};

	window_delta results_wd = cd.to_window();
	if( not(results_wd == expected_wd)) { return stage; }
	

	return -1;
}

/**
 * This function tests the methods of term
 */
int term_0() {
	int stage = 0;

	console::Console con;
	camhud expected_c{40, 340};
	term t{5, 5};

	camhud results_c = t.to_camhud(&con);
	if( not(results_c == expected_c)) { return stage; }

	return -1;
}

/**
 * Top level function for coord testing.
 */
void coord() {
	int ret;
	const char *testname;
	
	if ((ret = fixed_point_0()) != -1) {
		testname = "fixed_point test";
		goto out;
	}

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
	
	if ((ret = tile_delta_0()) != -1) {
		testname = "tile_delta test";
		goto out;
	}

	if ((ret = tile_and_phys2_0()) != -1) {
		testname = "tile and phys2 test";
		goto out;
	}
	
	if ((ret = chunk_0()) != -1) {
		testname = "chunk test";
		goto out;
	}

	if ((ret = phys3_0()) != -1) {
		testname = "phys3 test";
		goto out;
	}
	
	if ((ret = phys3_delta_0()) != -1) {
		testname = "phys3 delta test";
		goto out;
	}

	if ((ret = tile3_0()) != -1) {
		testname = "tile3 test";
		goto out;
	}

	if ((ret = camgame_0()) != -1) {
		testname = "camgame test";
		goto out;
	}

	if ((ret = camgame_delta_0()) != -1) {
		testname = "camgame delta test";
		goto out;
	}

	if ((ret = camhud_0()) != -1) {
		testname = "camhud test";
		goto out;
	}

	if ((ret = camhud_delta_0()) != -1) {
		testname = "camhud delta test";
		goto out;
	}

	if ((ret = term_0()) != -1) {
		testname = "term test";
		goto out;
	}

	log::log(MSG(info) << "Coord test passed");
	return;

out:
	log::log(MSG(err) << testname << " failed at stage " << ret);
	throw "failed coord tests";
}

} // namespace tests
} // namespace coord
} // namespace openage
