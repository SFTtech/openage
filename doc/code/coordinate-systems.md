Comprehensive list of all coordinate systems
============================================

phys2/phys3
-----------

Physics coordinates
orthonormal 2D/3D (NE, SE)/(NE, SE, UP), in tu/teu  
fixed-point integer with a 16-bit fractional part.

The origin is in the west corner of tile (0, 0) (at sea level),
and the west corner of chunk (0, 0)
positive in NE, SE, UP directions

A unit at (0.5, 0.5) stands at the center of tile (0, 0).  
A unit at (8, 8) stands at the center of chunk (0, 0)

 * NE: north-east, in tu (terrain length unit)
 * SE: south-east, in tu (terrain length unit)
 * UP: guess what, in teu (terrain elevation unit)

We believe that `1 teu = 1 tu / sqrt(8)` (from aspect ration calculations).
the actual relation is pretty irrelevant, though.

Given in these systems:
 * Camera
 * Units
 * Projectiles

tile/tile3
----------
Tile coordinates.
orthonormal 2D/3D (NE, SE)/(NE, SE, UP), integer, in tu/teu

It is Identical to phys2/phys3 systems, but uses integers.

given in these systems:
 * Terrain tiles
 * Buildings

chunk
-----
Chunk coordinates.
orthonormal 2D (NE, SE), integer, in 16tu/16teu

It is similar to the tile system, but describes chunks of 16x16 tiles.

Given in these systems:
 * Chunks

window
------
Window coordinates.
orthonormal 2D (x, y), integer, in pixels

The origin is the top left corner of window, positive in right and down directions.

given in this system:
 * SDL mouse
 * OpenGL viewport

camgame
-------
Game camera coordinates.
orthonormal 2D (x, y), integer, in pixels

The origin is the center of the window, positive in right and up directions

while in game rendering mode, the renderer accepts
coordinates of this type.

camhud
------
Game camera coordinates.
orthonormal 2D (x, y), integer, in pixels

The origin is the bottom left corner of window, positive in right and up directions

while in hud rendering mode, the renderer accepts
coordinates of this type.

term
----
Terminal character coordinates.
orthonormal 2D (x, y), integer, in characters

origin is top left corner of console area, positive in right and down directions


Conversions
===========

For rendering
-------------
Depending on whether objects are rendered as part of hud or by the game
camera, their coordinates are transformed to camgame or camhud.

For input processing
--------------------
Again depending on whether inputs were aimed at an HUD object or an
entity rendered by the game camera, they (i.e. mouse clicks) are
transformed to camgame or camhud.
camgame coordinates can then further be converted to phys coordinates,
for mapping the inputs to physics objects.

A possible idea: HUD drawing code will register HUD objects with the engine
the engine then memorizes the areas where HUD objects are drawn,
and checks whether the mouse click lies within one of the rectangles.

```

phys2 <--------> tile <--------> chunk
  |                |
  |                |
phys3 <--------> tile3
  |
  |
camgame
  |
  |
window <----------> camhud <------> term

```

 * chunk to tile: needs tile-on-chunk coordinates
 * tile-to-phys2: needs position-on-chunk coordinates
 * phys2 to phys3: needs z component
 * tile to tile3: needs z component
 * camgame to phys3: needs z component
