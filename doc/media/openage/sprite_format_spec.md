# Sprite Format Specification

**Format Version:** 2

The openage sprite format is a plaintext configuration file format for defining 2D
animations. It tells the openage renderer which texture resources it has to load
and how subtextures in these resources should be displayed.

All attributes start with a defined keyword followed by parameter values. Some
parameters have default values and are optional. The preferred file extension is
`.sprite`.


## Quick Reference

```
# This is a sprite configuration file
# comments start with # and are ignored

# file version
version 2

# texture file reference, relative to this file's location
texture <texture_id> <filename>

# the zoom level at which the animation is shown in full detail
# e.g. scalefactor 0.5 -> full detail at 200% zoom
scalefactor <factor>

# layer definitions
# all layers will be drawn
layer <layer_id> mode=off  position=<int>
layer <layer_id> mode=once position=<int> time_per_frame=<float>
layer <layer_id> mode=loop position=<int> time_per_frame=<float> replay_delay=<float>

# define an angle where frames can be assigned to or mirror from an existing angle
angle <degree> mirror-from=<existing_angle>

# assign frames to their layers and angles.
# angle is the direction in degrees, etc.
frame <frame_idx> <angle> <layer_id> <image_id> <subtex_id>
```


## Data Type Formatting

Type     | Example | Description
---------|---------|---------
int      | `5`     | Signed Integer
float    | `1.2`   | Float
string   | `"bla"` | String of characters enclosed by `"`
token    | `off`   | Alphanumeric predefined keyword


## Attributes

### `version`

Version of the sprite format. Increments every time the syntax
or keywords of the format change.

Parameter  | Type   | Optional | Default value
-----------|--------|----------|--------------
version_no | int    | No       | -

**version_no**<br>
Version number of the format.

#### Example

```
version 2
```


### `texture`

Tells the renderer which texture resources it has to load.
There has to be at least one `texture` defined.

Parameter  | Type   | Optional | Default value
-----------|--------|----------|--------------
texture_id | int    | No       | -
filename   | string | No       | -

**texture_id**<br>
Reference ID for the resource used in this file. IDs should start at `0`.

**filename**<br>
Path to the texture resource on the filesystem. The file must be a [texture format file](texture_format_spec.md).
The different methods of resource referencing are explained in the [file referencing](file_referencing.md)
docs.


#### Example

```
texture 0 "idle.texture"
texture 1 "../../attack.texture"
texture 2 "/{aoe2_base}/graphics/attack.texture"
```


### `scalefactor`

Defines a downscaling factor for the animation's sprites.

Parameter | Type  | Optional | Default value
----------|-------|----------|--------------
factor    | float | No       | -

**factor**<br>
Factor by which sprite images are scaled at default zoom level.
This allows for high resolution sprites to be displayed at an
arbitrary scale. It can be used for sprites that should retain
high image quality with higher zoom levels.


#### Example

```
# Assume the sprite image has a size of 100x100
# Factors <1 result in downscaling
scalefactor 1.0   # No scaling, 100x100 at default zoom
scalefactor 0.5   # 50x50 at default zoom; 100x100 at 2x zoom
scalefactor 0.25  # 25x25 at default zoom; 100x100 at 4x zoom

# Factors >1 result in upscaling
scalefactor 2.0  # 200x200 at default zoom; 100x100 at 2x zoom
```

### `layer`

Defines a layer for the rendered sprite. Layers allow sprites
to be delegated into the foreground or background of the animation.
All frames are assigned to a specific layer. At least one `layer`
has to be defined.

Parameter      | Type  | Optional | Default value
---------------|-------|----------|--------------
layer_id       | int   | No       | -
mode           | token | Yes      | `off`
position       | int   | Yes      | `0`
time_per_frame | float | Yes      | `1.0`
replay_delay   | float | Yes      | `0.0`

**layer_id**<br>
Reference ID for the layer used in this file. IDs should start at `0`.

**mode**<br>
Tells the renderer how often the animation on this layer repeats.

Mode   | Description
-------|------------
`off`  | Layer is not animated. Only the first frame is shown.
`once` | Animation is played once. After that, the last animation frame is shown.
`loop` | Animation loops indefinitely. The renderer waits `replay_delay` seconds after every loop.

**position**<br>
Position of the layer in the final animation. Layers with a higher position
overdraw ones with a lower position.

**time_per_frame**<br>
Length of time (in milliseconds) that a frame is shown.

**replay_delay**<br>
How long the renderer waits (in milliseconds) until the next animation loop is started.


#### Example

```
layer 0
layer 1 mode=off  position=10
layer 2 mode=once position=20 time_per_frame=300
layer 3 mode=loop position=15 time_per_frame=1200 replay_delay=200
```


### `angle`

Specifies an angle that frames can get assigned to. The renderer
decides which angle to draw based on the angle of the game world
object the animation is attached to. At least one `angle` has to
be defined.

Parameter   | Type  | Optional | Default value
------------|-------|----------|--------------
degree      | int   | No       | -
mirror_from | int   | Yes      | -

**degree**<br>
Acts as the ID and center point for the angle. Has to be an integer between
`0` and `359`.

The angle with center point `degree = 0` is facing the camera, i.e. the game object
direction should be *towards* the camera. Subsequent angles are ordered *clockwise*, e.g.
`90` faces left from the camera's point of view, `180` faces up, and `270` faces right.

Frames at an angle are drawn until the angle of the game world object is
closer to another defined angle.  In other words, as long as the game world
objects's angle is between
`previous_angle_degree + (degree - previous_angle_degree) / 2`
and `degree + (next_angle_degree - degree) / 2`.

```
angle 0
angle 45  # Drawn when game world object's angle in interval [22.5,67.5]
angle 90
...
```

**mirror_from**<br>
Specifies another angle that this angle will use for drawing frames.
The frames are flipped vertically (= mirrored).


#### Example

```
angle 0
angle 90
angle 180
angle 270 mirror_from=90
```


### `frame`

Defines a frame in the animation. Frames have to be defined
for every angle individually. Sprites displayed in the frame
are taken from a texture resource (spritesheet) referenced by the
`imagefile` attribute.

Parameter  | Type  | Optional | Default value
-----------|-------|----------|--------------
frame_idx  | int   | No       | -
angle      | int   | No       | -
layer_id   | int   | No       | -
texture_id | int   | No       | -
subtex_id  | int   | No       | -

**frame_idx**<br>
Index of the frame in the animation for the specified `angle`. The
first usable index value is `0`. Frames are played in order of
their assigned indices. If an index is skipped, the frame at this
index will be empty.

**angle**<br>
ID of the angle at which the frame is displayed.

**layer_id**<br>
ID of the layer on which the frame is drawn.

**texture_id**<br>
ID of the texture resource that contains the sutexture referenced by
`subtex_id`.

**subtex_id**<br>
ID of the subtexture from the referenced texture that is used as a source
for the sprite displayed in this frame.


#### Example

```
frame 0 90 1 0 2
# frame_idx  = 0  -> first frame in the animation
# angle      = 90 -> attached to angle 90
# layer_id   = 1  -> drawn on layer 1
# texture_id = 0  -> taken from texture resource with ID 0
# subtex_id  = 2  -> uses subtexture with ID 2 as sprite
```
