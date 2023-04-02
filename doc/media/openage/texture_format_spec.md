# Texture Format Specification

**Format Version:** 1

The openage texture format is a plaintext configuration file format for defining 2D
textures and contained subtextures. It tells the openage renderer which image resource
it has to load and how the resource s partioned into subtextures.

All attributes start with a defined keyword followed by parameter values. Some
parameters may have default values and are optional. The preferred file extension is
`.texture`.

A `.texture` file should be placed next to the image file it references and have the same
filename stem.


## Quick Reference

```
# This is a texture configuration file
# comments start with # and are ignored

# file version
version 1

# image file reference, relative to this file's location
imagefile <filename>

# Image size
size <width> <height>

# Pixel format representation
pxformat <format> cbit=<token>

# Defines a subtexture inside the image resource
# Subtextures have x and y coordinates, width and height
# as well as anchor points.
subtex <xpos> <ypos> <xsize> <ysize> <xhotspot> <yhotspot>
```


## Data Type Formatting

Type     | Example | Description
---------|---------|---------
int      | `5`     | Signed Integer
string   | `"bla"` | String of characters enclosed by `"`
token    | `off`   | Alphanumeric predefined keyword


## Attributes

### `version`

Version of the texture format. Increments every time the syntax
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

Tells the renderer which image resource it has to load.
There has to be exactly one `imagefile` defined.

Parameter | Type   | Optional | Default value
----------|--------|----------|--------------
filename  | string | No       | -

**filename**<br>
Path to the image resource on the filesystem. The different methods of
resource referencing are explained in the [file referencing](file_referencing.md)
docs.


#### Example

```
imagefile "idle.png"
```


### `size`

Size of the image loaded from the file.

Parameter | Type   | Optional | Default value
----------|--------|----------|--------------
width     | int    | No       | -
height    | int    | No       | -

**width**<br>
Width of the image in pixels.

**height**<br>
Height of the image in pixels.


#### Example

```
size 800 600
```


### `pxformat`

Pixel format used for pixels in the image.

Parameter | Type   | Optional | Default value
----------|--------|----------|--------------
format    | token  | No       | -
cbit      | token  | Yes      | `false`

**format**<br>
Pixel format of the image.

Format    | Description
----------|------------
`rgba8`   | 32 bits per pixel, RGBA colours

**cbit**<br>
Determines if the last significant bit of the pixel's alpha channel is reserved
as a colour command bit. If the command bit is set and has value `1`, the pixel
is handled as a colour command with special drawing rules.


#### Example

```
pxformat rgba8
pxformat rgba8 cbit=true
```


### `subtex`

Defines a subtexture in the texture. Subtextures are located inside
the image resource (spritesheet) defined by the `imagefile` attribute.

Every subtexture has an implicit reference ID. The ID is the index of
the `subtex` attribute, starting from index 0.

Parameter | Type  | Optional | Default value
----------|-------|----------|--------------
xpos      | int   | No       | -
ypos      | int   | No       | -
xsize     | int   | No       | -
ysize     | int   | No       | -
xanchor   | int   | No       | -
yanchor   | int   | No       | -

**xpos**<br>
Horizontal position of the subtexture inside the image resource. The
subtexture's position is an offset to the pixel in the upper left corner
of the texture (i.e. an offset to coordinate `(0,0)` in the image).

This must be a non-negative value.

**ypos**<br>
Vertical position of the subtexture inside the image resource. The
subtexture's position is an offset to the pixel in the upper left corner
of the texture (i.e. an offset to coordinate `(0,0)` in the image).

This must be a non-negative value.

**xsize**<br>
Width of the subtexture inside the image resource.

This must be a non-negative value.

**ysize**<br>
Height of the subtexture inside the image resource.

This must be a non-negative value.

**xanchor**<br>
Horizontal position of the anchor in the subtexture relative to
its width and height. The anchor is used for pinning the subtexture
to a game world object, i.e. the actual *zero* position of the subtexture.

This can be a negative value.

**yanchor**<br>
Vertical position of the anchor in the subtexture relative to
its width and height. The anchor is used for pinning the subtexture
to a game world object.

This can be a negative value.

#### Example

```
# Subtexture is located at (200,200) in the texture,
# has dimensions (20,20) and an anchor point at (9,10)
subtex 200 200 20 20 9 10
```
