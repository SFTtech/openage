# Blendmask Format Specification

**Format Version:** 1

The openage blendmask format is a plaintext configuration file format for defining
a blending pattern using alpha masks. A blendmask defines a table of directional filters
to blend a terrain texture into another texture. Standard tiles have 8 directional
edges where they can border other textures. Thus, there are `2^8 = 256` total
combinations which can be used as filters.

More info on the blending mechanism can be found in the [blendomatic docs](/doc/media/blendomatic.md).

All attributes start with a defined keyword followed by parameter values. Some
parameters have default values and are optional. The preferred file extension is
`.blmask`.


## Quick Reference

```
# This is a blendtable configuration file
# comments start with # and are ignored

# file version
version 1

# source image definitions
imagefile <image_id> <filename>

# How much the texture is scaled down at default zoom level
scalefactor <factor>

# selection of blendomatic borders
mask <directions> <image_id> <xpos> <ypos> <xsize> <ysize>
```


## Data Type Formatting

Type     | Example | Description
---------|---------|---------
bits     | `0b01`  | Number represented as bits
int      | `5`     | Signed Integer
float    | `1.2`   | Float
string   | `"bla"` | String of characters enclosed by `"`
token    | `off`   | Alphanumeric predefined keyword


## Attributes

### `version`

Version of the blendtable format. Increments every time the syntax
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
scalefactor 2.0  # scaled down to 50x50 at default zoom; original 100x100 at 2x zoom in
scalefactor 4.0  # scaled down to 25x25 at default zoom; original 100x100 at 4x zoom in

# Factors <1 result in upscaling
scalefactor 0.5  # scaled up to 200x200 at default zoom; original 100x100 at 2x zoom out
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
Path to the image resource for the blend texture on the filesystem.

There are two ways to specify a path: relative and absolute. Relative
paths are relative to the location of the blendmask definition file. They
can only refer to image resources that are in the same modpack.

```
imagefile 0 "grass.png"       # grass.png is in the same folder as the blendmask file
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


### `mask`

Tells the renderer which mask it has to use for what adjacent directions.

Parameter  | Type      | Optional | Default value
-----------|-----------|----------|--------------
directions | int, bits | No       | -
image_id   | int       | No       | -
xpos       | int       | No       | -
ypos       | int       | No       | -
xsize      | int       | No       | -
ysize      | int       | No       | -

**directions**<br>
The directions for which the *blending* terrain touches the *blended*
terrain, stored as an 8-Bit bitfield value. Each bit position represents
a direction around the blended tile. A mask is used if this value
matches the positions around the adjacent tiles.

They blending terrain positions are assgned to indices using the
following logic.

```
@   = the blended tile
0-7 = adjacent tiles

7  0  1

6  @  2

5  4  3
```

For every adjacent tile of `@` that is from the lending terrain, the bit
at the respective index is set. For example, if `@` is blended by another
terrain adjacent in directions north-west, north and north-east, the bits
at index 7, 0 and 1 are set (`0b10000011`). This is stored as a decimal integer
value or a bits value.

```
mask 131 ...   # 131 equals 0b10000011
```

The full range of 256 patterns can be used as lookups. However, 20
mandatory directions must be defined in every blendmask file to ensure
compatibility to Genie Engine blending style. The list of mandatory
blend directions can be found in section [Mandatory Directions](#mandatory-directions).
These mandatory directions are also used as fallback if a pattern
was left undefined.

**image_id**<br>
ID of the image resource that is used as a source for the blend mask.

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


#### Example

```
mask 131 0 0 0 12 124
mask 0b00101010 0 0 0 34 12
```

## Mandatory Directions

**Diagonal directions:**

Integer value | Bits value   | Adjacent directions
--------------|--------------|--------------------
2             | `0b00000010` | north east
8             | `0b00001000` | south east
32            | `0b00100000` | south west
128           | `0b10000000` | north west

**Vertical/Horizontal one-sided adjacency:**

Integer value | Bits value   | Adjacent directions
--------------|--------------|--------------------
1             | `0b00000001` | north
4             | `0b00000100` | east
16            | `0b00010000` | south
64            | `0b01000000` | west

**Vertical/Horizontal two-sided adjacency:**

Integer value | Bits value   | Adjacent directions
--------------|--------------|--------------------
5             | `0b00000101` | north, east
17            | `0b00010001` | north, south
20            | `0b00010100` | east, south
65            | `0b01000001` | north, west
68            | `0b01000100` | east, west
80            | `0b01010000` | south, west

**Vertical/Horizontal three-sided adjacency:**

Integer value | Bits value   | Adjacent directions
--------------|--------------|--------------------
21            | `0b00010101` | north, east, south
69            | `0b01000101` | north, east, west
81            | `0b01010001` | north, south, west
84            | `0b01010100` | east, south, west

**Vertical/Horizontal four-sided adjacency:**

Integer value | Bits value   | Adjacent directions
--------------|--------------|--------------------
85            | `0b01010101` | north, east, south, west


Visual representation ingame:

```
    id:   2              8             32             128
          NE             SE            SW             NW

          0              0              0              #
        00000          00000          00000          #####
      00000000#      000000000      #00000000      000###000
    000000000####  0000000000000  ####000000000  0000000000000
      00000000#      000###000      #00000000      000000000
        00000          #####          00000          00000
          0              #              0              0

    id:   1              4              16             64
          N              E              S              W

          #              0              0              #
        0####          00000          00000          ####0
      00000####      000000000      000000000      ####00000
    000000000####  00000000#####  ####000000000  ####000000000
      000000000      00000####      ####00000      000000000
        00000          0####          ####0          00000
          0              #              #              0

    id:   5             17             20             65             68             80
         N-E            N-S            E-S            N-W            E-W            S-W

          #              #              0              #              #              #
        0####          0####          00000          #####          ####0          ####0
      000######      00000####      000000000      #########      ####00000      ######000
    00000########  ####00000####  #############  #############  ####0000#####  ########00000
      000######      ####00000      #########      000000000      00000####      ######000
        0####          ####0          #####          00000          0####          ####0
          #              #              #              0              #              #

    id:  21             69             81             84
        N-E-S          N-E-W          N-S-W          E-S-W

          #              #              #              #
        0####          #####          #####          ####0
      00000####      ####0####      ####0####      ####00000
    ####00000####  ####00000####  ####00000####  ####00000####
      ####0####      00000####      ####00000      ####0####
        #####          0####          ####0          #####
          #              #              #              #

    id:  85
       N-E-S-W

          #
        #####
      ####0####
    ####00000####
      ####0####
        #####
          #
```
