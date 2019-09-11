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
2 bytes  | int16   | possibly angles         | 8, 0x08 (only for effects and v4.1 SLPs)
2 bytes  | int16   | unknown                 | always 0x0001
2 bytes  | int16   | Number of frames        | value never differs from previous value
4 bytes  | int32   | possibly checksum field | always 0x00000000 (v4.1 uses it)
4 bytes  | int32   | Offset for main graphic | 0x00000020
4 bytes  | int32   | Offset for shadow       | 0x0010C8C0, if 0x00000000 then there is no shadow stored
8 bytes  | Padding | Padding                 | -

v4.1 SLPs have the same structure as the v4.0 SLPs. However, they are only used for *decay* SLPs.

```cpp
struct slp_header_v4 {
  char  version[4];
  int16 num_frames;
  int16 angles;
  int16 unknown;
  int16 num_frames_alt;
  int32 checksum;
  int32 offset_main;
  int32 offset_shadow;
  padding;
};
```
Python format: `Struct("< 4s H H H H i i i 8x")`

### SLP Frame info
After the header, there are `num_frames` entries of `slp_frame_info`.
Every `slp_frame_info` stores meta-information about a single frame (texture)
within the SLP.

SLPs with version higher than 4.0 have `num_frames` additional entries of
`slp_frame_info` at `offset_shadow` if the offset is a non-zero value.
These frames store the shadows of the game entity.

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

* `palette_offset` is apparently never used.
* `properties` is only used to indicate which palette for the image to use.
  Up to HD, these seem to be the same, so can generally be ignored.
	* 0x10 - "default game palette"
	* 0x00 - "global color palette"
* In AoE1:DE `properties` stores the palette number in the second last 2 bytes
	* 0x0009 - use `09_buildings_asian.pal`
* `hotspot_x` & `hotspot_y` tell the engine where the centre of the unit is.

One image of size `width * height` has `height` rows, of course.


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

An `X` signifies that the bit can have any value. These bts are often used for
storing the length (pixel count) of the command.

Command Name             | Byte value        | Pixel Count    | Description
-------------------------|-------------------|----------------|------------
Lesser draw              | `0bXXXXXX00`     | `cmd_byte >> 2` | An array of length *Count* filled with 1-byte palette indices follows, 1 index per pixel.
Lesser skip              | `0bXXXXXX01`     | `cmd_byte >> 2` or next | *Count* transparent pixels should be drawn from the current position.
Greater draw             | `0bXXXX0010`     | `cmd_byte << 4 + next`  | An array of length *Count* filled with 1-byte palette indices follows, 1 index per pixel.
Greater skip             | `0bXXXX0011`     | `cmd_byte << 4 + next`  | *Count* transparent pixels should be drawn from the current position.
Player color draw        | `0bXXXX0110`     | `cmd_byte >> 4` or next | An array of length "Count" filled with 1-byte player color palette indices follow, 1 index per pixel. The real palette index is `player_color_palette_index + player * 16`, where `player` is the player ID you're drawing for (1-8).
Fill                     | `0bXXXX0111`     | `cmd_byte >> 4` or next | One palette index byte follows. This color should be drawn `pixel_count` times from the current position.
Fill player color        | `0bXXXX1010`     | `cmd_byte >> 4` or next | One player palette index byte follows. This color should be drawn `pixel_count` times. See `player_color_list (0x06)`
Shadow draw              | `0bXXXX1011`     | `cmd_byte >> 4` or next | Draw *Count* shadow pixels (The pixels to draw when a unit is behind another object).
Extended command         | `0bXXXX1110`     | depends        | Get the specific extended command by looking at the most significant bits of the command. (See the table below for details)
End of row              | `0x0F`            | 0              | End of commands for this row. If more commands follow, they are for the next row.

Extended commands, for outlines behind trees/buildings:

Command name        | Byte value  | Pixel Count | Description
--------------------|-------------|-------|------------
render_hint_xflip   | `0x0E`     | 0     | Draw the following command if sprite is not flipped right to left.
render_h_notxflip   | `0x1E`     | 0     | Draw following command if this sprite is x-flipped.
table_use_normal    | `0x2E`     | 0     | Set color transform table to normal.
table_use_alternate | `0x3E`     | 0     | Set color transform table to alternate.
outline_1           | `0x4E`      | 1     | `palette_index = player_index = player * 16`, if obstructed, draw player color, else transparent. This is the player color outline you see when a unit is behind a building. (special color = 1)
outline_span_1      | `0x5E`     | next  | `palette_index = player_index = player * 16`, can be >=1 pixel
outline_2           | `0x6E`      | 1     | `palette_index = 0`, if pixel obstructed, draw a pixel as black outline, else transparent. (special color = 2)
outline_span_2      | `0x7E`     | next  | `palette_index = 0`, same as obstruct_black, >=1 pixel.
dither              | `0x8E`     | ?     | ?
premulti_alpha      | `0x9E`     | ?     | Premultiplied alpha?
orig_alph           | `0xAE`     | ?     | Original alpha?


* "Player color block copy" (`0bXXXX0110`) is an index into the palette, in range 0-15.
* "Shadow" (`0bXXXX1011`) destination "pixels" are used as a lookup into a shadow_table.
  This lookup pixel is then used to draw into the buffer. The shadow table is a
  color-tinted variation of the real color table. It's also used to draw things
  like the red-tinted checkerboard when placing buildings at forbidden places.
* "special color" - 2 means black (enhance the outline, cartoonlike), 1 is
  player color.
* When a unit is behind an object (Commands `0x{4,5,6,7}E` used), only the
  outline of the corresponding part of the unit is drawn, the rest of the unit
  is transparent. So, the left outline is one pixel more to the left than the
  first color.

  Usually, these outline pixels should be stored to a second spritesheet, which is
  rendered on top of the tree/building by the fragment shader.

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

### SLP shadow commands (since version 4.0X)

SLPs with versions higher than 4.0 store their shadows in separate frames, but
use the same command syntax. Shadows are drawn with the *lesser draw*,
*lesser skip* and *fill* commands.

Command Name             | Byte value        | Pixel Count    | Description
-------------------------|-------------------|----------------|------------
Lesser draw              | `0bXXXXXX00`     | `cmd_byte >> 2` | An array of length *Count* filled with 1-byte "shadow" values (see below)
Lesser skip              | `0bXXXXXX01`     | `cmd_byte >> 2` or next | *Count* transparent pixels should be drawn from the current position.
Greater draw             | `0bXXXX0010`     | `cmd_byte << 4 + next`  | An array of length *Count* filled with 1-byte "shadow" values follows, 1 value per pixel.
Greater skip             | `0bXXXX0011`     | `cmd_byte << 4 + next`  | *Count* transparent pixels should be drawn from the current position.
Fill                     | `0bXXXX0111`     | `cmd_byte >> 4` or next | One palette index byte follows. This color should be drawn `pixel_count` times from the current position.

The "shadow" values have to be converted to an alpha mask by left shifting
by 2 and and flipping the bits (i.e. subtracting from 255):

```
shadow_alpha = 255 - (shadow_value << 2)
```

## Palette Files

The drawing palette is stored inside `interfac.drs` (until AoE2: HD), while
AoE1:DE and AoE2:DE store their palettes in separate files with a `.pal` or
`.palx` extension. It's basically an array of `(r, g, b)` tuples.

The file id is `50500+x`, the palettes (color tables) are stored the same way
as SLPs are. (see [DRS Files](drs-files.md)) Here `x` is the palette index,
which should be 0, experiment with `[1,10]`...

`interfac.drs` contains many of these files, but the ingame art uses id 50500.

The palette is a (text-based) JASC Paint Shop Pro file, starting with
`JASC-PAL\r\n`. Read this line at the very start of a .BIN file to see if it's a
palette file. The second line stores version information, should be `0100`. The
third line stores the number of entries, as text.

The rest is that many `r g b\r\n` lines. Again, `r`, `g` and `b` are stored as
text (range `0-255`).

Colors from the palette are referenced in the SLP files with an index.
The index refers to a line in the palette, so line 3=>index 0,
line 4=>index 1 etc.


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
