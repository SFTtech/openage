// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include <unistd.h>

#include "../log/log.h"
#include "../util/file.h"
#include "../engine.h"
#include "../console/console.h"
#include "../testing/testing.h"

#include "phys2.h"
#include "phys3.h"
#include "tile.h"
#include "tile3.h"
#include "chunk.h"
#include "camgame.h"
#include "camhud.h"

namespace openage {
namespace coord {
namespace tests {

/**
 * This function tests the methods of phys2.
 */
void phys2_0() {
	phys2 p{10, 10};
	phys3 expected_p3{10, 10, 0};

	// Test convert to phys3 with default arg

	phys3 results_p3 = p.to_phys3();
	(results_p3 == expected_p3) or TESTFAIL;

	// Test convert to phys3 with arg
	expected_p3.up = -5;

	results_p3 = p.to_phys3(-5);
	(results_p3 == expected_p3) or TESTFAIL;

	// Test phys size just under a whole tile
	tile expected_tile{0, 0};

	p = {65535, 65535};

	tile results_tile = p.to_tile();
	(results_tile == expected_tile) or TESTFAIL;

	// Test phys size right at a whole tile size
	expected_tile = {1, 2};

	p = {65536, 131072};

	results_tile = p.to_tile();
	(results_tile == expected_tile) or TESTFAIL;

	// Test phys size in the middle of tile size
	expected_tile = {1, 2};

	p = {65789, 142311};

	results_tile = p.to_tile();
	(results_tile == expected_tile) or TESTFAIL;

	// Test get fraction
	// As seen above 65536 is a whole tile, so here test getting the
	// Fraction with the whole tiles removed
	phys2_delta expected_p2d{32768, 32768};

	p = {98304, 98304};

	phys2_delta results_p2d = p.get_fraction();
	(results_p2d == expected_p2d) or TESTFAIL;
}

/**
 * This function tets the methods of phys2_delta.
 */
void phys2_delta_0() {
	phys2_delta pd{10, 10};
	phys3_delta expected_p3d{10, 10, 0};

	// Test convert to phys3 with default arg
	phys3_delta results_p3d = pd.to_phys3();
	(results_p3d == expected_p3d) or TESTFAIL;

	// Test convert to phys3 with arg
	expected_p3d.up = -5;

	results_p3d = pd.to_phys3(-5);
	(results_p3d == expected_p3d) or TESTFAIL;
}

/**
 * This function tests the methods of tile.
 */
void tile_0() {
	tile3 expected_t3{10, 10, 0};
	tile t{10, 10};

	// Test to_tile3 with default arg
	tile3 results_t3 = t.to_tile3();
	(results_t3 == expected_t3) or TESTFAIL;

	// Test to_tile3 with arg
	expected_t3.up = 10;

	results_t3 = t.to_tile3(10);
	(results_t3 == expected_t3) or TESTFAIL;

	// Test to_phys2 with default arg
	phys2 expected_p2{32768, 32768};

	t = {0, 0};

	phys2 results_p2 = t.to_phys2();
	(results_p2 == expected_p2) or TESTFAIL;

	// Test to_phys with default arg and non zero tile
	expected_p2 = {98304, 98304};

	t = {1, 1};

	results_p2 = t.to_phys2();
	(results_p2 == expected_p2) or TESTFAIL;

	// Test to_phys with arg

	expected_p2 = {16384, 16384};

	t = {0, 0};

	results_p2 = t.to_phys2({16384, 16384});
	(results_p2 == expected_p2) or TESTFAIL;

	// Test to_chunk just below whole chunck size
	chunk expected_chunk{0, 0};

	t = {15, 15};

	chunk results_chunk = t.to_chunk();
	(results_chunk == expected_chunk) or TESTFAIL;

	// Test to_chunk just at whole chunk size
	expected_chunk = {1, 1};

	t = {16, 16};

	results_chunk = t.to_chunk();
	(results_chunk == expected_chunk) or TESTFAIL;

	// Test to_chunk over a whole chunk size
	expected_chunk = {1, 1};

	t = {20, 20};

	results_chunk = t.to_chunk();
	(results_chunk == expected_chunk) or TESTFAIL;

	// Test get_pos_on_chunk
	// Similar to phys2.get_fraction
	tile_delta expected_td{4, 4};

	t = {20, 20};

	tile_delta results_td = t.get_pos_on_chunk();
	(results_td == expected_td) or TESTFAIL;
}

/**
 * This function tests the methods of tile_delta
 */
void tile_delta_0() {
	// Test tile2_delta to tile3_delta with default arg

	tile3_delta expected_t3d{10, 10, 0};
	tile_delta t2d{10, 10};

	tile3_delta results_t3d = t2d.to_tile3();
	(results_t3d == expected_t3d) or TESTFAIL;

	// Test tile2_delta to tile3_delta with arg
	expected_t3d = {10, 10, 10};

	results_t3d = t2d.to_tile3(10);
	(results_t3d == expected_t3d) or TESTFAIL;

	// Test converting to tile
	tile expected_tile{10, 10};

	tile results_tile = t2d.to_tile();
	(results_tile == expected_tile) or TESTFAIL;
}

/**
 * This function tests convert back and forth between tile and phys2.
 */
void tile_and_phys2_0() {
	tile expected_tile{1, 1};
	tile t{1, 1};

	// Test convert tile to phys2 and back
	tile results_tile = t.to_phys2().to_tile();
	(results_tile == expected_tile) or TESTFAIL;

	// Test convert phys2 to tile and back, phys under whole tile size
	phys2 expected_p2{0, 0};
	phys2 p2{65535, 65535};

	phys2 results_p2 = p2.to_tile().to_phys2({0, 0});
	(results_p2 == expected_p2) or TESTFAIL;

	// Test convert phys2 to tile and back, phys at whole tile size
	expected_p2 = {65536, 65536};
	p2 = {65536, 65536};

	results_p2 = p2.to_tile().to_phys2({0, 0});
	(results_p2 == expected_p2) or TESTFAIL;
}

/**
 * This functions tests the methods of chunk
 */
void chunk_0() {
	tile expected_tile{0, 0};
	chunk c{0, 0};
	tile_delta td{0, 0};

	tile results_tile = c.to_tile(td);
	(results_tile == expected_tile) or TESTFAIL;

	expected_tile = {16, 16};
	c = {1, 1};

	results_tile = c.to_tile(td);
	(results_tile == expected_tile) or TESTFAIL;

	expected_tile = {24, 24};
	c = {1, 1};
	td = {8, 8};

	results_tile = c.to_tile(td);
	(results_tile == expected_tile) or TESTFAIL;
}

/**
 * This function tests the methods of phys3.
 */
void phys3_0() {
	// Test the up component getting cut off
	coord_data* engine_coord_data{ Engine::get_coord_data() };

	phys2 expected_p2{10, 10};
	phys3 p3{10, 10, 10};

	phys2 results_p2 = p3.to_phys2();
	(results_p2 == expected_p2) or TESTFAIL;

	// Test to camgame
	// Get the current camgame_phys position
	phys3 gameP3 = engine_coord_data->camgame_phys;

	// Since we know we start centered out camgame should be
	// at 0, 0
	camgame expected_c{0, 0};

	camgame results_c = gameP3.to_camgame();
	(results_c == expected_c) or TESTFAIL;

	// Test the change in camgame_phys causes a camgame change

	// the base camgame_phy pos is {655360, 655360, 0}
	// by adding one tile(65536 to each component of camgame_phys
	// we should see -96 in x from moving one ne and one se
	// then -24 from moving one up
	expected_c = {-96, -24};
	engine_coord_data->camgame_phys = {720896, 720896, 65536};

	results_c = gameP3.to_camgame();
	(results_c == expected_c) or TESTFAIL;

	// Testing phys3 to tile3 just under a whole tile size
	tile3 expected_tile{0, 0, 0};
	p3 = {65535, 65535, 65535};

	tile3 results_tile = p3.to_tile3();
	(results_tile == expected_tile) or TESTFAIL;

	// Testing phys3 to tile3 at whole tile sizes
	expected_tile = {1, 2, 3};
	p3 = {65536, 131072, 196608};

	results_tile = p3.to_tile3();
	(results_tile == expected_tile) or TESTFAIL;

	// Testing phys3 to tile3 above whole tile sizes
	expected_tile = {1, 2, 3};
	p3 = {85641, 171072, 205465};

	results_tile = p3.to_tile3();
	(results_tile == expected_tile) or TESTFAIL;

	// Testing phys3 get_fraction just below whole tile size
	phys3_delta expected_p3d{65535, 65535, 65535};
	p3 = {65535, 131071, 196607};

	phys3_delta results_p3d = p3.get_fraction();
	(results_p3d == expected_p3d) or TESTFAIL;

	// Testing phys3 get_fraction at whole tile size
	expected_p3d = {0, 0, 0};
	p3 = {65536, 131072, 196608};

	results_p3d = p3.get_fraction();
	(results_p3d == expected_p3d) or TESTFAIL;

	// Testing phys3 get_fraction above whole tile size

	expected_p3d = {1, 1, 1};
	p3 = {65537, 131073, 196609};

	results_p3d = p3.get_fraction();
	(results_p3d == expected_p3d) or TESTFAIL;
}

/**
 * This function tests the methods of phys3_delta.
 */
void phys3_delta_0() {
	// Moving 1 tile se or ne is equivalent to 48 on x, up has no influence on x
	// Moving 1 tile up is equivalent to 24 on y, se and ne are also 24 but subtract from each other

	// phys3_delta to camgame_delta 1 tile ne
	camgame_delta expected_cd{48, 24};
	phys3_delta p3d{65536, 0, 0};

	camgame_delta results_cd = p3d.to_camgame();
	(results_cd == expected_cd) or TESTFAIL;

	// Testing that se has the same effect on x and negative on y
	expected_cd = {48, -24};
	p3d = {0, 65536, 0};

	results_cd = p3d.to_camgame();
	(results_cd == expected_cd) or TESTFAIL;

	// Testing that up effects y only
	expected_cd = {0, 24};
	p3d = {0, 0, 65536};

	results_cd = p3d.to_camgame();
	(results_cd == expected_cd) or TESTFAIL;

	// Testing ne and se add on x and cancel on y
	expected_cd = {96, 0};
	p3d = {65536, 65536, 0};

	results_cd = p3d.to_camgame();
	(results_cd == expected_cd) or TESTFAIL;

	// Testing phys3_delta to phys2_delta
	phys2_delta expected_p2d{10, 10};
	p3d = {10, 10, 10};

	phys2_delta results_p2d = p3d.to_phys2();
	(results_p2d == expected_p2d) or TESTFAIL;
}

/**
 * This function tests the methods of tile3.
 */
void tile3_0() {
	// Tests up portion getting cut off
	tile expected_t2{10, 10};
	tile3 t3{10, 10, 10};

	tile results_t2 = t3.to_tile();
	(results_t2 == expected_t2) or TESTFAIL;

	// Tests tile3 to phys3 with default arg
	phys3 expected_p3{32768, 32768, 0};
	t3 = {0, 0, 0};

	phys3 results_p3 = t3.to_phys3();
	(results_p3 == expected_p3) or TESTFAIL;

	// Tests tile3 to phys3 with 0's for arg
	expected_p3 = {0, 0, 0};

	results_p3 = t3.to_phys3({0, 0 ,0});
	(results_p3 == expected_p3) or TESTFAIL;

	// Test tile3 to phys3 with args
	expected_p3 = {65536, 131072, 196608};
	t3 = {1, 2, 3};

	results_p3 = t3.to_phys3({0, 0, 0});
	(results_p3 == expected_p3) or TESTFAIL;
}

/**
 * This function tests the methods for camgame.
 */
void camgame_0() {
	// Test camgame to phys3 with default arg and 0,0 camgame
	// to show we get the engines camgame_phys back

	coord_data* engine_coord_data{ Engine::get_coord_data() };

	// Reset camgame_phys to its normal starting value
	engine_coord_data->camgame_phys = {655360, 655360, 0};
	phys3 expected_p3 = engine_coord_data->camgame_phys;
	camgame c{0, 0};

	phys3 results_p3 = c.to_phys3();
	(results_p3 == expected_p3) or TESTFAIL;

	// Test camgame to phys3 with default arg and x=1 for camagame

	// This is the base camgame_phys position {655360, 655360, 0}
	// plus ((1 pixel / 48(px per tile)) * 65536 phys per tile ) / 2
	// for both the ne and se components since we only added x.
	// Also this get rounded down so we end up adding 682 to ne and se
	expected_p3 = {656042, 656042, 0};
	c = {1, 0};

	results_p3 = c.to_phys3();
	(results_p3 == expected_p3) or TESTFAIL;

	// Test camgame to phys3 with default arg and y=1 for camagame

	// This is the base camgame_phys position {655360, 655360, 0}
	// plus ((1 pixel / 24(px per tile)) * 65536 phys per tile ) / 2
	// this add to ne and subracted from se
	// Also this get rounded down so we end up adding 1365 to ne and se
	expected_p3 = {656725, 653995, 0};
	c = {0, 1};

	results_p3 = c.to_phys3();
	(results_p3 == expected_p3) or TESTFAIL;

	// Test camgame to phys3 with a pased arg

	// Here we start with the same ne and se ad before but we add/subract
	// the passed arg divided by 2. The passed arg is also applied directly
	// to the up component
	expected_p3 = {656726, 653994, 2};

	results_p3 = c.to_phys3(2);
	(results_p3 == expected_p3) or TESTFAIL;

	// Test camgame to window, camgame at 0,0

	// The default window is 800x600 so our window offset is 400,300
	window expected_w{400, 300};
	c = {0, 0};

	window results_w = c.to_window();
	(results_w == expected_w) or TESTFAIL;

	// Test camgame to window, camgame at 100, 100

	expected_w = {500, 200};
	c = {100, 100};

	results_w = c.to_window();
	(results_w == expected_w) or TESTFAIL;
}

/**
 * This function tests the methods of camgame_delta.
 */
void camgame_delta_0() {
	// Test camgame_delta to phys3_delta with default arg
	phys3_delta expected_p3d{0, 0, 0};
	camgame_delta cd{0, 0};

	phys3_delta results_p3d = cd.to_phys3();
	(results_p3d == expected_p3d) or TESTFAIL;

	// Test camgame_delta to phys_delta with default arg and x = 1

	// This is ((1 pixel / 48(px per tile)) * 65536 phys per tile ) / 2
	// for both the ne and se components since we only added x.
	// Also this get rounded down so we end up adding 682 to ne and se
	expected_p3d = {682, 682, 0};
	cd = {1, 0};

	results_p3d = cd.to_phys3();
	(results_p3d == expected_p3d) or TESTFAIL;

	// Test camgame_delta to phys3_delta with default arg and y = 1

	// This is ((1 pixel / 24(px per tile)) * 65536 phys per tile ) / 2
	// this add to ne and subracted frim se
	// Also this get rounded down so we end up adding 1365 to ne and se
	expected_p3d = {1365, -1365, 0};
	cd = {0, 1};

	results_p3d = cd.to_phys3();
	(results_p3d == expected_p3d) or TESTFAIL;

	// Test camgame_delta to phys3_delta with arg and y = 1

	// Here we start with the same ne and se ad before but we add/subract
	// the passed arg divided by 2. The passed arg is also applied directly
	// to the up component
	expected_p3d = {1366, -1366, 2};

	results_p3d = cd.to_phys3(2);
	(results_p3d == expected_p3d) or TESTFAIL;

	// Test camgame_delta to window_delta

	window_delta expected_wd{0, 0};
	cd = {0, 0};

	window_delta results_wd = cd.to_window();
	(results_wd == expected_wd) or TESTFAIL;

	// Test camgame_delta to window_delta, show y flips

	expected_wd = {100, -100};
	cd = {100, 100};

	results_wd = cd.to_window();
	(results_wd == expected_wd) or TESTFAIL;
}

/**
 * This function tests the methods of camhud.
 */
void camhud_0() {
	// Test camhud to window

	// The center is the one place camhud and window are the same
	window expected_w{400, 300};
	camhud c{400, 300};

	window results_w = c.to_window();
	(results_w == expected_w) or TESTFAIL;

	// Test camhud to window

	// camhud is always the same x as window but the height of the
	// screen below window
	expected_w = {0, 600};
	c = {0, 0};

	results_w = c.to_window();
	(results_w == expected_w) or TESTFAIL;

	// Test camhud to term

	// TODO The expected results for this was created by just seeing
	// What the results acually were. Need to investigate the bottomleft
	// and top right of console as a toptight that is the camhud type
	// is negative in its y and thus would be off the screen

	console::Console con;

	window size{80*con.charsize.x, 25*con.charsize.y};
	con.on_resize(size);

	term expected_t{1, -54};
	c = {12, 6};

	term results_t = c.to_term(&con);
	(results_t == expected_t) or TESTFAIL;
}

/**
 * This function tests the methods of camhud_delta.
 */
void camhud_delta_0() {
	// Test camhud_delta to window_delta, show y is flipped
	// TODO The expected results for this was created by just seeing
	// What the results acually were. Need to investigate the bottomleft
	// and top right of console as a toptight that is the camhud type
	// is negative in its y and thus would be off the screen

	window_delta expected_wd{100, -100};
	camhud_delta cd{100, 100};

	window_delta results_wd = cd.to_window();
	(results_wd == expected_wd) or TESTFAIL;
}

/**
 * This function tests the methods of term
 */
void term_0() {
	console::Console con;
	camhud expected_c{40, 340};
	term t{5, 5};

	camhud results_c = t.to_camhud(&con);
	(results_c == expected_c) or TESTFAIL;
}

/**
 * Top level function for coord testing.
 */
void coord() {
	phys2_0();
	phys2_delta_0();
	tile_0();
	tile_delta_0();
	tile_and_phys2_0();
	chunk_0();
	phys3_0();
	phys3_delta_0();
	tile3_0();
	camgame_0();
	camgame_delta_0();
	camhud_0();
	camhud_delta_0();
	term_0();
}

}}} // openage::coord::tests
