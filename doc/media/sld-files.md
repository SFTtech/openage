# SLD files

SLD is the sprite storage format introduced in Build 66692 of Age of Empires 2: Definitive Edition.

Unlike the [SMX](smx-files.md) and [SLP](slp-files.md) formats, the SLD format does not use
indexed palettes for pixel data. Instead, SLD image data utilizes lossy texture compression
algorithms [DXT1](https://en.wikipedia.org/wiki/S3_Texture_Compression#DXT1) and [DXT4](https://en.wikipedia.org/wiki/S3_Texture_Compression#DXT1) (also known as BC1 and BC4, respectively).

1. [SLD file format](#sld-file-format)
   1. [Header](#header)
   2. [SLD Frame](#sld-frame)
      1. [SLD Frame Header](#sld-frame-header)
   3. [SLD Layers](#sld-layers)
      1. [Layer Content Length](#layer-content-length)
      2. [SLD Main Graphics Layer](#sld-main-graphics-layer)
         1. [Header](#header-1)
         2. [Pixel Data](#pixel-data)
            1. [Command Array](#command-array)
            2. [Compressed Block Array](#compressed-block-array)
      3. [SLD Shadow Graphics Layer](#sld-shadow-graphics-layer)
         1. [Header](#header-2)
         2. [Pixel Data](#pixel-data-1)
            1. [Command Array](#command-array-1)
            2. [Compressed Block Array](#compressed-block-array-1)
      4. [SLD Damage Mask Layer](#sld-damage-mask-layer)
         1. [Header](#header-3)
         2. [Pixel Data](#pixel-data-2)
            1. [Command Array](#command-array-2)
            2. [Compressed Block Array](#compressed-block-array-2)
      5. [SLD Playercolor Mask Layer](#sld-playercolor-mask-layer)
         1. [Header](#header-4)
         2. [Pixel Data](#pixel-data-3)
            1. [Command Array](#command-array-3)
            2. [Compressed Block Array](#compressed-block-array-3)
2. [Excursion: DXT1/BC1 Block Compression](#excursion-dxt1bc1-block-compression)
   1. [Building the Lookup Table](#building-the-lookup-table)
   2. [Constructing the Pixel Block](#constructing-the-pixel-block)
   3. [Example](#example)
3. [Excursion: DXT4/BC4 Block Compression](#excursion-dxt4bc4-block-compression)
   1. [Building the Lookup Table](#building-the-lookup-table-1)
   2. [Constructing the Pixel Block](#constructing-the-pixel-block-1)
   3. [Example](#example-1)
4. [Processing the Command Array (Example)](#processing-the-command-array-example)

In addition to the format description found here, we also
provide a [pattern file](/doc/media/patterns/sld.hexpat) for the [imHex](https://imhex.werwolv.net/)
editor which can be used to explore the SLP data visually.

## SLD file format

SLD files are hierarchically structured into frames, layers and (compressed) color data.

### Header

The SLD file starts with a header:

| Length  | Type   | Description      | Example           |
| ------- | ------ | ---------------- | ----------------- |
| 4 bytes | string | Signature        | SLDX              |
| 2 bytes | uint16 | Version          | 4, 0x0004         |
| 2 bytes | uint16 | Number of frames | 721, 0x02D1       |
| 2 bytes | uint16 | Unknown          | always 0x0000     |
| 2 bytes | uint16 | Unknown          | always 0x0010     |
| 4 bytes | uint32 | Unknown          | always 0xFF000000 |

```cpp
struct sld_header {
  char   file_descriptor[4];
  uint16 version;
  uint16 num_frames;
  uint16 unknown1;
  uint16 unknown2;
  uint32 unknown3;
};
```
Python format: `Struct("< 4s 4H I")`


### SLD Frame

Frame definitions start directly after the file header. There are `sld_header.num_frames` frame
entries that can be read sequentially. Frames can contain up to 5 layers:

* main graphic layer
* shadow layer (optional)
* ??? layer (optional)
* damage mask layer (optional)
* player color mask layer (optional)

Which of these layers are present in a frame is determined by the `frame_type` value
in the frame header.

#### SLD Frame Header

The frame header contains 7 values:

| Length  | Type   | Description     | Example                   |
| ------- | ------ | --------------- | ------------------------- |
| 2 bytes | uint16 | Canvas Width    | 200, 0xC8                 |
| 2 bytes | uint16 | Canvas Height   | 200, 0xC8                 |
| 2 bytes | int16  | Canvas Center X | 100, 0x64                 |
| 2 bytes | int16  | Canvas Center Y | 100, 0x64                 |
| 1 bytes | uint8  | Frame Type      | 7, 0b00000111 (bit field) |
| 1 bytes | uint8  | Unkown          | 1, 0x01                   |
| 2 bytes | uint16 | Frame index     | 5, 0x0005                 |

```cpp
struct sld_frame_header {
  uint16  canvas_width;
  uint16  canvas_height;
  int16  canvas_hotspot_x;
  int16  canvas_hotspot_y;
  uint8   frame_type;
  uint8   unknown1;
  uint16  frame_index;
};
```
Python format: `Struct("< 4H 2B H")`

Frames define a *canvas* which is a square texture that the layers get drawn
into. The canvas size is dertermined by `canvas_width` and `canvas_height`.
Every canvas also has a *hotspot* which is an anchor point for the ingame mouse
pointer. When a unit/building is placed (e.g. in the editor or for building foundation),
the frame texture is attached to the mouse pointer at this position.

`frame_type` is a bit field. This means that every bit set to `1`
indicates that the frame contains a specific type of layer.

Indices are read from left to right:

| Bit index | Description                                                 |
| --------- | ----------------------------------------------------------- |
| 7         | If set to `1`, the frame contains a main graphic layer      |
| 6         | If set to `1`, the frame contains a shadow layer            |
| 5         | If set to `1`, the frame contains a ??? layer               |
| 4         | If set to `1`, the frame contains a damage mask layer       |
| 3         | If set to `1`, the frame contains a player color mask layer |
| 0-2       | Unused                                                      |

**Example**

```
frame_type = 0x17 = 0b0001 0111
```

This frame would contain a *main graphic layer*, a *shadow layer*, a *??? layer*
and a *player color mask layer*.

### SLD Layers

The frame header is immediately followed by the layers specified in the `frame_type`
value. The order of layers is always the same:

1. [main graphics layer](#sld-main-graphics-layer)
1. [shadow graphics layer](#sld-shadow-graphics-layer)
1. ??? layer
1. [damage mask layer](#sld-damage-mask-layer)
1. [playercolor mask layer](#sld-playercolor-mask-layer)

Layers that are not indicated by `frame_type` are skipped.

The layers use different compression methods for the pixel data they store. Here
you can see an overview over the used compression methods per layer.

| Layer            | Compression |
| ---------------- | ----------- |
| Main Graphics    | DXT1        |
| Shadow Graphics  | DXT4        |
| ???              | ???         |
| Damage Mask      | DXT1        |
| Playercolor Mask | DXT4        |

#### Layer Content Length

Every layer starts with 4 Bytes values that signify the number of bytes
in the layer (including the content length value).

| Length  | Type   | Description    | Example     |
| ------- | ------ | -------------- | ----------- |
| 4 bytes | uint32 | Content Length | 618, 0x026A |

```cpp
struct sld_layer_length {
  uint32  content_length;
};
```
Python format: `Struct("< I")`

**Important**: After the `content_length` bytes that contain data, each layer
is padded with null bytes until its length is a multiple of 4. Therefore,
the actual length of the layer in bytes should be calculated like this:

```
actual_length = content_length + ((4 - content_length) % 4)
```


#### SLD Main Graphics Layer

The main graphics layer stores the main texture of the game entity that is
displayed in the game.

This layer always exists and is not optional.

##### Header

The main graphics layer header contains 6 values:

| Length  | Type   | Description                    | Example   |
| ------- | ------ | ------------------------------ | --------- |
| 2 bytes | uint16 | Layer Offset X1 (top left)     | 72, 0x48  |
| 2 bytes | uint16 | Layer Offset Y1 (top left)     | 72, 0x48  |
| 2 bytes | uint16 | Layer Offset X2 (bottom right) | 132, 0x84 |
| 2 bytes | uint16 | Layer Offset Y2 (bottom right) | 108, 0x6C |
| 1 bytes | uint8  | Flag 1                         | 128, 0x80 |
| 1 bytes | uint8  | Unknown                        | 1, 0x01   |

```cpp
struct sld_graphics_header {
  uint16  offset_x1;
  uint16  offset_y1;
  uint16  offset_x2;
  uint16  offset_y2;
  uint8   flag1;
  uint8   unknown1;
};
```
Python format: `Struct("< 4H 2B")`

There are 4 offset values which define the position of the layer inside its
frame's canvas. `offset_x1` and `offset_y1` signify the position of the top left corner
of the layer, while `offset_x2` and `offset_y2` define the position of the
bottom right corner.

Width and height of the layer can be calculated from the 4 offset values.

```
width = offset_x2 - offset_x1
height = offset_y2 - offset_y1
```

You can also calculate the position of the canvas hotspot relative to the layer.

```
layer_hotspot_x = canvas_hotspot_x - offset_x1
layer_hotspot_y = canvas_hotspot_y - offset_y1
```

`flag1` is (presumably) a bit field that contains settings for drawing the layer.

Indices are read from left to right:

| Bit index | Description                                                   |
| --------- | ------------------------------------------------------------- |
| 0         | If set to `1`, the pixel data from previous frames is reused. |
| 1-6       | Unknown                                                       |
| 7         | Set to `1` in playercolor layers.                             |

##### Pixel Data

The pixel data in SLD main graphics is drawn block-wise, i.e. in blocks of 4x4 (= 16) pixels.
Pixel blocks are either fully transparent or colored with RGB/RGBA values from a compressed
**DXT1** data block. Blocks are inserted from left to right, starting in the
top left corner (0,0) of the sprite.

Drawing commands and compressed pixel data blocks for the sprite are stored
in two separate arrays: The **command array** and the **compressed block array**.

###### Command Array

Immediately after the SLD main graphics layer header, there is a length field
containing the number of commands for the command array:

| Length  | Type   | Description          | Example    |
| ------- | ------ | -------------------- | ---------- |
| 2 bytes | uint16 | Command array length | 11, 0x000B |

The command array is then followed by `command_array_length` commands with length 2 Bytes
(i.e. a total number of `2 * command_array_length` bytes).

Each command is a pair of bytes that tells us how many 4x4 pixel blocks are skipped (i.e.
fully transparent) and how many 4x4 pixel blocks are drawn using the compressed DXT1 data blocks
from the **compressed block array**.

**Important:** If bitfield `flag1` in the layer header has bit 7 set, skipping works
slightly different. Instead of drawing a fully transparent block, the pixel block
in the previous *frame* (same position, same layer) is copied.

| Length  | Type  | Description          | Example |
| ------- | ----- | -------------------- | ------- |
| 1 bytes | uint8 | Skipped blocks count | 8, 0x08 |
| 1 bytes | uint8 | Draw blocks count    | 3, 0x03 |

Blocks from the compressed block array are read sequentially. The first *Draw*
call will start reading at index 0 of the array. The next *Draw* command will continue
reading where the previous command stopped.

Note that there is no "end of row" command or something similar which indicates that
the draw pointer has to be moved to the beginning of the next row of blocks in the sprite.
You have to manually check if the drawing offset is still in the sprite boundaries and
move the drawing offset to the next block row if necessary.

See [this section](#processing-the-command-array-example) for an example on how to
process the command array.

###### Compressed Block Array

An array of blocks that each contain pixel data for a 4x4 pixel block in the sprite.

The **compressed block array** follows immediately after the **command array**.
There should be exactly as many blocks as the cumulative number of *Draw* calls
in the command array. However, there is no length field that contains the number of
blocks.

Blocks in the main graphics layer are compressed with **DXT1** compression.
See [the section on DXT1](#excursion-dxt1bc1-block-compression) for descriptions and
examples on how to decompress the data in the block.


#### SLD Shadow Graphics Layer

The shadow graphics layer stores the shadows of the game entity that is
displayed in the game.

This layer seems to be optional.

##### Header

[Same as in the SLD Main Graphics Layer](#header-1)

##### Pixel Data

The pixel data in SLD shadow graphics is drawn block-wise, i.e. in blocks of 4x4 (= 16) pixels
(same as in the main graphics layer). However, unlike the main graphics layer, the pixel blocks
layer are only grayscale colors (i.e. they only use one color channel). They also
use a different compression method: **DXT4**.

The rules for drawing blocks are the same as for the main graphics layer.
Blocks are inserted from left to right, starting in the top left corner (0,0) of the sprite.
Drawing commands and compressed pixel data blocks for the sprite are stored
in two separate arrays: The **command array** and the **compressed block array**.


###### Command Array

[Same as in the SLD Main Graphics Layer](#command-array)

###### Compressed Block Array

An array of blocks that each contain pixel data for a 4x4 pixel block in the sprite.

Blocks in the shadow graphics layer are compressed with **DXT4** compression.
See [the section on DXT4](#excursion-dxt4bc4-block-compression) for descriptions and
examples on how to decompress the data in the block.


#### SLD Damage Mask Layer

The damage mask layer is an overlay for the main graphis layer that stores a modifier value
in each pixel. Internally, this mask is used to display the darkening effect
of damaged buildings/units. Each pixel in the damage mask layer corresponds
to a pixel in the main graphics layer.
Using the modifier value in the damage mask pixel and the current damage percentage
of the unit, the game internally calculates a multiplier that is applied to the RGB
values of the main graphics pixel.

This layer is optional.

##### Header

The damage mask layer header contains 2 values:

| Length  | Type  | Description | Example   |
| ------- | ----- | ----------- | --------- |
| 1 bytes | uint8 | Flag 1      | 128, 0x80 |
| 1 bytes | uint8 | Unknown     | 1, 0x01   |

```cpp
struct sld_mask_header {
  uint8 flag1;
  uint8 unknown1;
};
```
Python format: `Struct("< 2B")`

This presumably is a shortened version of the header for the main graphics layer
and the shadow layer. There is no width and height information as it should have the exact same
size as the main graphics layer of the frame.

`flag1` is (presumably) a bit field that contains settings for drawing the layer.


##### Pixel Data

The pixel data in SLD damage mask is drawn block-wise, i.e. in blocks of 4x4 (= 16) pixels
(same as in the main graphics layer). Technically, the damage mask is stored as an
RGB/RGBA sprite, but it is never displayed as such. Instead, the game turns the RGBA
values into a modifier that is then applied to the main graphics during rendering.

Pixel blocks are either fully transparent or colored with RGB/RGBA values from a compressed
**DXT1** data block. Blocks are inserted from left to right, starting in the
top left corner (0,0) of the sprite.

The rules for drawing blocks are the same as for the main graphics layer.
Blocks are inserted from left to right, starting in the top left corner (0,0) of the sprite.
Drawing commands and compressed pixel data blocks for the sprite are stored
in two separate arrays: The **command array** and the **compressed block array**.

###### Command Array

[Same as in the SLD Main Graphics Layer](#command-array)

###### Compressed Block Array

An array of blocks that each contain pixel data for a 4x4 pixel block in the sprite.

Blocks in the shadow graphics layer are compressed with **DXT1** compression.
See [the section on DXT1](#excursion-dxt1bc1-block-compression) for descriptions and
examples on how to decompress the data in the block.

#### SLD Playercolor Mask Layer

The playercolor mask layer is an overlay for the main graphis layer that stores an
index for a playercolor value in each pixel.
Using the index in the playercolor mask pixel and the owner of the unit,
the game gets a playercolor RGBA value and displays it ingame at the position
of the pixel.

This layer is optional.

##### Header

[Same as in the SLD Damage Mask Layer](#header-3)

##### Pixel Data

The pixel data in SLD playercolor mask is drawn block-wise, i.e. in blocks of 4x4 (= 16) pixels
(same as in the main graphics layer). Playercolor mask pixels only use one color channel,
so they are technically greyscale images. However, the color channel value is never
directly displayed and instead treated as an index for looking up the actual
player color value from a palette. The compression method used for the pixel
blocks is **DXT4**.

The rules for drawing blocks are the same as for the main graphics layer.
Blocks are inserted from left to right, starting in the top left corner (0,0) of the sprite.
Drawing commands and compressed pixel data blocks for the sprite are stored
in two separate arrays: The **command array** and the **compressed block array**.

###### Command Array

[Same as in the SLD Main Graphics Layer](#command-array)

###### Compressed Block Array

An array of blocks that each contain pixel data for a 4x4 pixel block in the sprite.

Blocks in the shadow graphics layer are compressed with **DXT4** compression.
See [the section on DXT4](#excursion-dxt4bc4-block-compression) for descriptions and
examples on how to decompress the data in the block.


## Excursion: DXT1/BC1 Block Compression

DXT1 block compression is a method to store a 4x4 pixel block in 8 bytes of data.
Each block can display up to 4 different 16-Bit RGB(A) colors. The 16 pixels in the
block are represented by 2-Bit indices for a lookup table referencing the 4 RGBA colors.

The 8 byte block is organized as follows:

| Length  | Type   | Description       |
| ------- | ------ | ----------------- |
| 2 bytes | uint16 | Reference color 0 |
| 2 bytes | uint16 | Reference color 1 |
| 4 bytes | uint32 | Pixel indices     |

```cpp
struct bc1_block {
  uint16 color0;
  uint16 color1;
  uint32 pixel_indices;
};
```
Python format: `Struct("< 2H I")`

`color0` and `color1` are 16-Bit colors using the R5G6B5 format, i.e. the red
channel uses 5 Bits, the green channel uses 6 Bits, and the blue channel uses
5 Bits. You can get the invidual values for each channel with a bitmask and bitshift:

```
r0 = (color0 & 0xF800) >> 11  (5 Bits; bit index 0-4)
g0 = (color0 & 0x07E0) >> 5   (6 Bits; bit index 5-10)
b0 = (color0 & 0x001F)        (5 Bits; bit index 11-15)

RGB16: (r0, g0, b0)

(Optional conversion to RGB32 with 8 Bit channel size):
RGB32: (r0 * 8, g0 * 4, b0 * 8)
```

Uncompressing the 4x4 pixel block requires 2 steps. First, we have to build
the lookup table for the 4 RGB colors using the two reference colors. Second,
we can assign colors to the pixels in the block by looking up their
indices.

### Building the Lookup Table

The lookup table has 4 entries that map a 2-Bit index to a 16-Bit RGB color
value. The first 2 entries are the reference colors `color0` and `color1`.

| Index | color Value |
| ----- | ----------- |
| 00    | `color0`    |
| 01    | `color1`    |

Using the reference colors, we have to interpolate the color values for the
other two indices. The interpolation works differently depending on which of
the two reference color values (that means their 16-Bit integer values) is greater.

If `color0` is **greater than** `color1`, the color values for the remaining
indices are calculated as follows:

| Index | Color Value                                |
| ----- | ------------------------------------------ |
| 10    | `color2 = (2/3) * color0 + (1/3) * color1` |
| 11    | `color3 = (1/3) * color0 + (2/3) * color1` |

(The factors above are applied to each of the three color channels. Similarly,
the addition of coulour values is the addition of the colors' respective color
channels.)

If `color0` is **smaller than or equal to** `color1`, only the color value of `color2`
is interpolated, while `color3` represents a color value with full transparency:

| Index | Color Value                                |
| ----- | ------------------------------------------ |
| 10    | `color2 = (1/2) * color0 + (1/2) * color1` |
| 11    | `color3 = (0, 0, 0, 0)`                    |

The latter case allows a pixel block to contain transparent pixels, although that
sacrifices one of the possible color values.

### Constructing the Pixel Block

After we have created the lookup table, we can reconstruct the pixel block by looking
up the pixel indices in the lookup table. There are 16 indices in each data block,
each with a size of 2 Bit (= 32 Bit (4 Byte) in total). The position of an index
in the data block also determines the position in the resulting 4x4 pixel block.

The easiest way to contruct the 4x4 pixel block is to fill it from left to right
starting in the top left corner. To do this, read the `pixel_indices` value
as a 32-Bit unsigned integer with little endian byte order. You can then look up
the colors for all pixel indices one by one starting with the two least
significant bits.

A very simple implementation of this can be achieved
by applying a `0b11` bitmask that extracts the current pixel index and a
bitshift by 2 to get to the next pixel index (see below).

```python
pixels = []
bitmask = 0b11
for shift in range(16):
    bc1_idx = pixel_indices & bitmask
    col = lookup_table[bc1_idx]
    pixels.append(col)
    pixel_indices = pixel_indices >> 2
```

The pixels in the generated list have the correct order if you want to insert them
into the pixel block from left to right and top to bottom.

### Example

Let's look at the the following compressed data block in the file:

```
BC1 block: 00 00 82 10 FB 37 17 77
```

First of all, we can extract the three values `color0`, `color1`, and `pixel_indices` in the block
by reading them as integers in little endian format.

```
color0:        0x0000
color1:        0x1082
pixel_indices: 0x771737FB
```

Using `color0` and `color1`, we can construct the lookup table. The first step is to
get the values for the three color channels.

```
color0: 0x0000
r0 = (color0 & 0xF800) >> 11 = 0b00000  = 0    (5 Bits; bit index 0-4)
g0 = (color0 & 0x07E0) >> 5  = 0b000000 = 0    (6 Bits; bit index 5-10)
b0 = (color0 & 0x001F)       = 0b00000  = 0    (5 Bits; bit index 11-15)

RGB16: (0, 0, 0)
RGB32: (0, 0, 0)

color1: 0x1082
r1 = (color1 & 0xF800) >> 11 = 0b00010  = 2    (5 Bits; bit index 0-4)
g1 = (color1 & 0x07E0) >> 5  = 0b000100 = 4    (6 Bits; bit index 5-10)
b1 = (color1 & 0x001F)       = 0b00010  = 2    (5 Bits; bit index 11-15)

RGB16: (2, 4, 2)
RGB32: (16, 16, 16)
```

Next up, we have to determine the other 2 color values. Since the value of `color0` is
smaller than `color1`, we know that `color3` must be fully transparent. We only
have to interpolate `color2`.

```
color2 c= (1/2) * color0 + (1/2) * color1
        = (1/2) * (0,0,0) + (1/2) * (2,4,2)
        = (0,0,0)         + (1,2,1)
        = (1,2,1)

RGB16: (1, 2, 1)
RGB32: (8, 8, 8)

color3 = (0, 0, 0, 0)   (full transparency)
```

In the end, our lookup table looks like this (using RGBA16 format):


| Index | Color Value         |
| ----- | ------------------- |
| 00    | `c0 = (2, 4, 2, 1)` |
| 01    | `c1 = (0, 0, 0, 1)` |
| 10    | `c2 = (1, 2, 1, 1)` |
| 11    | `c3 = (0, 0, 0, 0)` |

All we have to do now is to fill the pixel block using the `pixel_indices` values.
Using the algorithm from the previous section, we can read it from the back
to get the lookup indices.

```
pixel_indices (Hex):    0x771737FB
pixel_indices (Binary): 01110111000101110011011111111011
    (split into 2 Bit): 01 11 01 11 00 01 01 11 00 11 01 11 11 11 10 11
                                                                     ^
                                                                     start here
                                                                     (0,0)
```

When we assign the indices from left to right and top to bottom, the assignment
looks like this:

| Coord  | x0   | x1   | x2   | x3   |
| ------ | ---- | ---- | ---- | ---- |
| **y0** | `11` | `10` | `11` | `11` |
| **y1** | `11` | `01` | `11` | `00` |
| **y2** | `11` | `01` | `01` | `00` |
| **y3** | `11` | `01` | `11` | `01` |

Or alternatively, using the color lookups:

| Coord  | x0  | x1  | x2  | x3  |
| ------ | --- | --- | --- | --- |
| **y0** | c3  | c2  | c3  | c3  |
| **y1** | c3  | c1  | c3  | c0  |
| **y2** | c3  | c1  | c1  | c0  |
| **y3** | c3  | c1  | c3  | c1  |


## Excursion: DXT4/BC4 Block Compression

DXT4 block compression is a method to store a 4x4 pixel block in 8 bytes of data.
Each block can store up to 8 different 8-Bit single-channel colors (only the red
channel of an RGBA color used). You can also interpret them as greysale images since
these also only use 8-Bit. DXT4 compressed pixel values are usually not used for display,
but for storing other data necessary during rendering.

The 16 pixels in the block are represented by 3-Bit indices for a lookup table
referencing the 8 single-channel colors.

The 8 byte block is organized as follows:

| Length  | Type    | Description       |
| ------- | ------- | ----------------- |
| 1 bytes | uint8   | Reference color 0 |
| 1 bytes | uint8   | Reference color 1 |
| 6 bytes | char[6] | Pixel indices     |

```cpp
struct bc4_block {
  uint8 color0;
  uint8 color1;
  uint8 pixel_indices[6];
};
```
Python format: `Struct("< 8B")`

`color0` and `color1` are 8-Bit single-channel colors, i.e. they represent the
red channel of an RGB color.

Uncompressing the 4x4 pixel block requires 2 steps. First, we have to build
the lookup table for the 8 single-channel colors using the two reference colors.
Second, we can assign colors to the pixels in the block by looking up their
indices.

### Building the Lookup Table

The lookup table has 8 entries that map a 3-Bit index to a 8-Bit red channel color
value. The first 2 entries are the reference colors `color0` and `color1`.

| Index | Color Value |
| ----- | ----------- |
| 000   | `color0`    |
| 001   | `color1`    |

Using the reference colors, we have to interpolate the color values for the
other 6 indices. The interpolation works differently depending on which of
the two reference color values (that means their 8-Bit integer values) is greater.

If `color0` is **greater than** `color1`, the color values for the remaining
indices are calculated as follows:

| Index | Color Value                                |
| ----- | ------------------------------------------ |
| 010   | `color2 = (6 * color0 + 1 * color1) / 7.0` |
| 011   | `color3 = (5 * color0 + 2 * color1) / 7.0` |
| 100   | `color4 = (4 * color0 + 3 * color1) / 7.0` |
| 101   | `color5 = (3 * color0 + 4 * color1) / 7.0` |
| 110   | `color6 = (2 * color0 + 5 * color1) / 7.0` |
| 111   | `color7 = (1 * color0 + 6 * color1) / 7.0` |

If `color0` is **smaller than or equal to** `color1`, only the color value of 4 other colors (`color2`,
`color3`, `color4`, `color5`) are interpolated. `color6` is set to `0` and `color7` is set to
`255`.

| Index | Color Value                                |
| ----- | ------------------------------------------ |
| 010   | `color2 = (4 * color0 + 1 * color1) / 5.0` |
| 011   | `color3 = (3 * color0 + 2 * color1) / 5.0` |
| 100   | `color4 = (2 * color0 + 3 * color1) / 5.0` |
| 101   | `color5 = (1 * color0 + 4 * color1) / 5.0` |
| 110   | `color6 = 0`                               |
| 111   | `color7 = 255`                             |

### Constructing the Pixel Block

After we have created the lookup table, we can reconstruct the pixel block by looking
up the pixel indices in the lookup table. There are 16 indices in each data block,
each with a size of 3 Bit (= 48 Bit (6 Byte) in total). The position of an index
in the data block also determines the position in the resulting 4x4 pixel block.

The easiest way to contruct the 4x4 pixel block is to fill it from left to right
starting in the top left corner. To do this, split the `pixel_indices` value
into two 3 Byte sections and convert them to 32-Bit unsigned integers
with little endian byte order. You can then look up the colors for all pixel indices
one by one, 3 bits at a time.

A very simple implementation of this can be achieved
by applying a `0b111` bitmask that extracts the current pixel index and a
bitshift by 3 to get to the next pixel index (see below).

```python
pixels = []
bitmask = 0b111
pixel_indices0 = pixel_indices[0:3]
for shift in range(8):
    bc4_idx = pixel_indices0 & bitmask
    col = lookup_table[bc4_idx]
    pixels.append(col)
    pixel_indices0 = pixel_indices0 >> 3

pixel_indices1 = pixel_indices[3:6]
for shift in range(8):
    bc4_idx = pixel_indices1 & bitmask
    col = lookup_table[bc4_idx]
    pixels.append(col)
    pixel_indices1 = pixel_indices1 >> 3
```

The pixels in the generated list have the correct order if you want to insert them
into the pixel block from left to right and top to bottom.

### Example

Let's look at the the following compressed data block in the file:

```
BC1 block: 10 82 33 45 FB 37 17 77
```

First of all, we can extract the three values `color0`, `color1`, and `pixel_indices` in the block.
WE can already split `pixel_indices` into two 3 byte blocks that we convert to integers in
little endian format.

```
color0:         0x10
color1:         0x82
pixel_indices:  33 45 FB 37 17 77
pixel_indices0: 0xFB4533
pixel_indices1: 0x771737
```

Using `color0` and `color1`, we can construct the lookup table. Since the value of `color0` is
smaller than `color1`, we know that `color6` must be 0 and `color7` must be 255. The rest
of the colors has to be interpolated.

```
color0: 0x10 = 16
color1: 0x82 = 130
color2 = (4 * color0 + 1 * color1) / 5.0
       = (4 * 16     + 1 * 130)    / 5.0
       = 194 / 5.0
       = 38
color3 = (3 * color0 + 2 * color1) / 5.0
       = (3 * 16     + 2 * 130)    / 5.0
       = 308 / 5.0
       = 61
color4 = (2 * color0 + 3 * color1) / 5.0
       = (2 * 16     + 3 * 130)    / 5.0
       = 422 / 5.0
       = 84
color5 = (1 * color0 + 4 * color1) / 5.0
       = (1 * 16     + 4 * 130)    / 5.0
       = 536 / 5.0
       = 107
color6 = 0
color7 = 255
```

In the end, our lookup table looks like this:

| Index | Color Value |
| ----- | ----------- |
| 000   | `c0 = 16`   |
| 000   | `c1 = 130`  |
| 010   | `c2 = 38`   |
| 011   | `c3 = 61`   |
| 100   | `c3 = 84`   |
| 101   | `c3 = 107`  |
| 110   | `c3 = 0`    |
| 111   | `c3 = 255`  |

All we have to do now is to fill the pixel block using the `pixel_indices` values.
Using the algorithm from the previous section, we can read it from the back
to get the lookup indices.

```
pixel_indices0 (Hex):    0xFB4533
pixel_indices0 (Binary): 111110110100010100110011
     (split into 3 Bit): 111 110 110 100 010 100 110 011
                                                     ^
                                                     start here
                                                     (0,0)

pixel_indices1 (Hex):    0x771737
pixel_indices1 (Binary): 011101110001011100110111
     (split into 3 Bit): 011 101 110 001 011 100 110 111
                                                     ^
                                                     (2,0)
```

When we assign the indices from left to right and top to bottom, the assignment
looks like this:

| Coord  | x0    | x1    | x2    | x3    |
| ------ | ----- | ----- | ----- | ----- |
| **y0** | `011` | `110` | `100` | `010` |
| **y1** | `100` | `110` | `110` | `111` |
| **y2** | `111` | `110` | `100` | `011` |
| **y3** | `001` | `110` | `101` | `011` |

Or alternatively, using the color lookups:

| Coord  | x0  | x1  | x2  | x3  |
| ------ | --- | --- | --- | --- |
| **y0** | c3  | c6  | c4  | c2  |
| **y1** | c4  | c6  | c6  | c7  |
| **y2** | c7  | c6  | c4  | c3  |
| **y3** | c1  | c6  | c5  | c3  |


## Processing the Command Array (Example)

```
Width: 32px
Height: 12px
Command Array Length (Hex): 03 00
Command Array (Hex): 03 01  07 04  05 04
```

The sprite in this example has the dimensions 32x12 (width x height). Since
we are operating on 4x4 blocks, that means the sprite demensions in blocks
are 8x3.

| Block | 0   | 1   | 2   | 3   | 4   | 5   | 6   | 7   |
| ----- | --- | --- | --- | --- | --- | --- | --- | --- |
| **0** | -   | -   | -   | -   | -   | -   | -   | -   |
| **1** | -   | -   | -   | -   | -   | -   | -   | -   |
| **2** | -   | -   | -   | -   | -   | -   | -   | -   |

We start in the top left corner at block (0, 0). The first command is `03 01`
which means we need 3 transparent blocks ((0, 0), (1, 0) and (2, 0)) and
then draw 1 block from the compressed block array at (3,0).

| Block | 0   | 1   | 2   | 3   | 4   | 5   | 6   | 7   |
| ----- | --- | --- | --- | --- | --- | --- | --- | --- |
| **0** |     |     |     | X   | -   | -   | -   | -   |
| **1** | -   | -   | -   | -   | -   | -   | -   | -   |
| **2** | -   | -   | -   | -   | -   | -   | -   | -   |

The next command is `07 04` which means we move the draw pointer
forwards by 7 blocks. Since the current row only has 4 blocks remaining
((4, 0) to (7, 0)), we wrap around to the next row. We then
skip 3 blocks at the beginning of this row. Afterwards, we have
to draw 4 blocks from the compressed block array starting at
(3, 1).

| Block | 0   | 1   | 2   | 3   | 4   | 5   | 6   | 7   |
| ----- | --- | --- | --- | --- | --- | --- | --- | --- |
| **0** |     |     |     | X   |     |     |     |     |
| **1** |     |     |     | X   | X   | X   | X   | -   |
| **2** | -   | -   | -   | -   | -   | -   | -   | -   |

The final command is `05 04`, so 5 skipped blocks followed by
4 draw blocks. We first skip the remaining block in the current
row, then move to the next one and skip another 4 blocks.
Then we draw 4 blocks from the compressed block array. Since
this is the last command, there should be no more blocks to draw
and the sprite is successfully decompressed.

| Block | 0   | 1   | 2   | 3   | 4   | 5   | 6   | 7   |
| ----- | --- | --- | --- | --- | --- | --- | --- | --- |
| **0** |     |     |     | X   |     |     |     |     |
| **1** |     |     |     | X   | X   | X   | X   |     |
| **2** |     |     |     |     | X   | X   | X   | X   |
