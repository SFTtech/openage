terrain documentation
=====================

This file describes the research done on terrain rendering.


Contents of terrain.drs
-----------------------

27 slp files for terrain data, each slp stores one terrain type with all tiles.

id | filename   | priority | tilecount | description
---|------------|---------:|-----------|------------
 0 | 015000.slp |       70 | 100 tiles | brown dirty earth
 1 | 015001.slp |      102 | 100 tiles | more dry grass
 2 | 015002.slp |      139 | 100 tiles | light water
 3 | 015004.slp |      155 |  36 tiles | full grown farm field
 4 | 015005.slp |      157 |  36 tiles | decaying farm field
 5 | 015006.slp |      101 | 100 tiles | dirt with little grass
 6 | 015007.slp |      106 | 100 tiles | dry grass
 7 | 015008.slp |       90 | 100 tiles | dark grass
 8 | 015009.slp |      100 | 100 tiles | normal grass
 9 | 015010.slp |       80 | 100 tiles | desert
10 | 015011.slp |       92 | 100 tiles | sandy dry grass
11 | 015014.slp |       60 | 100 tiles | swamp
12 | 015015.slp |      140 | 100 tiles | deep water
13 | 015016.slp |      141 | 100 tiles | ocean
14 | 015017.slp |      110 | 100 tiles | sandy sand
15 | 015018.slp |      122 | 100 tiles | ancient building fundaments
16 | 015019.slp |      123 | 100 tiles | dirt ancient building fundaments
17 | 015021.slp |      150 |   9 tiles | ready and empty farm field
18 | 015022.slp |      151 |   9 tiles | stage 0 plants on farm field
19 | 015023.slp |      152 |   9 tiles | stage 1 plants on farm field
20 | 015024.slp |       40 | 100 tiles | ice
21 | 015026.slp |      130 | 100 tiles | snow
22 | 015027.slp |      132 | 100 tiles | dirt with snow
23 | 015028.slp |      134 | 100 tiles | grass with snow
24 | 015029.slp |      136 | 100 tiles | grass with snow
25 | 015030.slp |      162 | 100 tiles | ancient building fundaments with snow
26 | 015031.slp |      120 | 100 tiles | grass ancient building fundaments


One terrain tile is a parallelogram with equal side lengths, and the same angles in edges on opposing sides. (This is also known as a rhombus.)

Tile image height = 97px
Tile image width  = 49px


Half-tile structure: (`#` is any pixel, `@` is the center pixel for the entire tile)

		                                                #
		                                              #####
		                                            #########
		                                          #############
		                                        #################
		                                      #####################
		                                    #########################
		                                  #############################
		                                #################################
		                              #####################################
		                            #########################################
		                          #############################################
		                        #################################################
		                      #####################################################
		                    #########################################################
		                  #############################################################
		                #################################################################
		              #####################################################################
		            #########################################################################
		          #############################################################################
		        #################################################################################
		      #####################################################################################
		    #########################################################################################
		  #############################################################################################
		################################################@################################################ <| 97 pixels, 25th row


Drawing smooth uniform terrain is simple:

`#` is a single terrain tile.
this illustration shows the coordinate grid we use for the tile drawing.

		         3
		       2   #
		     1   #   #
		x= 0   #   *   #
		     #   #   #   #
		y= 0   #   #   #
		     1   #   #
		       2   #
		         3


Tile `*` is at (2, 1).
The formula for selecting the correct terrain .SLP frame index at position (x, y):

```python
tc = sqrt(terraintilecount) #=10 for regular terrains
frame_id = (x % tc) + ((y % tc) * tc)
```


Terrain blending modes
----------------------

blending mode: see the [blendomatic docs](/doc/media/blendomatic.md) for
more information about terrain blending.

blending mode id correction:


stored mode | terrain type  | corrected mode
-----------:|---------------|---------------
0           | dirt, grass   | 1
1           | farms         | 3
2           | beach         | 2
3           | water         | 0
4           | shallows      | 1
5           | roads         | 4
6           | ice           | 5
7           | snow          | 6
8           | not assigned  | 4

Terrain elevations
------------------

Drawing and blending in flat elevation.

### Sloped tiles
generated by texture mapping the flat tiles and lighting them (in HSV color space)
blend RGB space, maybe blending them in HSV space could reduce the amount of grey.
lift up the vertices by half of their diamond height.



view_icm.dat
------------

"inverse color map"

=> get best color_palette entry
rgb to palette index transformation table with 10 variations of brightness

=> 10 color maps, 32x32x32 lookup table, 1 byte indices.
=> 10 32 kilobyte structures

brightness variation: use row 0..9

fetches the closest color index in the color_table for a given rgb (3 x 8 bit) value.
set each "axis" (the 32 bit position) to the upper 5 bits of r, g, b (shift >> 3)
select one row => coordinates in the table:

```python
offset = i * (32^3) + r * 32^2 + g * 32 + b
i == [0,9] = icm index
palette_index = byte_at(offset)
```

sloped terrain lighting brightness levels
1 ICM:  normal/unlit pixels (flat tiles)
9 ICMs: lighting
=> 4 step of darkening and 4 steps of brightening, 1 neutral.
one ICM maps to the 50500 palette
the other 9 adjust the pixel brightness.

generating the other ICMs:
lighting in HSV color space, modify S and V separately.
for each RGB value in the ICM, convert to HSV, multiply (clamped) S and V by modifiers
convert the resulting HSV color back to RGB.
look up this color by the standard ICM, store back palette indices for the modified light to new ICM.

lighting on sloped tiles:
sloped/elevation tiles are dynamically generated by texturemapping flat tile data onto the sloped format
filtermaps.dat does that
texture mapping operates on the RGB values |> weighted bilinear filtering of the source pixels
=> to convert the resultant R G B back to a palette_index ==> ICM lookup
multiple ICMs: pixel lighting step done in higher quality HSV color,
no performance impact: lighting data is precalculated.

ICMs may not contain system reserved colors at the beginning and end of the palette
=> some colors are not used/present in the ICMs


tileedge.dat / blkedge.dat format
---------------------------------

`blkedge.dat` contains bitmasks that are used for drawing edges for fog of war and unexplored regions.

```cpp
struct tile_edge {
	uint32_t elevation_offsets[17];
	struct {
		uint32_t tile_offsets[94];
		struct {
			struct {
				uint8 y;   // 0 <= y <= 72
				uint8 x0;  // 0 <= x0 <= 96
				uint8 x1;  // 0 <= x1 <= 96 && x0 <= x1
			} spans[n];    // repeat until y == 255
		} tiles[94];       // each starting at tile_offsets[i]
	} elevations[17];      // each starting at elevation_offsets[i]
}
```

blkedge.dat: replace 94 with 47, otherwise same format.

each 'spans' == pixels from (x0,y) to (x1,y) (inclusive) to set to 1;
=> 17*94 tiles, each 97x73 1-bit pixels



patternmasks.dat format
-----------------------

No idea what this data is for, also the format's not verified yet.
filesize = 164000 bytes = 2^5 * 5^3 * 41 bytes

164000 bytes / 16 = 10250 bytes

64 * 64 * 40 bytes = 163840 bytes, 160 bytes left then.
(64 * 64 + 4) * 40 bytes = 164000 => win.

```cpp
struct patternmasks {
	struct {
		uint_32 unknown;  //4 bytes: 0x00100000 => if uint32: 1048576
		char data[64*64];
	} mask[40];
}
```
