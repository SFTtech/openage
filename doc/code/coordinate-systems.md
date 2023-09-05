Comprehensive list of all coordinate systems
============================================

Physical Coordinates
--------------------

A coordinate in such a system describes positions in the 3D space of the game world and rendering scene.

Positions are stored in NE (northeast), SE (southeast) and UP (elevation).

Units for distances are terrain units (tu) and terrain elevation units (teu).


### `phys2/phys3`

Physics coordinates in the game world.

Used in these systems:
* Core engine
* Gamestate
* Game entity (unit/building/projectile/...) positions

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


### `scene2/scene3`

Same as `phys2/phys3` but for usage in the renderer. All `phys2/phys3`
passed to the renderer are eventually converted to `scene2/scene3`.

Used in these systems:
* World renderer (sprite renderer)
* Terrain renderer
* Camera positioning

`scene2/scene3` types can be converted to Eigen vectors that are used for
rendering operations, e.g. creating uniforms, matrix transformations,
vertex calculations, etc.

Eigen vectors are created from `scene3` coordinates using this transformation:

`Eigen(x, y, z) = (SE, UP / sqrt(8), -NE)`

This will ensure that objects at coordinates are correctly displayed when using
the camera (i.e. the *north east* of a coordinate will also be displayed in the
*north east* when using the renderer camera).

UP is divided by `sqrt(8)` to mimic AoE2 terrain heights (from aspect ration calculations),
This means that a coordinate elevation of `1 teu` is displayed at height `1 / sqrt(8)` in
the 3D scene. The actual relation is pretty irrelevant though and could be adjusted
with other values.

Eigen vectors created from `scene2` coordinates have a fixed `y` value of
`0.0f` but are otherwise the same:

`Eigen(x, y, z) = (SE, 0.0, -NE)`


### `tile/tile3`

Tile coordinates.
Orthonormal 2D/3D (NE, SE)/(NE, SE, UP), integer, in tu/teu

Identical to phys2/phys3 systems, but uses integers

The center of a tile has phys coordinates `(0.5, 0.5)`.

Used in these systems:
 * Terrain
 * Buildings


### `chunk`

Chunk coordinates.
Orthonormal 2D (NE, SE), integer, in 16tu/16teu

Similar to tile system, but describes chunks of `16x16` tiles

Used in these systems:
 * Chunks


Pixel Coordinates
-----------------

Coordinates in these systems designate a position on the screen plane.

### `input`

Coordinates used for input events.
Orthonormal 2D `(x, y)`, integer, in pixels

Origin is the top left corner of the viewport, positive in right and down directions

Used in these systems:
* Input manager


### `viewport`

Viewport coordinates. Used for rendering generic objects in the viewport.
Orthonormal 2D `(x, y)`, integer, in pixels

Origin is bottom left corner of viewport, positive in right and up directions

Used in these systems:
* Debugging


### `camhud`

Game camera coordinates.
Orthonormal 2D `(x, y)`, integer, in pixels

Origin is bottom left corner of viewport, positive in right and up directions

Used in these systems:
* HUD drawing


### `term`

Terminal character coordinates.
Orthonormal 2D `(x, y)`, integer, in characters

Origin is top left corner of console area, positive in right and down directions

Used in these systems:
* Console drawing


### `camgame`

**Deprecated:** *replaced by `scene2/scene3` and new camera system*

Game camera coordinates.
Orthonormal 2D `(x, y)`, integer, in pixels

Origin is center of viewport, positive in right and up directions


Conversions
===========

### For rendering

Depending on whether objects are inside the game world, rendered as part of hud or
the GUI, different conversion take place.

* Object that are part of the game world are converted from `phys` to `scene` when passed to the renderer, and finally from `scene` to Eigen vectors when passed to OpenGL/Vulkan shaders
* HUD objects for ingame objects (e.g. HP bars for units) are transformed from `phys` to `camhud`

### For input processing

Again depending on whether inputs were aimed at an HUD object or an
entity rendered by the game camera, they (i.e. mouse clicks) are
transformed from input to `phys` or `camhud`.


### Possible conversions between coordinate types

```
           scene2 <----> phys2 <--------> tile <--------> chunk
              |           |                |
              |           |                |
              |           |                |
input ***> scene3 <----> phys3 <--------> tile3
   |          |
   |          |
   ------> viewport <--> term
              |
              |
           camhud
```

- Conversions from `input` to `scene3` are realized with raycasting using the camera direction and position. As the calculation is done with floating point values, the result is subject to float rounding errors and therefore **not precise**.

### Possible conversions to/from renderer types

```
scene2 ----> world space (Eigen::Vector2f)
   |
   |
scene3 ----> world space (Eigen::Vector3f)
   |
   |
viewport --> normalized device space (Eigen::Vector2f)
```

### Translation details

The translations make use of current coordinate state (e.g. scroll position,
stored in `CoordManager`) and additional infos to solve
degrees of freedom (see [below](#parameters)).


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
* `phys2` -> `phys3`: needs UP component ()
* `scene2` -> `scene3`: needs UP component ()
* `tile` -> `tile3`: needs UP component (elevation)
* `camgame` -> `phys3`: needs UP component (elevation above terrain.
  reason: intersection between ray through camera and the terrain,
  e.g. for 'move here' command on a hill)
