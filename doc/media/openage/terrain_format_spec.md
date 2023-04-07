# Terrain Format Specification

**Format Version:** 2

The openage terrain format is a plaintext configuration file format for defining
terrain textures. It tells the openage renderer which texture resources it has to load
and how these resources should be displayed.

All attributes start with a defined keyword followed by parameter values. Some
parameters have default values and are optional. The preferred file extension is
`.terrain`.


## Quick Reference

```
# This is a terrain configuration file
# comments start with # and are ignored

# file version
version 2

# texture file reference, relative to this file's location
texture <texture_id> <filename>

# selection of blending pattern
blendtable <table_id> <filename>

# the zoom level at which the animation is shown in full detail
# e.g. scalefactor 0.5 -> full detail at 200% zoom
scalefactor <factor>

# layer and animation definitions
# layers defined first will be overdrawn by later definitions
layer <layer_id> mode=off  position=<int>
layer <layer_id> mode=loop position=<int> time_per_frame=<float> replay_delay=<float>

# definition of a terrain frames
# these are iterated for an animation
frame <frame_idx> <layer_id> <texture_id> <subtex_id> priority=<int> blend_mode=<int>
```


## Data Type Formatting

| Type   | Example | Description                          |
| ------ | ------- | ------------------------------------ |
| int    | `5`     | Signed Integer                       |
| float  | `1.2`   | Float                                |
| string | `"bla"` | String of characters enclosed by `"` |
| token  | `off`   | Alphanumeric predefined keyword      |


## Attributes

### `version`

Version of the terrain format. Increments every time the syntax
or keywords of the format change.

| Parameter  | Type | Optional | Default value |
| ---------- | ---- | -------- | ------------- |
| version_no | int  | No       | -             |

**version_no**<br>
Version number of the format.


#### Example

```
version 2
```


### `texture`

Tells the renderer which texture resources it has to load.
There has to be at least one `texture` defined.

| Parameter  | Type   | Optional | Default value |
| ---------- | ------ | -------- | ------------- |
| texture_id | int    | No       | -             |
| filename   | string | No       | -             |

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


### `blendtable`

Defines a blending table that is used for looking up a blending pattern. The
blending mechanism is described in more detail in the [blendomatic](/doc/media/blendomatic.md)
documentation. The blending table and blending patterns are further defined
in the [blending table format](blendtable_format_spec.md) and
[blendmask format](blendmask_format_spec.md) specifications.

Blending patterns/tables are **optional** and do not need to be used.

| Parameter | Type   | Optional | Default value |
| --------- | ------ | -------- | ------------- |
| table_id  | int    | No       | -             |
| filename  | string | No       | -             |

**table_id**<br>
Reference ID for the blending table used in this file. IDs should start at `0`.

**filename**<br>
Path to the blending table definition on the filesystem. The different methods of
resource referencing are explained in the [file referencing](file_referencing.md)
docs.


#### Example

```
blendtable 0 "blend0.bltable" 40 7
blendtable 1 "./blend3.bltable" 10 1
blendtable 2 "/{aoe2_base}/blend8.bltable" 90 1
```


### `scalefactor`

Defines a downscaling factor for the texture.

| Parameter | Type  | Optional | Default value |
| --------- | ----- | -------- | ------------- |
| factor    | float | No       | -             |

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

Defines a layer for the rendered texture. Layers allow subtextures
to be delegated into the foreground or background of the final result.
All frames are assigned to a specific layer. At least one `layer`
has to be defined.

| Parameter      | Type  | Optional | Default value |
| -------------- | ----- | -------- | ------------- |
| layer_id       | int   | No       | -             |
| mode           | token | Yes      | `off`         |
| position       | int   | Yes      | `0`           |
| time_per_frame | float | Yes      | `1.0`         |
| replay_delay   | float | Yes      | `0.0`         |

**layer_id**<br>
Reference ID for the layer used in this file. IDs should start at `0`.

**mode**<br>
Tells the renderer how often the animation on this layer repeats.

| Mode   | Description                                                                               |
| ------ | ----------------------------------------------------------------------------------------- |
| `off`  | Layer is not animated. Only the first frame is shown.                                     |
| `loop` | Animation loops indefinitely. The renderer waits `replay_delay` seconds after every loop. |

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
layer 3 mode=loop position=15 time_per_frame=1200 replay_delay=200
```


### `frame`

Defines a frame in a texture animation. If the texture is not
supposed to be animated, only frame `0` has to be defined.
Textures displayed in the frame are taken from an image resource
defined by the `imagefile` attribute.

| Parameter  | Type | Optional | Default value |
| ---------- | ---- | -------- | ------------- |
| frame_idx  | int  | No       | -             |
| layer_id   | int  | No       | -             |
| texture_id | int  | No       | -             |
| subtex_id  | int  | No       | -             |
| priority   | int  | Yes      | -             |
| blend_mode | int  | Yes      | -             |

**frame_idx**<br>
Index of the frame in the animation. The first usable index value
is `0`. Frames are played in order of their assigned indices. If
an index is skipped, the frame at this index will be empty.

**texture_id**<br>
ID of the texture resource that contains the sutexture referenced by
`subtex_id`.

**subtex_id**<br>
ID of the subtexture from the referenced texture that is used as a source
for the terrain graphics.

**priority**<br>
Decides which [blending table](blendtable_format_spec.md) of the two
adjacent terrain textures is selected. The table referenced by the terrain
with the highest priority value will be picked.

```
# grass.terrain
...
frame 0 1 0 0 0 200 200 priority=5 blend_mode=0
...

# sand.terrain
...
frame 0 1 0 0 0 200 200 priority=1 blend_mode=0
...

-> grass.terrain's frame has a higher blending priority.
   therefore, its blending table definition is used when
   grass.terrain and sand.terrain are adjacent to each other.
```

If two adjacent terrains have equal priority, the blending table of the terrain
with a lower x coordinate value is selected. If the x coordinate value is also
equal, the blending table of the terrain with the lowest y coordinate is selected.

If no priority is defined, the renderer assigns the frame priority 0.

**blend_mode**<br>
Used for looking up the blending pattern index in the blending table. If no
blend mode is defined, then no blending pattern is used at all, even if
adjacent terrain textures define one.


#### Example

```
frame 0 1 0 0 0 200 200 priority=1 blend_mode=0
# frame_idx  = 0  -> first frame in the animation
# layer_id   = 1  -> drawn on layer 1
# image_id   = 0  -> taken from image resource with ID 0
# priority   = 1  -> this frame's blending table is selected with priority 1
# blend_mode = 0  -> use blend mode 0
# Texture is located at (0,0) in image resource
# and has a size of (200,200).
```
