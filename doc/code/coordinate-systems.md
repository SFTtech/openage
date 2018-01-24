Comprehensive list of all coordinate systems
============================================

Physical Coordinates
--------------------

A coordinate in such a system describes positions in the game world.

Game world positions are stored in NE (northeast), SE (southeast) and UP (elevation).


### `phys2/phys3`

Physics coordinates
Orthonormal 2D/3D (NE, SE)/(NE, SE, UP), in tu/teu
Fixed-point integer with a 16-bit fractional part.

Origin is in the west corner of tile `(0, 0)` (at sea level),
and the west corner of chunk `(0, 0)`.
Positive in NE, SE, UP directions.

A unit at `(0.5, 0.5)` is at the center of tile `(0, 0)`,
with on-tile offset `(0.5, 0.5)`.
A unit at `(8, 8)` is at the center of chunk `(0, 0)`.

 * NE: north-east, in tu (terrain length unit)
 * SE: south-east, in tu (terrain length unit)
 * UP: guess what, in teu (terrain elevation unit)

We believe that `1 teu = 1 tu / sqrt(8)` (from aspect ration calculations).
The actual relation is pretty irrelevant, though.

Given in these systems:
 * Camera position (where does the cam look at?)
 * Unit positions
 * Projectiles


### `tile/tile3`

Tile coordinates.
Orthonormal 2D/3D (NE, SE)/(NE, SE, UP), integer, in tu/teu

Identical to phys2/phys3 systems, but uses integers

The center of a tile has phys coordinates `(0.5, 0.5)`.

Given in these systems:
 * Terrain tiles
 * Buildings


### `chunk`

Chunk coordinates.
Orthonormal 2D (NE, SE), integer, in 16tu/16teu

Similar to tile system, but describes chunks of `16x16` tiles

Given in these systems:
 * Chunks


Pixel Coordinates
-----------------

Coordinates in these systems designate a position on the screen plane.


### `viewport`

Viewport coordinates. Used for rendering.
Orthonormal 2D `(x, y)`, integer, in pixels

Origin is bottom left corner of viewport, positive in right and up directions

Given in this system:
 * Renderer OpenGL viewport

** For display, all other coordinates are converted to `viewport` eventually. **


### `input`

Coordinates used for input events.
Orthonormal 2D `(x, y)`, integer, in pixels

Origin is the top left corner of the viewport, positive in right and down directions


### `camgame`

Game camera coordinates.
Orthonormal 2D `(x, y)`, integer, in pixels

Origin is center of viewport, positive in right and up directions


### `camhud`

Game camera coordinates.
Orthonormal 2D `(x, y)`, integer, in pixels

Origin is bottom left corner of viewport, positive in right and up directions


### `term`

Terminal character coordinates.
Orthonormal 2D `(x, y)`, integer, in characters

Origin is top left corner of console area, positive in right and down directions


Conversions
===========

### For rendering

Depending on whether objects are rendered as part of hud or by the game
camera, their coordinates are transformed to `camgame` or `camhud`,
then to `viewport` coordinates which are then drawn on screen.


### For input processing

Again depending on whether inputs were aimed at an HUD object or an
entity rendered by the game camera, they (i.e. mouse clicks) are
transformed from input to camgame or camhud.
camgame coordinates can then further be converted to phys coordinates,
for mapping the inputs to physics objects.

Idea: HUD drawing code will register HUD objects with the engine
the engine then memorizes the areas where HUD objects are drawn,
and checks whether the mouse click lies within one of the rectangles


### Possible conversions

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
viewport <----------> camhud <------> term
  |
  |
input

```

### Translation details

The translations make use of current coordinate state (e.g. scroll position,
stored in `CoordManager`) and additional infos to solve
degrees of freedom (see [below](#Parameters)).


* `input` -> `viewport`: just flip the y axis
* `camgame` -> `viewport`: both are 'on-screen' coordinates, the difference is a linear offset
                         of their `(0, 0)` point and the camera zoom
* `camhud` -> `viewport`: linear offset of their `(0, 0)`
* `phys3` -> `camgame`: add the camera scroll position (given in `phys3`), then perform spatial transform
* The other translations should be intuitive


### Parameters

These parameters are needed to solve remaining degrees of freedom
when translating a coordinate of one system to another.

* `chunk` -> `tile`: needs tile-on-chunk coordinates
* `tile` -> `phys2`: needs position-on-tile coordinates
* `phys2` -> `phys3`: needs z component ()
* `tile` -> `tile3`: needs z component (elevation)
* `camgame` -> `phys3`: needs z component (elevation above terrain.
  reason: intersection between ray through camera and the terrain,
  e.g. for 'move here' command on a hill)
