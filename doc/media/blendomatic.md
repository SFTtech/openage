Blendomatic
===========

or: how to merge terrain tile edges.

why do we need that?
--------------------

`terrain.drs` stores all the terrains as slp files, see [doc/media/terrain.md](./terrain.md) for that.

Placing these tiles beside each other works fine, if you use only one terrain (e.g. grass).

If two different terrains are next to each other, they need to be blended. Water and land will be blended to water and shore, etc etc.

This procedure allows merging any terrain type with any other.


Blending procedure
------------------

Blending terrain tiles works by using alpha masks.
To create a "transition tile", a base tile and an alpha-masked overlay tile are merged.
The transition tile is a mixture of the two adjacent terrain tiles,
where one of them gets drawn over partially by the neighbor.

Each terrain type has a priority level (see table at the bottom).

The masked higher level tile is copied on top of the lower level tile.

This means that parts of the highlevel tile are visible in the original area
of the lower level tile, creating a smooth transition.

Example:

    ######@@@@@@
    ######@@@@@@
    ######@@@@@@
    ######@@@@@@

let `#` be terrain with priority 8, and `@` has priority 42.

`@` has a higher priority than `#`, meaning that `@` is alpha masked before copying.

```
Mask:    Tile:

000111   @@@@@@             @@@
001111   @@@@@@  masked    @@@@
000111   @@@@@@   ===>      @@@
000001   @@@@@@               @
```

(note that the mask actually has byte values for alpha blending, not just binary "draw" and "hide")

For the real blending masks, see below in section "blending directions".


The alpha-masked `@`-terrain is then drawn at the neighbor's position, on top of it.

    ###@@@@@@@@@
    ##@@@@@@@@@@
    ###@@@@@@@@@
    #####@@@@@@@

You can see that this creates a transition from the two textures,
where higher prioritized terrains "flood" on their neighbor tile.

If you imagine `@` as water, and `#` as shore, it's literally flooding...


Which alpha mask is determined by two conditions:

1. the tile positioning
    * what direction will the transition be?
1. the blending mode
    * how does the transition look like?
    * associated to terrain class -> ice needs other edges than shore
    * which mode is used does not depend on the terrain priority,
       it depends on the meeting classes.
       e.g. when blending with ice,
       the ice blend mode is applied to the other blending terrain.
    * existing blending modes (mask shapes):
      * mode 0: rough transition, full, used for dirt, grass, ...
      * mode 1: same as mode 0
      * mode 2: smooth transition, full length
      * mode 3: smooth transition, short
      * mode 4: rough hard edges, spraylike
      * mode 5: sharp edges
      * mode 6: same as mode 4
      * mode 7: same as mode 0
      * mode 8: same as mode 0
      * => you can see that we actually have 5 blending modes.
    * reason likely for that redundancy:
      * blending mode contains more information than it looks at first.
      * the highest blendmode_id gets selected as mask.
      * explains the double modes: same mask needed,
        but other priority requested.
    * determine the mask for a tile-tile transition:

      tile type | stored mode
      ----------|------------
      grass     | 0
      farm      | 1
      beach     | 2
      water     | 3
      shallows  | 4
      snow road | 5
      ice       | 6
      snow      | 7
      unknown   | 8

      The mode is then used as index into a lookup table:
      Current tile mode selects the row, neighbor mode the column.

      ``` c
      char blend_mask_lookup[8][8] = {
          { 2, 3, 2, 1, 1, 6, 5, 4 },
          { 3, 3, 3, 1, 1, 6, 5, 4 },
          { 2, 3, 2, 1, 1, 6, 1, 4 },
          { 1, 1, 1, 0, 7, 6, 5, 4 },
          { 1, 1, 1, 7, 7, 6, 5, 4 },
          { 6, 6, 6, 6, 6, 6, 5, 4 },
          { 5, 5, 1, 5, 5, 5, 5, 4 },
          { 4, 3, 4, 4, 4, 4, 4, 4 }
      };
      ```


blending directions
-------------------

non-blended tile:

          #
        #####
      #########
    #############
      #########
        #####
          #


blendomatic stores the following alpha masks:

id is the tile number for the 31 tiles within one blending mode.

`0` means: keep the base tile pixel, don't overdraw (mask 0)
`#` means: use this pixel from the dominant neighbor to draw over the base tile (mask 1)

the id's 0..15 describe only 4 directions, but have 16 tiles.
these were created to avoid the obviously repeating pattern.
method to choose one of the 4:
-> use lower 2 bit of tile destination x or y coordinates.

    id: 0..3           4..7           8..11          12..15
      lower right   upper right    lower left     upper left

          0              #              0              #
        00000          0####          00000          ####0
      000000000      00000####      000000000      ####00000
    000000000####  000000000####  ####000000000  ####000000000
      00000####      000000000      ####00000      000000000
        0####          00000          ####0          00000
          #              0              #              0

    id:  16             17             18             19
        right          down            up            left

          0              0              #              0
        00000          00000          #####          00000
      00000000#      000000000      000###000      #00000000
    000000000####  0000000000000  0000000000000  ####000000000
      00000000#      000###000      000000000      #00000000
        00000          #####          00000          00000
          0              #              0              0

    id:  20             21             22             23             24             25
      upperright     upperleft      onlyright      onlydown        onlyup        onlyleft
      lowerleft      lowerright

          #              #              #              #              0              #
        0####          ####0          ####0          #####          00000          0####
      00000####      ####00000      ######000      #########      000000000      000######
    ####00000####  ####00000####  ########00000  #############  #############  00000########
      ####00000      00000####      ######000      000000000      #########      000######
        ####0          0####          ####0          00000          #####          0####
          #              #              #              0              #              #

    id:  26             27             28             29             30
        keep           keep           keep           keep           all
       upperleft     upperright     lowerright     lowerleft

          #              #              #              #              #
        0####          ####0          #####          #####          #####
      00000####      ####00000      ####0####      ####0####      ####0####
    ####00000####  ####00000####  ####00000####  ####00000####  ####00000####
      ####0####      ####0####      ####00000      00000####      ####0####
        #####          #####          ####0          0####          #####
          #              #              #              #              #


these 31 tiles are used to alphamask all possible terrain junctions.


Blending algorithm
------------------

`@` = tile to draw next

        0
      7   1      => 8 neighbours that have influence on
    6   @   2         the mask id selection.
      5   3
        4

Algorithm:
```python
for @ in alltiles:

    #draw the base tile:
    @.draw()

    #storage for influences by neighbor tiles
    influence = dict()

    #first step: gather information about possible influences
    #look at every neighbor tile for that
    for i in [0..7]:

        #neighbor only interesting if it's a different terrain than @
        if i.terrain_type != @.terraintype:

            #adjacent tile will draw onto @, as it's priority is higher
            #else, ignore this neighbor
            if i.priority > @.priority:

                if i.is_diagonal_influence:
                    #get the ids of the adjacent neighbors of the diagonal
                    #influence:
                    i_neighbors = map(lambda x: x % 8, [i - 1, i + 1])

                    if any of i_neighbors have influence:
                        #don't apply diagonal influence i, as any of its
                        #neighbors already influences the tile.
                        continue

                #as tile i has influence for this priority
                # => bit i is set to 1 by 2^i (== 1 << i)
                #each priority is drawn seperately later.
                influence[i.terrain_id] |= 2**i

    #sort influences by priority, so that higher priorities get drawn last.
    influence = sorted(influence, by=influence.priority)

    #now: we got all influences, grouped by terrain priority.
    #for each of these influences, we continue finding the blendomatic mask
    # and apply it to the neighbors texture,
    # then draw the masked tile on top of the base (@) tile.

    #the terrain_id has influences coming from directions 'binf',
    #so we can select directional masks for that terrain.
    for terrain_id, binf in influence.items():

        #there is exactly one adjacent mask id for all combinations
        adjacent_mask_id  = []
        diagonal_mask_ids = []

        #find mask id by influencing neighbor tiles
        #                           neighbor id: 76543210
        adjacent_mask_id = [0 .. 3] if binf == 0b00001000
                   .add()  [4 .. 7] if binf == 0b00000010
                           [8 ..11] if binf == 0b00100000
                           [12..15] if binf == 0b10000000
                           20       if binf == 0b00100010
                           21       if binf == 0b10001000
                           22       if binf == 0b10100000
                           23       if binf == 0b10000010
                           24       if binf == 0b00101000
                           25       if binf == 0b00001010
                           26       if binf == 0b00101010
                           27       if binf == 0b10101000
                           28       if binf == 0b10100010
                           29       if binf == 0b10001010
                           30       if binf == 0b10101010

        diagonal_mask_id.add(16)    if binf  & 0b00000100 > 0
                             17     if binf  & 0b00010000 > 0
                             18     if binf  & 0b00000001 > 0
                             19     if binf  & 0b01000000 > 0


        #which of the 9 blending modes to use?
        #the selected blending mode is depending on which tiles meet.
        # (water->shore != water->ice)
        # terrain class => Land, Farm, Beach, Road, Water, Ice, ..
        #this means to determine:
        # use i.blendmode or @.blendmode
        #  when i is drawn onto @ later.
        blendmode    = get_blending_mode(priority, @)
        neighbortile = get_terrain_by_priority(priority)

        #all masks to draw: one adjacent mask xor 1 to 4 diagonal masks
        draw_masks = adjacent_mask_id + diagonal_mask_ids

        for maskid in draw_masks:
            #do the tile blending:
            #mask away pixels by applying the combined mask
            maskdata = mask[blendmode][maskid]

            #draw the masked terrain piece on top of our base (@) tile
            overlay_data = apply_mask(neighbortile.data, maskdata)
            overlay_data.draw()

```

The described alpha masks, are stored in `blendomatic.dat`:

Blendomatic.dat file format
---------------------------

This file contains the alphamasks that are applied per-tile.
This results in masking away areas, so a transition between the
partially-missing top tile, and the underlying base tile is possible.

```cpp
struct {
	struct {
		unsigned int nr_blending_modes;              // normally 9
		unsigned int nr_tiles;                       // normally 31
	} blendomatic_header;

	struct {
		unsigned int  tile_size;                     // normally 2353
		unsigned char tile_flags[nr_tiles];

		struct {
			uint8_t alpha_bitmask[tile_size / 8];    // with tile_size pixels, use the data bitwise.
		} tile_bitmasks[32];                         // why 32? maybe nr_tiles + 1?

		struct {
			uint8_t alpha_bytemap[tile_size];        // 7-bit alpha value pixels
		} tile_bytemasks[nr_tiles];
	} blending_modes[nr_blending_modes]
} blendomatic.dat;
```

The `alpha_bytemap` array contains values to be drawn as `*`
We have to add the spaces (.) ourself.

With the default `tile_size`, we end up drawing the rhombus with 49 rows.

The resulting tile is then used to overlay on a regular terrain tile, so that parts are alphamasked.

`alpha_bitmasks` selects which bytes from each tile are used,
the `alpha_bytemap` determines how much to blend.

This is our drawing goal (of course a bit bigger):

    ....*....
    ..*****..
    *********
    ..*****..
    ....*....

Each `*` is a pixel, with a 7 or 1 bit value, which just is an alpha threshold.

for `alpha_bytemap`:
--> 128 == when masking the dominant texture, draw the pixel fully opaque.
-->   0 == keep the base tile pixel here.

for `alpha_bitmasks`:
-->   1 == this pixel will be overdrawn by dominant neighbor.
-->   0 == keep the base tile pixel.
