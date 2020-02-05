# SLP files

SLP files are stored in the DRS files. They contain all the (animation)
textures. Like the DRS format, it can also be read sequentially.

## SLP file format

### Header (up to version 3.0)
The SLP file starts with a header:

Length   | Type   | Description      | Example
---------|--------|------------------|--------
4 bytes  | string | Version          | 2.0N
4 bytes  | int32  | Number of frames | 1, 0x00000001
24 bytes | string | Comment          | ArtDesk 1.00 SLP Writer

```cpp
struct slp_header {
  char  version[4];
  int32 num_frames;
  char  comment[24];
};
```
Python format: `Struct("< 4s i 24s")`

### Header (since version 4.0X)
Age of Empires 1: Definitive Edition SLPs (v4.0) use a differently structured header:

Length   | Type    | Description             | Example
---------|---------|-------------------------|--------
4 bytes  | string  | Version                 | 4.0X
2 bytes  | int16   | Number of frames        | 960, 0x000003C0
2 bytes  | int16   | Type                    | 8, 0x08 (for VFX SLPs)
2 bytes  | int16   | Number of directions    | 32, 0x0020 (always set to 1)
2 bytes  | int16   | Frames per direction    | 45, 0x002D (always set to num_frames)
4 bytes  | int32   | Palette ID              | always 0x00000000 (v4.1 uses it)
4 bytes  | int32   | Offset for main graphic | 0x00000020
4 bytes  | int32   | Offset for shadow/alphas| 0x0010C8C0, if 0x00000000 then there is no attached graphic data
8 bytes  | Padding | Padding                 | - (possibly used for additional offsets)

```cpp
struct slp_header_v4 {
  char     version[4];
  int16    num_frames;
  int16    type;
  int16    num_directions;
  int16    frames_per_direction;
  int32    palette_id;
  int32    offset_main;
  int32    offset_secondary;
  pad byte padding[8];
};
```
Python format: `Struct("< 4s H H H H i i i 8x")`

There are 4 known versions of SLP: '2.0N', '3.0', '4.0X', and '4.1X'. AoE1,
AoK (including HD), and SWGB all use the '2.0N' version. With AoE1: Definitive
Edition (AoE1 DE), three new versions were created; '3.0' '4.0X', and '4.1X'.
Version 3.0 doesn't seem to be much different from 2.0N SLPs (seems to be used
for rescaled SLPs and terrains), whereas 4.0X and 4.1X SLPs have additional
variables in the header that replace the comment field. '4.1X' was introduced
in a patch for AoE1 DE (probably Update 9). It uses the same structure as '4.0X',
but only seems to be used with decay SLPs. It was likely updated to include the
'decay' type of SLP which apparently darkens bodies on decay.

The Type field designates what type of graphic the SLP is:

Value    | Description
---------|------------------------------------------------------------------------
0x00     | "Normal"
0x01     | "Color Mask" (Possibly unused, there are no examples of it being used)
0x02     | "Shadow Layer"
0x04     | "Outline" (Possibly unused, outlines were never implemented in AoE1 DE)
0x08     | "VFX Color" (Used for effects, like fire or smoke)
0x10     | "VFX Alpha" (Uses "0x08" instead as it's frame type marker when used in attached data)
0x20     | "Decay" (Introduced in 4.1X SLPs)

The `num_directions` and `frames_per_direction` fields are for reference only as
these values are still determined within the .dat file. All official 4.0X SLPs
don't appear to even use these fields correctly, since `num_directions` is always
set to 0x0001 and `frames_per_direction` is always set to the same value as
`num_frames`. In 4.1X, these fields are not used at all.

`palette_id` may also be for reference only and is always unused in 4.0X SLPs.
In 4.1X, an unknown value seems to have replaced it.

`offset_main` points to the location of where frame data begins. This value is
usually set to 0x20 for the third line where frame headers are located. This value
could pontentially be changed to a value such as 0x30 to make room for a comment
field or something.

`offset_secondary` points to the location of where attached data begins. Usually,
this is where shadow data is stored, but is also used to store alpha values to the
VFX type SLPs that use a palette. The attached data essentially works like a SLP
within a SLP as they contain their own frame info headers, edge outline tables,
command offset tables, and draw commands. Shadows in 4.0X SLPs don't use the
shadow draw commands of previous version SLPs.

### SLP Frame info
After the header, there are `num_frames` entries of `slp_frame_info`.
Every `slp_frame_info` stores meta-information about a single frame (texture)
within the SLP.

SLPs with version 4.0X or higher have `num_frames` additional entries of
`slp_frame_info` at `offset_secondary` if the offset is a non-zero value.

Length   | Type   | Description                | Example
---------|--------|----------------------------|--------
4 bytes  | uint32 | Command table offset       | 2464, 0x000009A0
4 bytes  | uint32 | Outline table offset       | 64, 0x00000040
4 bytes  | uint32 | Palette offset (unused)    | 0, 0x00000000
4 bytes  | uint32 | Properties                 | 16, 0x00000010
4 bytes  | int32  | Width of image             | 800, 0x00000320
4 bytes  | int32  | Height of image            | 600, 0x00000258
4 bytes  | int32  | Centre of sprite (X coord) | 0, 0x00000000
4 bytes  | int32  | Centre of sprite (Y coord) | 0, 0x00000000

```cpp
struct slp_frame_info {
  uint32 cmd_table_offset;
  uint32 outline_table_offset;
  uint32 palette_offset;
  uint32 properties;
  int32  width;
  int32  height;
  int32  hotspot_x;
  int32  hotspot_y;
};
```
Python format: `Struct("< I I I I i i i i")`

Following the `slp_frame_info` array is the data for each frame.
`outline_table_offset` points to the position of the `slp_frame_row_edge` array
and `cmd_table_offset` points to the position of the `slp_command_offset` array.
Both of these arrays are of length `height`.

The `height` and `width` values represent the size of the sprite while the
`hotspot_X` and `hotspot_Y` values represent the center coordinates of the sprite.

The `palette_offset` is unused in all games. AoE1 SLPs have written values here,
but they are not read. It seems to be an early remnant of AoE1 development that
was abandoned.

The `properties` field is also an unused field (despite having some values such as
0x10 written there) for AoE1, AoK, and SWGB, but it was later repurposed in AoK HD
and AoE1 DE to include palette IDs for each frame. Values work as following:

**AoK HD Palette IDs:**

Value      | Description
-----------|------------------------------------------------------------------------
0x00       | Default (50500)
0x07       | 32-bit / Or: (value & 7) == 0x07
0x10       | Default (50500)
0x18       | Default (50500)
0x010000   | clf_pal (Cliff)
0x020000   | pal_2 (Oak Trees)
0x030000   | pal_3 (Palm Trees)
0x040000   | pal_4 (Pine Trees)
0x050000   | pal_5 (Snow Trees)
0x060000   | pal_6 (Fire Effects)

**AoE1 DE Palette IDs:**

Value      | Description
-----------|------------------------------------------------------------------------
0x00       | Default (50500)
0x07       | 32-bit / Or: (value & 7) == 0x07
0x010000   | 01_units
0x020000   | 02_nature
0x030000   | 03_buildings_stonetool
0x040000   | 04_buildings_greek
0x050000   | 05_buildings_babylon
0x060000   | 06_buildings_roman
0x070000   | 07_nature_tree_conifer
0x080000   | 08_nature_tree_palms
0x090000   | 09_buildings_asian
0x0A0000   | 10_buildings_egypt
0x360000   | effects
0x08000000 | effects

### SLP Frame row edge
At `outline_table_offset` (after the `slp_frame_info` structs), an array of
  `slp_frame_row_edge` (of length `height`) structs begins.

Length   | Type   | Description   | Example
---------|--------|---------------|--------
2 bytes  | uint16 | Left spacing  | 20, 0x0014
2 bytes  | uint16 | Right spacing | 3, 0x0003

```cpp
struct slp_frame_row_edge {
  uint16 left_space;
  uint16 right_space;
};
```
Python format: `Struct("< H H")`

For every row, `left_space` and `right_space` specify the number of transparent
pixels, from each side to the center. For example, in a 50 pixels wide row, with
a `slp_frame_row_edge` of `{ .left_space = 20, .right_space = 3 }`, the leftmost
20 pixels will be transparent, the rightmost 3 will be transparent and there
will be 27 pixels of graphical data provided through some number of commands.

If the right or left value is `0x8000`, the row is completely transparent.
Note that there are no command bytes for these rows, so will have to be skipped
"manually".

`width - left_space - right_space` = number of pixels in this line.


### SLP command table

After those padding frames, at `slp_frame_info.cmd_table_offset`, an array of
uint32 (of length `height`) begins:

```cpp
struct slp_command_offset {
  uint32 offset;
}
```
Python format: `Struct("< I")`

Each `offset` defines the offset (beginning) of the first command of a row.
The first `offset` in this array is the first drawing command for the image.

These are not actually necessary to use (but obviously are necessary to read),
since the commands can be read sequentially, although they can be used for
validation purposes.

The actual command data starts after the end of the command offsets, or:
`slp_frame_info.cmd_table_offset + 4 * slp_frame_info.height`.


### SLP drawing commands

The image is drawn line by line, a line is finished with the "End of line"
command (0x0F). A command is a one-byte number (`cmd_byte`), followed
by command-specific data with a length (number of pixels) varying
depending on the command. The next command immediately follows the
previous command's data.

Commands can also store meta information like the number of following pixels
in the same byte. More complex commands store the pixel count in the following
byte.

Each command triggers a drawing method for n = "Count" pixels.

All the commands tell you to draw a palette_index, for n pixels.

Commands can be identified by examining the 4 least significant bits of `cmd_byte`.

For examples of drawing commands, see the [Examples](#examples) section.

### Full drawing command list

Value name     | Description
---------------|------------
next           | The byte following `cmd_byte`
`>> 2`         | `cmd_byte >> 2` (i.e., the 6 most significant bits used as data value)
`>> n` or next | `pixel_count = cmd_byte >> n; if pixel_count == 0: pixel_count = next_byte`. i.e., the `8 - n` most significant bits of `cmd_byte` if they are `!= 0`, else the next byte.
`<< 4 + next`  | `((cmd_byte & 0xf0) << 4) + next_byte`

The length of a pixel's `px_color_value` depends on the SLP version and whether
it is a 32-bit SLP.

SLP properties     | Length  | Color value
-------------------|---------|-----------------------------------------------
SLP version <= 4.0 | 1 bytes | 1-byte palette index
SLP version >= 4.1 | 2 bytes | 1-byte display modifier, 1-byte palette index
32-Bit SLP         | 4 bytes | BGRA color

An `X` signifies that the bit can have any value. These bits are often used for
storing the length (pixel count) of the command.

Command Name             | Byte value       | Pixel Count             | Description
-------------------------|------------------|-------------------------|------------
Lesser draw              | `0bXXXXXX00`     | `cmd_byte >> 2`         | An array of length *Count* filled with `px_color_value`.
Lesser skip              | `0bXXXXXX01`     | `cmd_byte >> 2` or next | *Count* transparent pixels should be drawn from the current position.
Greater draw             | `0bXXXX0010`     | `cmd_byte << 4 + next`  | An array of length *Count* filled with `px_color_value`.
Greater skip             | `0bXXXX0011`     | `cmd_byte << 4 + next`  | *Count* transparent pixels should be drawn from the current position.
Player color draw        | `0bXXXX0110`     | `cmd_byte >> 4` or next | An array of length "Count" filled with `px_color_value`. The real palette index is `player_color_palette_index + player * 16`, where `player` is the player ID you're drawing for (1-8).
Fill                     | `0bXXXX0111`     | `cmd_byte >> 4` or next | One `px_color_value` struct follows. This color should be drawn `pixel_count` times from the current position.
Fill player color        | `0bXXXX1010`     | `cmd_byte >> 4` or next | One player palette index byte follows. This color should be drawn `pixel_count` times. See `player_color_list (0x06)`
Shadow draw              | `0bXXXX1011`     | `cmd_byte >> 4` or next | Draw *Count* shadow pixels (The pixels to draw when a unit is behind another object).
Extended command         | `0bXXXX1110`     | depends                 | Get the specific extended command by looking at the most significant bits of the command. (See the table below for details)
End of row               | `0x0F`           | 0                       | End of commands for this row. If more commands follow, they are for the next row.

**Command cases:**

**Lesser Draw (Case 0x00, 0x04, 0x08, 0x0C):**

This case represents a short block of pixels up to a length of 64 and is good for
small chunks of non-repeating pixels. The pixels to copy follows the command byte
and is `length` bytes long.

In SLPs with version 4.0 and below, each of these bytes represents an index within
the color palette.

SLPs with version 4.1 store an additional *display modifier* byte before the
palette index. This modifier defines how long the pixel should be shown on screen
after the animation has started. The display time is `display_modifier / 2` seconds.
After this time has passed, the pixel is drawn transparently.

For 32-bit SLPs, each 4 bytes is read for "length" bytes long in
BGRA order and the alpha value here is always 255.
```
length = command >> 2
(ex: 0x04 = 1, 0x08 = 2, 0x0C = 3, 0x10 = 4, 0x14 = 5, 0x18 = 6, 0x1C = 7, 0x20 = 8, 0x24 = 9)
```

**Lesser Skip (Case 0x01, 0x05, 0x09, 0x0D):**

This case represents a short skip up to a length of 64 pixels. This command is
mainly used for an empty/transparent space within the sprite, as it just moves
the pointer to the drawing buffer forward.
```
length = command >> 2
(ex: 0x05 = 1, 0x09 = 2, 0x0D = 3, 0x11 = 4, 0x15 = 5, 0x19 = 6, 0x1D = 7, 0x21 = 8,  0x25 = 9)
```

**Greater Draw (Case 0x02):**

This case represents a long block of pixels greater than a length of 64 and is
good for big chunks of non-repeating pixels. The pixels to copy follows the
command byte and is "length" bytes long.

In SLPs with version 4.0 and below, each of these bytes represents an index within
the color palette.

SLPs with version 4.1 store an additional *display modifier* byte before the
palette index. This modifier defines how long the pixel should be shown on screen
after the animation has started. The display time is `display_modifier / 2` seconds.
After this time has passed, the pixel is drawn transparently.

For 32-bit SLPs, each 4 bytes is read for "length" bytes long in
BGRA order and the alpha value here is always 255.
```
length = ((command & 0xf0) << 4) + next_byte
(ex: 0x12 + 0x00 = 256, 0x12 + 0x0A = 266, 0x22 + 0x00 = 512)
```

**Greater Skip (Case 0x03):**

This case represents a long skip of pixels greater than 64 pixels.
length = ((command & 0xf0) << 4) + next_byte
```
(ex: 0x13 + 0x00 = 256, 0x13 + 0x0A = 266, 0x23 + 0x00 = 512)
```

**Player Color Draw (Case 0x06):**

This case represents a block of player color pixels that transform based on the
color selected by the player. The pixels to copy follows the command byte and is
`length` bytes long. Each of these bytes represents an index within the player color
range and is different for each game. For AoE1, index values are between 0x00 (brightest)
and 0x09 (darkest). In AoK and SWGB, index values are between 0x00 (darkest) and
0x07 (brightest). And in AoE1 DE, index values are between 0x00 and 0x7F (0-127)
and use separate player color palettes. In-game, the colors are drawn from the start
index in the palette for the player + the index within the player color range. For AoK
and SWGB, blue player colors begin at index 16 in the palette, red at index 32, green
at index 48, yellow at index 64, orange at index 80, cyan/teal at index 96, purple at
index 112, and grey at index 128.
```
length = command >> 4. If 0, the next byte is read and used as the length.
(ex: 0x16 = 1, 0x26 = 2, 0xA6 = 10, 0xF6 = 15, 0x06 + 0x10 = 16)
```

**Fill (Case 0x07):**

This case represents a block of the same color pixels. This command is useful for
shortening the amount of bytes needed for a line that consists of the same exact color
for more than 2 pixels straight and shrink it down to just 2 or 3 bytes (depending on
length). This command does not work in AoE1 and will break terrain SLPs in AoK and
SWGB due to how elevation and blending are generated.

In SLPs with version 4.0 and below, each of these bytes represents an index within
the color palette.

SLPs with version 4.1 store an additional *display modifier* byte before the
palette index. This modifier defines how long the pixel should be shown on screen
after the animation has started. The display time is `display_modifier / 2` seconds.
After this time has passed, the pixel is drawn transparently.

For 32-bit SLPs, each 4 bytes is read for "length" bytes long in
BGRA order and the alpha value here is always 255.
```
length = command >> 4. If 0, the next byte is read and used as the length.
(ex: 0x17 = 1, 0x27 = 2, 0xA7 = 10, 0xF7 = 15, 0x07 + 0x10 = 16)
```

**Fill Player Color (Case 0x0A):**

This case represents a block of the same player color pixels. This command is useful
for shortening the amount of bytes needed for a line that consists of the same exact
player color for more than 2 pixels straight and shrink it down to just 2 or 3 bytes
(depending on length). This command does not work in AoE1.
```
length = command >> 4. If 0, the next byte is read and used as the length.
(ex: 0x1A = 1, 0x2A = 2, 0xAA = 10, 0xFA = 15, 0x0A + 0x10 = 16)
```

**Shadow Draw (Case 0x0B):**

This case represents a block of shadow pixels. The pixels underneath these shadow
pixels are identified within a shadow palette and used to draw into the buffer. The
shadow palette is essentially a darkened variation of the graphic color palette (50500).
It's also used to draw things like the red-tinted checkerboard when placing buildings
at forbidden places.
```
length = command >> 4. If 0, the next byte is read and used as the length.
(ex: 0x1B = 1, 0x2B = 2, 0xAB = 10, 0xFB = 15, 0x0B + 0x10 = 16)
```

**Extended Commands (Case 0x0E):**

Command name        | Byte value | Pixel Count | Description
--------------------|------------|-------------|------------
Forward Draw        | `0x0E`     | 0           | Draw the following command if sprite is not flipped right to left.
Reverse Draw        | `0x1E`     | 0           | Draw following command if this sprite is x-flipped.
Normal Transform    | `0x2E`     | 0           | Set color transform table to normal.
Alternate Transform | `0x3E`     | 0           | Set color transform table to alternate.
Outline 1           | `0x4E`     | 1           | `palette_index = player_index = player * 16`, if obstructed, draw player color, else transparent. This is the player color outline you see when a unit is behind a building. (special color = 1)
Outline 1 Fill      | `0x5E`     | next        | `palette_index = player_index = player * 16`, can be >=1 pixel
Outline 2           | `0x6E`     | 1           | `palette_index = 243`, shield outline. Used in SWGB for when shielded units are hit, a yellow outline is drawn surrounding the unit. (special color = 2)
Outline 2 Fill      | `0x7E`     | next        | `palette_index = 243`, shield outline, can be >=1 pixel.
Dither              | `0x8E`     | ?           | ?
Premultiplied Alpha | `0x9E`     | next        | Draws a line of semi-transparent pixels
Original Alpha      | `0xAE`     | ?           | ?

Forward Draw, Reverse Draw, Normal Transform, and Alternate Transform are all
possibly unused, obsolete, or abandoned commands.

Outline 1 is the outline that surrounds a unit and is seen when a unit is obstructed,
such as when it is behind a building or tree in AoK and SWGB. Its color is determined
by the team color of the player. For AoK, Outline 2 does not have a real purpose. It
may have originally been intended as a black outline when obstructed, but in HD it draws
a player color outline the same as Outline 1. In SWGB, Outline 2 is used for the shield
outline. This outline is the yellow outline that surrounds a unit and is seen when a
shielded unit takes damage. When a unit is behind an object, only the outline of the
corresponding part of the unit is drawn, the rest of the unit is transparent. So, the
left outline is one pixel more to the left than the first color. Usually, these outline
pixels should be stored to a second spritesheet, which is rendered on top of the
tree/building by the fragment shader.

Dither is unknown and was possibly an unused or abandoned command. There are no known
examples of it being used.

Premultiplied Alpha (32-bit Only) draws pixels that contain alpha values lesser than
255. The next pixel after the command determines the `length`. After the `length` byte,
each 4 bytes therafter is read for `length` bytes long in BGRA order to determine the
color. The alpha value is also inverted `(ex: 255 - alpha)`.

Original Alpha is unknown and was possibly abandoned. There are no known examples of
it being used.


For later drawing, a graphics file needs flags for:

* object has outline
* can show objects behind by outline
* both of them

Now the palette indices for all the colors of the unit are known, but a palette
is needed for them to be drawn.

#### Examples

##### Lesser draw and skip

```
Row example: 0x08 0x55 0xF4 0x19 0x28 0x99 0x35 0xF4 0x6D 0x67 0x6E 0xA5 0x01 0x4D 0x8E 0x0F

first cmd_byte = 0x08 = 0b00001000
```

The first `cmd_byte` value has the 2 least significant bits set to `0b00`,
so we know it has to be a *lesser draw*. We can now calculate the pixel
count by shifting the command byte to the right 2 times:

```
pixel_count = cmd_byte >> 2 = 0b00000010 = 0x02 = 2
```

The pixel count is 2 which tells us that an array of 2 indices will follow
`cmd_byte`. Therefore, the bytes `0x55` and `0xF4` belong to the drawing
command.

The next `cmd_byte` is `0xF4`:

```
second cmd_byte = 0x19 = 0b00011001
```

The 2 least significant bits of this command are `0b01`, so this can be
identified as a *lesser skip* command. Here, we also have to calculate
the pixel count by shifting 2 times to the right:

```
pixel_count = cmd_byte >> 2 = 0b00000110 = 0x06 = 6
```

This tells us that 6 transparent pixels have to be drawn. *Lesser skips* do
not reference any other bytes, so the following byte `0x28` is our next
command byte.

```
third cmd_byte = 0x28 = 0b00101000
```

This again is a *lesser draw* command because the 2 least significant bits
are set to `0b00`, albeit with a different pixel count.

```
pixel_count = cmd_byte >> 2 = 0b00001010 = 0x0A = 10
```

This time, the next 10 bytes are palette indices that belong to the drawing
command (`0x99 0x35 0xF4 0x6D 0x67 0x6E 0xA5 0x01 0x4D 0x8E`).

Our next command byte is `0x0F`. This is the *end of row* command which tells
us that the row is finished.


### Secondary Frame info (since version 4.0X)
SLPs with versions of 4.0X or higher store an attached secondary graphic data located
after all the main graphic draw commands. This secondary frame data can be used for
shadow layers, alpha values for 8-bit pixels, and potentially player outlines
(although unused in AoE1 DE). If the main graphic draw commands end before the end
of the line, the rest of the line is padded with null bytes so that the attached
data can begin at a position ending in 0.

Attached data begins with the `secondary_frame_info` array. This array is of size
`num_frames` and contains structures with info for each of it's frames. Each
structure should be 32 bytes in length.

Length   | Type   | Description                | Example
---------|--------|----------------------------|--------
4 bytes  | uint32 | Command table offset       | 43328, 0x0000A940
4 bytes  | uint32 | Outline table offset       | 43072, 0x0000A840
4 bytes  | ?      | Unused                     | 0, 0x00000000
3 bytes  | uint24 | Properties? (Unknown)      | 0, 0x000000 (sometimes 0x010010)
1 byte   | uint8  | Frame Type                 | 8, 0x08 (VFX Alpha)
4 bytes  | int32  | Width of image             | 800, 0x00000320
4 bytes  | int32  | Height of image            | 600, 0x00000258
4 bytes  | int32  | Centre of sprite (X coord) | 0, 0x00000000
4 bytes  | int32  | Centre of sprite (Y coord) | 0, 0x00000000

```cpp
struct secondary_frame_info {
  uint32 sec_cmd_table_offset;
  uint32 sec_outline_table_offset;
  uint32 sec_null;
  uint24 sec_properties;
  uint8  sec_frame_type;
  int32  sec_width;
  int32  sec_height;
  int32  sec_hotspot_x;
  int32  sec_hotspot_y;
};
```
The Frame Type field designates what type of graphic the secondary frame is:

Value    | Description
---------|------------------------------------------------------------------------
0x01     | "Color Mask" (Possibly unused, there are no examples of it being used)
0x02     | "Shadow Layer"
0x04     | "Outline" (Possibly unused, outlines were never implemented in AoE1 DE)
0x08     | "VFX Alpha" (Not 0x10)

Just like with the main graphic data, secondary graphics also contain their own frame
info headers, edge outline tables, command offset tables, and draw commands.

### Secondary Draw Commands (since version 4.0X)
Secondary draw commands use the same command syntax, but only use the *lesser draw*,
*lesser skip* and *fill* commands.

Command Name             | Byte value       | Pixel Count             | Description
-------------------------|------------------|-------------------------|------------
Lesser draw              | `0bXXXXXX00`     | `cmd_byte >> 2`         | An array of length *Count* filled with 1-byte "shadow" values (see below)
Lesser skip              | `0bXXXXXX01`     | `cmd_byte >> 2` or next | *Count* transparent pixels should be drawn from the current position.
Greater draw             | `0bXXXX0010`     | `cmd_byte << 4 + next`  | An array of length *Count* filled with 1-byte "shadow" values follows, 1 value per pixel.
Greater skip             | `0bXXXX0011`     | `cmd_byte << 4 + next`  | *Count* transparent pixels should be drawn from the current position.
Fill                     | `0bXXXX0111`     | `cmd_byte >> 4` or next | One palette index byte follows. This color should be drawn `pixel_count` times from the current position.

For shadows, the values read have to be converted to an alpha mask by left shifting
by 2 and and flipping the bits (i.e. subtracting from 255):

```
shadow_alpha = 255 - (shadow_value << 2)
```

For VFX alphas, the values can be read as-is and are not inverted or altered in any way:


## Palette Files

The drawing palette is stored inside `interfac.drs` (until AoE2: HD), while
AoE1:DE and AoE2:DE store their palettes in separate files with a `.pal` or
`.palx` extension. It's basically an array of `(r, g, b)` tuples.

The palette is a (text-based) JASC Paint Shop Pro file, starting with
`JASC-PAL\r\n`. Read this line at the very start of a .BIN file to see if it's a
palette file. The second line stores version information, should be `0100`. The
third line stores the number of entries, as text.

The rest is that many `r g b\r\n` lines. Again, `r`, `g` and `b` are stored as
text (range `0-255`).

Colors from the palette are referenced in the SLP files with an index.
The index refers to a line in the palette, so line 3=>index 0,
line 4=>index 1 etc.


### Palette files in older versions of AoE1 and AoE2 (up until AoE2:HD)

The file id is `50500+x`, the palettes (color tables) are stored the same way
as SLPs are. (see [DRS Files](drs-files.md)) Here `x` is the palette index,
which should be 0, experiment with `[1,10]`...

`interfac.drs` contains many of these files, but the ingame art uses id 50500.


### Palette files in AoE1:DE

Palettes are stored as plain human-readable palette files with the suffixes
`.pal` or `.palx`. Palette numbers are stored in a `palettes.conf` file that
contains a bunch of lines with assignments in the form of `palette_number,filename`.


## SLP types

### SLP files for moving objects

In Age of Empires 1, Age of Empires 2 and Star Wars: Galactic Battlegrounds,
each animation has 10 keyframes and 5 directions. The other 3 directions are
generated later by flipping the sprite on the y axis.

One SLP stores one animation -> 50 frames per SLP.

Military units have 5 states that have animations:

1. Idle
2. Move
3. Attack
4. Die
5. Decay

Villagers have way more than the 5 states (for the different resources),
boats have 2 separate animations for boat & sails.


### SLP files for static objects

Contain 1 frame for the building. Some static objects have multiple chunks,
like the Town Center, where units can be under one arm and 'in front of' the
main building.


### SLP files for projectiles

Arrows and projectiles have 35 keyframes and 5 directions for their animation
because they need a smoother transition between up- and downwards motion.
-> 175 frames per SLP.


### SLP files for shadows

Every object in game has a shadow. Up until SLP v3.0, moving units store
their shadow in the same frame as the main graphic, but buildings and
other objects have their shadows in separate SLPs.

Since version 4.0, shadows are stored in separate frames in the same SLP file.
