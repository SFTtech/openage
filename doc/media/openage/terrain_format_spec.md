# Terrain Format Specification

**Format Version:** 1

The openage terrain format is a plaintext configuration file format for defining
terrain textures. It tells the openage renderer which image resources it has to load
and how textures in these resources should be displayed.

All attributes start with a defined keyword followed by parameter values. Some
parameters have default values and are optional. The preferred file extension is
`.terrain`.


## Quick Reference

```
# This is a terrain configuration file
# comments start with # and are ignored

# file version
version 1

# source image definitions
imagefile <image_id> <filename>

# selection of blendomatic borders
blendmask <blend_id> <filename> <priority>

# How much the texture is scaled down at default zoom level
scalefactor <factor>

# layer and animation definitions
# layers defined first will be overdrawn by later definitions
layer <id> mode=off  position=<int>
layer <id> mode=loop position=<int> time_per_frame=<float> replay_delay=<float>

# definition of a terrain frames
# these are iterated for an animation
frame <layer_id> <image_id> <xpos> <ypos> <xsize> <ysize> blend_id=<int>
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

Version of the terrain format. Increments every time the syntax
or keywords of the format change.

Parameter  | Type   | Optional | Default value
-----------|--------|----------|--------------
version_no | int    | No       | -

**version_no**<br>
Version number of the format.


#### Example

```
version 1
```


### `imagefile`

Tells the renderer which image resources it has to load.
There has to be at least one `imagefile` defined.

Parameter | Type   | Optional | Default value
----------|--------|----------|--------------
image_id  | int    | No       | -
filename  | string | No       | -

**image_id**<br>
Reference ID for the resource used in this file. IDs should start at `0`.

**filename**<br>
Path to the image resource for the terrain texture on the filesystem.

There are two ways to specify a path: relative and absolute. Relative
paths are relative to the location of the terrain definition file. They
can only refer to image resources that are in the same modpack.

```
imagefile 0 "grass.png"       # grass.png is in the same folder as the terrain file
imagefile 1 "./grass.png"     # same as above, but more explicit
imagefile 2 "media/sand.png"  # sand.png is in the subfolder media/
```

Absolute paths start from the (virtual) modpack root (the path where all
modpacks are installed). They always begin with `/` followed by either
a modpack identifier or a shortened modpack alias enclosed by `{}`. For
information on modpack identifiers and aliases see the [modpack](modpacks.md#alias-and-identifier)
docs.

```
imagefile 0 "/{aoe2_base@openage}/sand.png"  # absolute path with modpack identifier
imagefile 1 "/{aoe2_base}/sand.png"          # absolute path with modpack alias
```

Absolute paths are the only way to reference image resources from other
modpacks.


#### Example

```
imagefile 0 "grass.png"
imagefile 1 "../../sand.png"
imagefile 2 "/{aoe2_base}/graphics/grass.png"
```


### `blendtable`

Defines a blending table that is used for looking up a blending mask. The
blending mechanism is described in more detail in the [blendomatic](/doc/media/blendomatic.md)
documentation. Blending masks/tables are **optional** and do not need to be used.

Parameter  | Type   | Optional | Default value
-----------|--------|----------|--------------
table_id   | int    | No       | -
filename   | string | No       | -
priority   | int    | No       | -
blend_mode | int    | No       | -

**table_id**<br>
Reference ID for the blending table used in this file. IDs should start at `0`.

**filename**<br>
Path to the blending table definition on the filesystem.

There are two ways to specify a path: relative and absolute. Relative
paths are relative to the location of the terrain definition file. They
can only refer to image resources that are in the same modpack.

```
blendmask 0 "blend0.bltable" 10 3        # blend0.bltable is in the same folder as the terrain file
blendmask 1 "./blend0.bltable" 10 3      # same as above, but more explicit
blendmask 2 "media/blend1.bltable" 10 3  # blend1.bltable is in the subfolder media/
```

Absolute paths start from the (virtual) modpack root (the path where all
modpacks are installed). They always begin with `/` followed by either
a modpack identifier or a shortened modpack alias enclosed by `{}`. For
information on modpack identifiers and aliases see the [TODO]() docs.

```
blendmask 0 "/{aoe2_base@openage}/blend0.bltable" 10 3  # absolute path with modpack identifier
blendmask 1 "/{aoe2_base}/blend0.bltable" 10 3         # absolute path with modpack alias
```

Absolute paths are the only way to reference image resources from other
modpacks.

**priority**<br>
Decides which blending table of the two adjacent terrain textures is selected.
The table referenced by the terrain with the highest priority value will be picked.
If two adjacent terrains have equal priority, the blending table of the terrain
with a lower x coordinate value is selected. If the x coordinate value is also
equal, the blending table of the terrain with the lowest y coordinate is selected.

```
# grass.terrain
...
blendmask 0 "blend0.bltable" 12 2
...

# sand.terrain
...
blendmask 0 "blend1.bltable" 10 3
...

-> grass.terrain's blending table has a higher blending priority.
   Its blending table definition is therefore used when grass.terrain
   and sand.terrain are adjacent to each other.
```

**blend_mode**<br>
Used for looking up the blending mask index in the blending table.


#### Example

```
blendmask 0 "blend0.bltable" 40 7
blendmask 1 "./blend3.bltable" 10 1
blendmask 2 "/{aoe2_base}/blend8.bltable" 90 1
```


### `scalefactor`

Defines a downscaling factor for the texture.

Parameter | Type  | Optional | Default value
----------|-------|----------|--------------
factor    | float | No       | -

**factor**<br>
Factor by which textures are scaled *down* at default zoom level.
This allows for high resolution textures to be displayed at an
arbitrary scale. Zooming in counteracts the downscaling. Thus,
this factor can be used for textures that should retain high
image quality with higher zoom levels.


#### Example

```
# Assume the texture image has a size of 100x100
scalefactor 1.0  # No scaling, 100x100 at default zoom
scalefactor 2.0  # 50x50 at default zoom; 100x100 at 2x zoom
scalefactor 4.0  # 25x25 at default zoom; 100x100 at 4x zoom

# Factors <1 result in upscaling
scalefactor 0.5  # 200x200 at default zoom; 100x100 at 2x zoom
```


### `layer`

Defines a layer for the rendered texture. Layers allow subtextures
to be delegated into the foreground or background of the final result.
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
layer 3 mode=loop position=15 time_per_frame=1200 replay_delay=200
```


### `frame`

Defines a frame in a texture animation. If the texture is not
supposed to be animated, only frame `0` has to be defined.
Textures displayed in the frame are taken from an image resource
defined by the `imagefile` attribute.

Parameter | Type  | Optional | Default value
----------|-------|----------|--------------
frame_idx | int   | No       | -
layer_id  | int   | No       | -
image_id  | int   | No       | -
xpos      | int   | No       | -
ypos      | int   | No       | -
xsize     | int   | No       | -
ysize     | int   | No       | -
blend_id  | int   | Yes      | -

**frame_idx**<br>
Index of the frame in the animation. The first usable index value
is `0`. Frames are played in order of their assigned indices. If
an index is skipped, the frame at this index will be empty.

**layer_id**<br>
ID of the layer on which the frame is drawn.

**image_id**<br>
ID of the image resource that is used as a source for the texture
displayed in this frame.

**xpos**<br>
Horizontal position of the texture inside the image resource. The
texture's position is the pixel in the upper left corner of the
texture (`(0,0)` from the texture's POV).

**ypos**<br>
Vertical position of the texture inside the image resource. The
texture's position is the pixel in the upper left corner of the
texture (`(0,0)` from the texture's POV).

**xsize**<br>
Width of the texture inside the image resource.

**ysize**<br>
Height of the texture inside the image resource.

**blend_id**<br>
ID of the blending mask used in this frame. If no ID is defined, the
renderer will always select the blending mask of the adjacent terrain
texture for blending.


#### Example

```
frame 0 1 0 0 0 200 200 blend_id=1
# frame_idx = 0  -> first frame in the animation
# layer_id  = 1  -> drawn on layer 1
# image_id  = 0  -> taken from image resource with ID 0
# blend_id  = 1  -> use blending mask 1
# Texture is located at (0,0) in image resource
# and has a size of (200,200).
```
