// Copyright 2016-2015 the openage authors. See copying.md for legal info.

#include "tests.h"

#include <unistd.h>
#include "../log.h"
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
 * This function tests the methods of phys2.
 */
int phys2_0() {
	int stage = 0;
	
	phys2 p{10, 10};
	phys3 expectedP3{10, 10, 0};
	
	stage += 1; // 1
	// Test convert to phys3 with default arg
	
	phys3 resultsP3 = p.to_phys3();
	if (not (resultsP3 == expectedP3)) { return stage; }
	
	stage += 1; // 2
	// Test convert to phys3 with arg
	expectedP3.up = -5;
	
	resultsP3 = p.to_phys3(-5);
	if (not (resultsP3 == expectedP3)) { return stage; }
	
	stage += 1; // 3
	// Test phys size just under a whole tile
	tile expectedTile{0, 0};
	
	p = {65535, 65535};
	
	tile resultsTile = p.to_tile();
	if (not (resultsTile == expectedTile)) { return stage; }
	
	stage += 1; // 4
	// Test phys size right at a whole tile size
	expectedTile = {1, 2};
	
	p = {65536, 131072};

	resultsTile = p.to_tile();
	if ( not(resultsTile == expectedTile)) { return stage; }
	
	stage += 1; // 5
	// Test phys size in the middle of tile size
	expectedTile = {1, 2};
	
	p = {65789, 142311};
	
	resultsTile = p.to_tile();
	if ( not(resultsTile == expectedTile)) { return stage; }
	
	stage += 1; // 6
	// Test get fraction
  // As seen above 65536 is a whole tile, so here test getting the
  // Fraction with the whole tiles removed
	phys2_delta expectedP2D{32768, 32768};
	
	p = {98304, 98304};
	
	phys2_delta resultsP2D = p.get_fraction();
  if (not (resultsP2D == expectedP2D)) { return stage; }
	
	return -1;
}

/**
 * This function tets the methods of phys2_delta.
 */
int phys2_delta_0() {
	int stage = 0;
	
	phys2_delta pD{10, 10};
	phys3_delta expectedP3D{10, 10, 0};
	
	stage += 1; // 1
	// Test convert to phys3 with default arg
	
	phys3_delta resultsP3D = pD.to_phys3();
	if (not (resultsP3D == expectedP3D)) { return stage; }
	
	stage += 1; // 2
	// Test convert to phys3 with arg
	expectedP3D.up = -5;
	
	resultsP3D = pD.to_phys3(-5);
	if (not (resultsP3D == expectedP3D)) { return stage; }
	
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
	
	tile3 resultsT3 = t.to_tile3();
	if (not (resultsT3 == expectedT3)) { return stage; }
	
	stage += 1; // 2
	// Test to_tile3 with arg
	
	expectedT3.up = 10;
	
	resultsT3 = t.to_tile3(10);
	if (not (resultsT3 == expectedT3)) { return stage; }
	
	stage += 1; // 3
	// Test to_phys2 with default arg
	
	phys2 expectedP2{32768, 32768};
	
	t = {0, 0};
	
	phys2 resultsP2 = t.to_phys2();
	if (not (resultsP2 == expectedP2)) { return stage; }
	
	stage += 1; // 4
	// Test to_phys with default arg and non zero tile
	
	expectedP2 = {98304, 98304};
  
	t = {1, 1};
	
	resultsP2 = t.to_phys2();
	if (not (resultsP2 == expectedP2)) { return stage; }
	
	stage += 1; // 5
	// Test to_phys with arg
	
	expectedP2 = {16384, 16384};
	
	t = {0, 0};
	
	resultsP2 = t.to_phys2({16384, 16384});
	if (not (resultsP2 == expectedP2)) { return stage; }
	
	stage += 1; // 6
	// Test to_chunk just below whole chunck size
	
	chunk expectedChunk{0, 0};
	
	t = {15, 15};
	
	chunk resultsChunk = t.to_chunk();
	if ( not(resultsChunk == expectedChunk)) { return stage; }
	
	stage += 1; // 7
	// Test to_chunk just at whole chunk size
	
	expectedChunk = {1, 1};
	
	t = {16, 16};
	
	resultsChunk = t.to_chunk();
	if (not (resultsChunk == expectedChunk)) { return stage; }
	
	stage += 1; // 8
	// Test to_chunk over a whole chunk size

	expectedChunk = {1, 1};
	
	t = {20, 20};
	
	resultsChunk = t.to_chunk();
	if (not (resultsChunk == expectedChunk)) { return stage; }
	
	stage += 1; // 9
	// Test get_pos_on_chunk
  // Similar to phys2.get_fraction
	
	// TODO This is not the results I expect. I would think that we
	// should get 4,4 back. Possibly a bug.
	tile_delta expectedTD{20, 20};

  t = {20, 20};
	
	tile_delta resultsTD = t.get_pos_on_chunk();
  if (not (resultsTD == expectedTD)) { return stage; }

	return -1;
}

/**
 * This function tests the methods of tile_delta
 */
int tile_delta_0() {
	int stage = 0;
	// Test tile2_delta to tile3_delta with default arg

	tile3_delta expectedT3D{10, 10, 0};
	tile_delta t2D{10, 10};

	tile3_delta resultsT3D = t2D.to_tile3();
	if( not(resultsT3D == expectedT3D)) { return stage; }

	stage += 1; // 1
	// Test tile2_delta to tile3_delta with arg

	expectedT3D = {10, 10, 10};

	resultsT3D = t2D.to_tile3(10);
	if( not(resultsT3D == expectedT3D)) { return stage; }

	stage += 1; // 2
	// Test converting to tile

	tile expectedTile{10, 10};

	tile resultsTile = t2D.to_tile();
	if( not(resultsTile == expectedTile)) { return stage; }

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
	
	tile resultsTile = t.to_phys2().to_tile();
	if (not (resultsTile == expectedTile)) { return stage; }
	
	stage += 1; // 2
	// Test convert phys2 to tile and back, phys under whole tile size
	
	phys2 expectedP2{0, 0};
	phys2 p2{65535, 65535};
	
	phys2 resultsP2 = p2.to_tile().to_phys2({0, 0});
	if (not (resultsP2 == expectedP2)) { return stage; }
	
	stage += 1; // 3
	// Test convert phys2 to tile and back, phys at whole tile size
	
	expectedP2 = {65536, 65536};
	p2 = {65536, 65536};
	
	resultsP2 = p2.to_tile().to_phys2({0, 0});
	if (not (resultsP2 == expectedP2)) { return stage; }
	
	return -1;
}

/**
 * This functions tests the methods of chunk
 */
int chunk_0() {
	int stage = 0;

	tile expectedTile{0, 0};
	chunk c{0, 0};
	tile_delta tD{0, 0};

	tile resultsTile = c.to_tile(tD);
	if (not (resultsTile == expectedTile)) { return stage; }

	stage += 1; // 1

	expectedTile = {16, 16};
	c = {1, 1};

	resultsTile = c.to_tile(tD);
	if( not(resultsTile == expectedTile)) { return stage; }

	stage += 1; // 2

	expectedTile = {24, 24};
	c = {1, 1};
	tD = {8, 8};

	resultsTile = c.to_tile(tD);
	if ( not(resultsTile == expectedTile)) { return stage; }

	return -1;
}

/**
 * This function tests the methods of phys3.
 */
int phys3_0() {
	int stage = 0;
	// Test the up component getting cut off

	phys2 expectedP2{10, 10};
	phys3 p3{10, 10, 10};

	phys2 resultsP2 = p3.to_phys2();
	if( not(resultsP2 == expectedP2)) { return stage; }
	
	stage += 1; // 1
	// Test to camgame

	Engine &e = Engine::get();
	
	// Get the current camgame_phys position
	phys3 gameP3 = e.camgame_phys;

  // Since we know we start centered out camgame should be
  // at 0, 0
	camgame expectedC{0, 0};
	
	camgame resultsC = gameP3.to_camgame();
	if( not(resultsC == expectedC)) { return stage; } 
	
	stage += 1; // 2
	// Test that camgame movement changes the distance to 
	// the given phys3
	
	expectedC = {-100, 99};
	e.move_phys_camera(100, 100, 1);
	
	resultsC = gameP3.to_camgame();
	if( not(resultsC == expectedC)) { return stage; } 
	
	stage += 1; // 3
	// Testing phys3 to tile3 just under a whole tile size

	tile3 expectedTile{0, 0, 0}; 
	p3 = {65535, 65535, 65535};

	tile3 resultsTile = p3.to_tile3();
	if (not(resultsTile == expectedTile)) { return stage; }

	stage += 1; // 4
	// Testing phys3 to tile3 at whole tile sizes

	expectedTile = {1, 2, 3};
	p3 = {65536, 131072, 196608};

	resultsTile = p3.to_tile3();
	if( not(resultsTile == expectedTile)) { return stage; }

	stage += 1; // 5
	// Testing phys3 to tile3 above whole tile sizes

	expectedTile = {1, 2, 3};
	p3 = {85641, 171072, 205465};

	resultsTile = p3.to_tile3();
	if( not(resultsTile == expectedTile)) { return stage; }

	stage += 1; // 6
	// Testing phys3 get_fraction just below whole tile size

	phys3_delta expectedP3D{65535, 65535, 65535};
	p3 = {65535, 131071, 196607};

	phys3_delta resultsP3D = p3.get_fraction();
	if( not(resultsP3D == expectedP3D)) { return stage; }

	stage += 1; // 7
	// Testing phys3 get_fraction at whole tile size

	expectedP3D = {0, 0, 0};
	p3 = {65536, 131072, 196608};

	resultsP3D = p3.get_fraction();
	if( not(resultsP3D == expectedP3D)) { return stage; }
	
	stage += 1; // 7
	// Testing phys3 get_fraction above whole tile size

	expectedP3D = {1, 1, 1};
	p3 = {65537, 131073, 196609};

	resultsP3D = p3.get_fraction();
	if( not(resultsP3D == expectedP3D)) { return stage; }

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
	camgame_delta expectedCD{48, 24};
	phys3_delta p3D{65536, 0, 0};
	
	camgame_delta resultsCD = p3D.to_camgame();
	if( not(resultsCD == expectedCD)) { return stage; }

	stage += 1; // 1
	// Testing that se has the same effect on x and negative on y

	expectedCD = {48, -24};
	p3D = {0, 65536, 0};

	resultsCD = p3D.to_camgame();
	if( not(resultsCD == expectedCD)) { return stage; }

	stage += 1; // 2
	// Testing that up effects y only

	expectedCD = {0, 24};
	p3D = {0, 0, 65536};

	resultsCD = p3D.to_camgame();
	if( not(resultsCD == expectedCD)) { return stage; }

	stage += 1; // 3
	// Testing ne and se add on x and cancel on y

	expectedCD = {96, 0};
	p3D = {65536, 65536, 0};

	resultsCD = p3D.to_camgame();
	if( not(resultsCD == expectedCD)) { return stage; }

	stage += 1; // 4
	// Testing phys3_delta to phys2_delta
	
	phys2_delta expectedP2D{10, 10};
	p3D = {10, 10, 10};

	phys2_delta resultsP2D = p3D.to_phys2();
	if( not(resultsP2D == expectedP2D)) { return stage; }

	return -1;
}

/**
 * This function tests the methods of tile3.
 */
int tile3_0() {
	int stage = 0;
	// Tests up portion getting cut off

	tile expectedT2{10, 10};
	tile3 t3{10, 10, 10};

	tile resultsT2 = t3.to_tile();
	if( not(resultsT2 == expectedT2)) { return stage; }

	stage += 1; // 1
	// Tests tile3 to phys3 with default arg

	phys3 expectedP3{32768, 32768, 0};
	t3 = {0, 0, 0};

	phys3 resultsP3 = t3.to_phys3();
	if( not(resultsP3 == expectedP3)) { return stage; }

	stage += 1; // 2
	// Tests tile3 to phys3 with 0's for arg

	expectedP3 = {0, 0, 0};

	resultsP3 = t3.to_phys3({0, 0 ,0});
	if( not(resultsP3 == expectedP3)) { return stage; }

	stage += 1; // 3
	// Test tile3 to phys3 with args

	expectedP3 = {65536, 131072, 196608};
	t3 = {1, 2, 3};

	resultsP3 = t3.to_phys3({0, 0, 0});
	if( not(resultsP3 == expectedP3)) { return stage; }

	return -1;
}

/**
 * This function tests the methods for camgame.
 */
int camgame_0() {
	int stage = 0;
	// Test camgame to phys3 with default arg and 0,0 camgame
	// to show we get the engines camgame_phys back

	Engine &e = Engine::get();
	phys3 expectedP3 = e.camgame_phys;
	camgame c{0, 0};

	phys3 resultsP3 = c.to_phys3();
	if( not(resultsP3 == expectedP3)) { return stage; }
	
	stage += 1; // 1
	// Test camgame to phys3 with default arg and x=1 for camagame

	// This is the base camgame_phys position {587094, 860159, 0}
	// plus ((1 pixel / 48(px per tile)) * 65536 phys per tile ) / 2
	// for both the ne and se components since we only added x.
	// Also this get rounded down so we end up adding 682 to ne and se
	expectedP3 = {587776, 860841, 0};
	c = {1, 0};

	resultsP3 = c.to_phys3();
	if( not(resultsP3 == expectedP3)) { return stage; }

	stage += 1; // 2
	// Test camgame to phys3 with default arg and y=1 for camagame

	// This is the base camgame_phys position {587094, 860159, 0}
	// plus ((1 pixel / 24(px per tile)) * 65536 phys per tile ) / 2
	// this add to ne and subracted frim se
	// Also this get rounded down so we end up adding 1365 to ne and se
	expectedP3 = {588459, 858794, 0};
	c = {0, 1};
	
	resultsP3 = c.to_phys3();
	if( not(resultsP3 == expectedP3)) { return stage; }

	stage += 1; // 3
	// Test camgame to phys3 with a pased arg

	// Here we start with the same ne and se ad before but we add/subract
	// the passed arg divided by 2. The passed arg is also applied directly
	// to the up component
	expectedP3 = {588460, 858793, 2};

	resultsP3 = c.to_phys3(2);
	if( not(resultsP3 == expectedP3)) { return stage; }

	stage += 1; // 4
	// Test camgame to window, camgame at 0,0

	// The default window is 800x600 so our window offset is 400,300
	window expectedW{400, 300};
	c = {0, 0};

	window resultsW = c.to_window();
	if( not(resultsW == expectedW)) { return stage; }

	stage += 1; // 5
	// Test camgame to window, camgame at 100, 100

	expectedW = {500, 200};
	c = {100, 100};

	resultsW = c.to_window();
	if( not(resultsW == expectedW)) { return stage; }

	return -1;
}

/**
 * This function tests the methods of camgame_delta.
 */
int camgame_delta_0() {
	int stage = 0;
	// Test camgame_delta to phys3_delta with default arg
	phys3_delta expectedP3D{0, 0, 0};
	camgame_delta cD{0, 0};

	phys3_delta resultsP3D = cD.to_phys3();
	if( not(resultsP3D == expectedP3D)) { return stage; }
	
	stage += 1; // 1
	// Test camgame_delta to phys_delta with default arg and x = 1

	// This is ((1 pixel / 48(px per tile)) * 65536 phys per tile ) / 2
	// for both the ne and se components since we only added x.
	// Also this get rounded down so we end up adding 682 to ne and se
	expectedP3D = {682, 682, 0};
	cD = {1, 0};

	resultsP3D = cD.to_phys3();
	if( not(resultsP3D == expectedP3D)) { return stage; }

	stage += 1; // 2
	// Test camgame_delta to phys3_delta with default arg and y = 1

	// This is ((1 pixel / 24(px per tile)) * 65536 phys per tile ) / 2
	// this add to ne and subracted frim se
	// Also this get rounded down so we end up adding 1365 to ne and se
	expectedP3D = {1365, -1365, 0};
	cD = {0, 1};

	resultsP3D = cD.to_phys3();
	if( not(resultsP3D == expectedP3D)) { return stage; }

	stage += 1; // 3
	// Test camgame_delta to phys3_delta with arg and y = 1

	// Here we start with the same ne and se ad before but we add/subract
	// the passed arg divided by 2. The passed arg is also applied directly
	// to the up component
	expectedP3D = {1366, -1366, 2};
	
	resultsP3D = cD.to_phys3(2);
	if( not(resultsP3D == expectedP3D)) { return stage; }

	stage += 1; // 4
	// Test camgame_delta to window_delta

	window_delta expectedWD{0, 0};
	cD = {0, 0};

	window_delta resultsWD = cD.to_window();
	if( not(resultsWD == expectedWD)) { return stage; }

	stage += 1; // 5
	// Test camgame_delta to window_delta, show y flips
	
	expectedWD = {100, -100};
	cD = {100, 100};

	resultsWD = cD.to_window();
	if( not(resultsWD == expectedWD)) { return stage; }

	return -1;
}

/**
 * This function tests the methods of camhud.
 */
int camhud_0() {
	int stage = 0;
	// Test camhud to window

	// The center is the one place camhud and window are the same
	window expectedW{400, 300};
	camhud c{400, 300};

	window resultsW = c.to_window();
	//log::err("%d, %d", resultsW.x, resultsW.y);
	//throw;
	if( not(resultsW == expectedW)) { return stage; }
	
	stage += 1; // 1
	// Test camhud to window
	
	// camhud is always the same x as window but the height of the
	// screen below window
	expectedW = {0, 600};
	c = {0, 0};

	resultsW = c.to_window();
	if( not(resultsW == expectedW)) { return stage; }

	stage += 1; // 2
	// Test camhud to term
	
	// TODO The expected results for this was created by just seeing
	// What the results acually were. Need to investigate the bottomleft
	// and top right of console as a toptight that is the camhud type
	// is negative in its y and thus would be off the screen

	// Since we are not running from the main engine, get the cwd
	char currentPath[128];
	char* stop;
	getcwd(currentPath, sizeof(currentPath));

	// Cut off the un-needed stuff
	stop = strstr(currentPath, ".bin");
  stop--;
	*stop = 0;

	// Add the correct assets dir
	util::Dir assetDir(currentPath);
	assetDir = assetDir.append("assets/converted");

	auto termcolors = util::read_csv_file<gamedata::palette_color>(assetDir.join("termcolors.docx"));
	console::Console con(termcolors);

	window size{80*con.charsize.x, 25*con.charsize.y};
	con.on_resize(size);

	stage += 1; //  3

	term expectedT{1, -54};
	c = {12, 6};
	
	term resultsT = c.to_term(&con);
	if( not(resultsT == expectedT)) { return stage; }

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

	window_delta expectedWD{100, -100};
	camhud_delta cD{100, 100};

	window_delta resultsWD = cD.to_window();
	if( not(resultsWD == expectedWD)) { return stage; }
	

	return -1;
}

/**
 * This function tests the methods of term
 */
int term_0() {
	int stage = 0;

	// Since we are not running from the main engine, get the cwd
	char currentPath[128];
	char* stop;
	getcwd(currentPath, sizeof(currentPath));

	// Cut off the un-needed stuff
	stop = strstr(currentPath, ".bin");
  stop--;
	*stop = 0;

	// Add the correct assets dir
	util::Dir assetDir(currentPath);
	assetDir = assetDir.append("assets/converted");

	auto termcolors = util::read_csv_file<gamedata::palette_color>(assetDir.join("termcolors.docx"));
	console::Console con(termcolors);

	stage += 1; // 1

	camhud expectedC{40, -84};
	term t{5, 5};

	camhud resultsC = t.to_camhud(&con);
	if( not(resultsC == expectedC)) { return stage; }

	return -1;
}

/**
 * Top level function for coord testing.
 */
void coord() {
	int ret;
	const char *testname;

	// Some tests below need the engine to be initialized
	util::Dir dataDir{"assets"};
	Engine::create( &dataDir ,"testing openage");
	
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


	// Cleanup engine
	Engine::destroy();
	return;

out:
	// Cleanup engine
	Engine::destroy();

	log::err("%s failed at stage %d", testname, ret);
	throw "failed coord tests";
}

} // namespace tests
} // namespace coord
} // namespace openage
