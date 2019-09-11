# SMP files

SMP files are the successor format to SLP files. Like SLP files,
they contain animations, shadows and outlines for units. SMPs
were introduced with Age of Empires 2: Definitive Edition.


## SMP file format

SMPs share a lot of structural similarities to SLPs. However,
all of the drawing commands have changed, so the formats are not
compatible to each other.


### Header

The SMP file starts with a header:

Length   | Type   | Description        | Example
---------|--------|--------------------|--------
4 bytes  | string | Version            | SMP$
4 bytes  | int32  | ??                 | 256, 0x00000100 (same value for almost all units)
4 bytes  | int32  | Number of frames   | 721, 0x000002D1
4 bytes  | int32  | ??                 | 1, 0x0000001 (almost always 0x00000001)
4 bytes  | int32  | Number of frames   | 721, 0x000002D1 (0x00000001 for version 0x0B)
4 bytes  | int32  | possibly checksum  | 0x8554F6F3
4 bytes  | int32  | File size in bytes | 0x003D5800
4 bytes  | int32  | Version?           | 0x0B or 0x0C
32 bytes | string | Comment            | Apparently the file path on FE's machines


```cpp
struct smp_header {
  char  version[4];
  int32 ??;
  int32 num_frames;
  int32 ??;
  int32 num_frames;
  int32 checksum;
  int32 file_size;
  int32 version;
  char  comment[32];
};
```
Python format: `Struct("< 4s 7i 32s")`


### SMP Bundle Offsets

SMP frames come in bundles that can consist of up to 3 images. The images
contain the following data:

* main sprite
* shadow for that sprite (optional)
* outline (optional, only used for units)

After the header, there are `num_frames` entries of `smp_bundle_offset`.
Every `smp_bundle_offset` stores the offset to a bundle within the SMP
file.

```cpp
struct smp_bundle_offset {
  uint32 offset;
}
```
Python format: `Struct("< I")`


### SMP Bundle header

At every `smp_bundle_offset` there is a 32 bytes long bundle header
that stores how many images exist for the frame in a 4 byte length
field at the end.

```cpp
struct smp_bundle_offset {
  28 bytes unused; # stores frame header info in 0x0B SMP version
  uint32   length;
}
```

In version 0x0B, the bundle header has the same structure as the frame
headers (see below), except that `outline_table_offset` and
`cmd_table_offset` and `frame_type` are set to zero. In version
0x0C, all fields except the length fiield are set to zero.


### SMP Frame Header

After the bundle header, there are `length` entries of `smp_frame_header`.
These struct are similar to the SLP Frame Info struct in that they store
metadata about the frame.

Length   | Type   | Description                | Example
---------|--------|----------------------------|--------
4 bytes  | uint32 | Width of image             | 168, 0x000000A8
4 bytes  | uint32 | Height of image            | 145, 0x00000091
4 bytes  | uint32 | Centre of sprite (X coord) | 88, 0x00000058
4 bytes  | uint32 | Centre of sprite (Y coord) | 99, 0x00000063
4 bytes  | int32  | Frame type                 | 0x02, 0x04 or 0x08
4 bytes  | int32  | Outline table offset       | 600, 0x00000258
4 bytes  | int32  | Command table offset       | 0, 0x00000000
4 bytes  | int32  | ??                         | 0x01, 0x02 or 0x80

```cpp
struct smp_frame_header {
  uint32 width;
  uint32 height;
  uint32 hotspot_x;
  uint32 hotspot_y;
  uint32 frame_type;
  uint32 outline_table_offset;
  uint32 cmd_table_offset;
  uint32 ??;
};
```
Python format: `Struct("< 8i")`

* Frame types can be `0x02` (main graphic), `0x04` (shadow) or `0x08`
(outline). In version 0x0B, outlines have a different frame type: `0x10`.
* Outline and command table offsets **are always relative to the bundle offset**.


### SMP Frame row edge

At `outline_table_offset` (after the `smp_frame_header` structs), an array of
`smp_frame_row_edge` (of length `height`) structs begins.

Length   | Type   | Description   | Example
---------|--------|---------------|-----------
2 bytes  | uint16 | Left spacing  | 20, 0x0014
2 bytes  | uint16 | Right spacing | 3, 0x0003

```cpp
struct smp_frame_row_edge {
  uint16 left_space;
  uint16 right_space;
};
```
Python format: `Struct("< H H")`

For every row, `left_space` and `right_space` specify the number of transparent
pixels, from each side to the center. For example, in a 50 pixels wide row, with
a `smp_frame_row_edge` of `{ .left_space = 20, .right_space = 3 }`, the leftmost
20 pixels will be transparent, the rightmost 3 will be transparent and there
will be 27 pixels of graphical data provided through some number of commands.

If the right or left value is `0xFFFF`, the row is completely transparent.
Note that there are no command bytes for these rows, so will have to be skipped
"manually".

`width - left_space - right_space` = number of pixels in this line.


### SMP command table

At `smp_frame_header.cmd_table_offset`, an array of
uint32 (of length `height`) begins:

```cpp
struct smp_command_offset {
  uint32 offset;
}
```
Python format: `Struct("< I")`

Each `offset` defines the offset (beginning) of the first command of a row.
The first `offset` in this array is the first drawing command for the image.
All offsets are relative to their respective `smp_bundle_offset`.

These are not actually necessary to use (but obviously are necessary to read),
since the commands can be read sequentially, although they can be used for
validation purposes.


### SMP drawing commands

The image is drawn line by line, a line is finished with the "End of line"
command (0x03). A command is a one-byte number (`cmd_byte`), followed
by command-specific data with a length (number of pixels) varying
depending on the command. The next command immediately follows the
previous command's data.

In contrast to SLPs, the SMP format uses a much more simplified command set
that only contains four commands: *Skip*, *Draw*, *Player Color Draw*
and *End of Row*. The type of command is stored in the 2 least significant
bits of the command byte. The 6 most significant bytes define the length of the
command.

Each command triggers a drawing method for n = "Count" pixels.

For examples of drawing commands, see the [Examples](#examples) section.


### Full command list

An `X` signifies that the bit can have any value. These bits are used for
storing the length (pixel count) of the command.

The commands works slightly different for each frame type.


#### Main graphics type

Command Name     | Byte value    | Pixel Count              | Description
-----------------|---------------|--------------------------|------------
Skip             | `0bXXXXXX00`  | `(cmd_byte >> 2) + 1`    | *Count* transparent pixels should be drawn from the current position.
Draw             | `0bXXXXXX01`  | `(cmd_byte >> 2) + 1`    | An array of length `pixel_count * 4` bytes filled with 4-byte SMP pixels follows (see [SMP Pixel](#smp-pixel))
Playercolor Draw | `0bXXXXXX10`  | `(cmd_byte >> 2) + 1`    | An array of length `pixel_count * 4` bytes filled with 4-byte SMP pixels follows (see [SMP Pixel](#smp-pixel))
End of Row       | `0bXXXXXX11`  | 0                        | End of commands for this row. If more commands follow, they are for the next row.

* When converting the main graphics, the alpha values from the palette are
apparently ignored by the game.


#### Shadow type

Command Name     | Byte value    | Pixel Count              | Description
-----------------|---------------|--------------------------|------------
Skip             | `0bXXXXXX00`  | `(cmd_byte >> 2) + 1`    | *Count* transparent pixels should be drawn from the current position.
Draw             | `0bXXXXXX01`  | `(cmd_byte >> 2) + 1`    | An array of length `pixel_count * 4` bytes filled with 1-byte alpha values follows.
End of Row       | `0bXXXXXX11`  | 0                        | End of commands for this row. If more commands follow, they are for the next row.

* Shadow frames (frame type `0x04`) sometimes do not explicitely draw the last
pixel in a row. If that happens, the openage converter draws the last *Draw* command
again


#### Outline type

Command Name     | Byte value    | Pixel Count              | Description
-----------------|---------------|--------------------------|------------
Skip             | `0bXXXXXX00`  | `(cmd_byte >> 2) + 1`    | *Count* transparent pixels should be drawn from the current position.
Draw             | `0bXXXXXX01`  | `(cmd_byte >> 2) + 1`    | *Count* player color pixels should be drawn from the current position.
End of Row       | `0bXXXXXX11`  | 0                        | End of commands for this row. If more commands follow, they are for the next row.

* SMP files do ot specify a color from a palette. The openage converter always uses the
color from index 0 in the player color palette for these *Draw* commands.


### SMP Pixel

SMP pixels store a palette index, palette number and section as well
as occlusion masks.

Length   | Type   | Description                | Example
---------|--------|----------------------------|--------
1 byte   | uint8  | Palette index              | 20, 0x0014
1 byte   | uint8  | Palette number and section | 7, 0x0007
1 byte   | uint8  | Damage mask                | 128, 0x80 (only high nibble is used)
1 byte   | uint8  | Damage mask                | 3, 0x03

```cpp
struct smp_pixel {
  uint8 px_index;
  uint8 px_palette;
  uint8 px_damage_mask_1;
  uint8 px_damage_mask_2;
};
```
Python format: `Struct("< B B B B")`

Colors are stored in JASC palettes with 1024 colors. The palettes are assigned an index
which is stored in a `palette.conf` file. To find the encoded color of a SMP pixel,
the *palette index* and *palette section* have to be determined from the `px_palette`
value. The palette index is stored in the 6 most significant bits, while the palette
section is stored in the 2 least significant bits.

```
palette_index = px_palette >> 2

palette_section = px_palette & 0b00000011
```

`px_index` has to be added to `256 * palette_section` to retrieve the actual
index for the palette. This index can then be used to get the color value from
the palette with the index `palette_index`.

The other two bytes in the file are used for masking when units get damaged.


#### Examples

##### Retrieving a color value from a SMP pixel

Let's assume we have a single SMP pixel and want to find the correct palette for it.

```
SMP pixel example: 0xEF 0x57 0x50 0x20
```

The second byte value `0x57` contains the palette information.

We can retrieve the *palette index* by shifting `0x57` by 2 to the right. Alternatively,
you can also divide the value by 4 and floor the result.

```
palette_index = 0x57 >> 2 = 0b01010111 >> 2 = 0b00010101 = 21
```

The *palette index* is 21 which maps to the palette `b_west.pal` in the `palettes.conf`
of Age of Empires 2: Definitive Edition.

Now we have to determine the section of the palette that is used  for the pixel. To
do that, we can either look at the 2 most significant or calculate
`px_palette mod 4`.

```
palette_section = 0x57 & 0b00000011 = 0b01010111 & 0b00000011 = 0b00000011 = 3
```

Here, the *palette section* is 3 which would cover the indexes 512 to 767 in
`b_west.pal`. From the retrieved values we can now determine the actual index
in the palette by adjusting `px_index = 0xEF` to the palette section.

```
color_index = px_index + 256 * palette_section
            = 0xEF + 256 * 0x03
            = 239  + 256 * 3
            = 1007
```

Finally, we can use this index to look up the color value in `b_west.pal`.
In our example, the RGBA value is (5,19,4,255).
