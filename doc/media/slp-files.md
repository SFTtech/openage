# SLP files

SLP files are stored in the DRS files. They contain all the (animation)
textures. Like the DRS format, it can also be read sequentially.

## SLP file format

#### Header
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

#### SLP Frame info
After the header, there are `num_frames` entries of `slp_frame_info`:
Every `slp_frame_info` stores meta-information about a single frame (texture)
within the SLP.

Length   | Type   | Description                | Example
---------|--------|----------------------------|--------
4 bytes  | uint32 | Command table offset       | 2464, 0x000009A0
4 bytes  | uint32 | Outline table offset       | 64, 0x00000040
4 bytes  | uint32 | Palette offset (unused)    | 0, 0x00000000
4 bytes  | uint32 | Properties (likely unused) | 16, 0x00000010
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
  However, these seem to be the same, so can generally be ignored.
    * 0x10 - "default game palette"
    * 0x00 - "global color palette"
* `hotspot_x` & `hotspot_y` tell the engine where the centre of the unit is.
  
One image of size `width`*`height` has `height` rows, of course.

#### SLP Frame row edge
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
command (0x0F), where a command is a one-byte number (stored in the
least-significant bits of the byte, e.g. `command = data & 0x0F`), followed by
command-specific data with a length varying depending on the command. The next
command immediately follows the previous command's data.

Each command triggers a drawing method for n = "Count" pixels.

All the commands tell you to draw a palette_index, for n pixels.

The *Pixel count* value is stored at these locations: (where `cmd_byte` is the
value of the command byte, and `next_byte` is the value of the next byte)

Value name     | Pixel count
---------------|------------
next           | `next_byte`
`>> 2`         | `cmd_byte >> 2` (i.e., most significant bits used as data value)
`>> n` or next | `pixel_count = cmd_byte >> n; if pixel_count == 0: pixel_count = next_byte`. i.e., the `8 - n` most significant bits of `cmd_byte` if they are `!= 0`, else the next byte.
`<< 4 + next`  | `((cmd_byte & 0xf0) << 4) + next_byte`

Commands where *Count* is `>> i` only reuse the most significant bits of `cmd_byte`.

Command Name             | Byte value | Count          | Description
-------------------------|------------|----------------|------------
Lesser block copy        | `0x00`     | `>> 2`         | An array of length *Count* filled with 1-byte palette indices follows, 1 index per pixel.
Lesser skip              | `0x01`     | `>> 2` or next | *Count* transparent pixels should be drawn from the current position.
Greater block copy       | `0x02`     | `<< 4 + next`  | An array of length *Count* filled with 1-byte palette indices follows, 1 index per pixel.
Greater skip             | `0x03`     | `<< 4 + next`  | *Count* transparent pixels should be drawn from the current position.
Player color block copy | `0x06`     | `>> 4` or next | An array of length "Count" filled with 1-byte player color palette indices follow, 1 index per pixel. The real palette index is `player_color_palette_index + player * 16`, where `player` is the player ID you're drawing for (1-8).
Fill                     | `0x07`     | `>> 4` or next | One palette index byte follows. This color should be drawn `pixel_count` times from the current position.
Fill player color       | `0x0A`     | `>> 4` or next | One player palette index byte follows. This color should be drawn `pixel_count` times. See `player_color_list (0x06)`
Shadow                   | `0x0B`     | `>> 4` or next | Draw *Count* shadow pixels (The pixels to draw when a unit is behind another object).
Extended command         | `0x0E`     | depends        | Get the specific extended command by looking at the most significant bits of the command. (See the table below for details)
End of line              | `0x0F`     | 0              | End of commands for this row. If more commands follow, they are for the next row.

Extended commands, for outlines behind trees/buildings:

Command name        | Byte value | Count | Description
--------------------|------------|-------|------------
render_hint_xflip   | `0x0E`     | 0     | Draw the following command if sprite is not flipped right to left.
render_h_notxflip   | `0x1E`     | 0     | Draw following command if this sprite is x-flipped.
table_use_normal    | `0x2E`     | 0     | Set color transform table to normal.
table_use_alternate | `0x3E`     | 0     | Set color transform table to alternate.
outline_1           | `0x4E`     | 1     | `palette_index = player_index = player * 16`, if obstructed, draw player color, else transparent. This is the player color outline you see when a unit is behind a building. (special color = 1)
outline_span_1      | `0x5E`     | next  | `palette_index = player_index = player * 16`, can be >=1 pixel
outline_2           | `0x6E`     | 1     | `palette_index = 0`, if pixel obstructed, draw a pixel as black outline, else transparent. (special color = 2)
outline_span_2      | `0x7E`     | next  | `palette_index = 0`, same as obstruct_black, >=1 pixel.


* "Lesser block copy" (0x00) also takes the values `0x04, 0x08 & 0x0C`
* "Lesser skip" (0x01) also takes the values `0x05, 0x09, 0x0D`.
* "Player color block copy" (0x06) is an index into the palette, in range 0-15.
* "Shadow" (0x0B) destination "pixels" are used as a lookup into a shadow_table.
  This lookup pixel is then used to draw into the buffer. The shadow table is a
  color-tinted variation of the real color table. It's also used to draw things
  like the red-tinted checkerboard when placing buildings at forbidden places.
* "special color" - 2 means black (enhance the outline, cartoonlike), 1 is
  player color.
* When a unit is behind an object (Commands `0x{4,5,6,7}E` used), only the
  outline of the corresponding part of the unit is drawn, the rest of the unit
  is transparent. So, the left outline is one pixel more to the left than the
  first color.

  Usually, these outline pixels should be stored to a second image, which is
  rendered on top of the tree/building by the fragment shader.

For later drawing, a graphics file needs flags for:

* object has outline
* can show objects behind by outline
* both of them

Now the palette indices for all the colors of the unit are known, but a palette
is needed for them to be drawn.

## Palette Files

The drawing palette is stored inside `interfac.drs`. It's basically an array of
`(r, g, b)` tuples.

The file id is `50500+x`, the palettes (color tables) are stored the same way
as .SLPs are. (see [.DRS Files](drs-files.md)) Here `x` is the palette index,
which should be 0, experiment with `[1,10]`...

`interfac.drs` contains many of these files, but the ingame art uses id 50500.

The palette is a (text-based) JASC Paint Shop Pro file, starting with
`JASC-PAL\r\n`. Read this line at the very start of a .BIN file to see if it's a
palette file. The second line stores version information, should be `0100`. The
third line stores the number of entries, as text.

The rest is that many `r g b\r\n` lines. Again, `r`, `g` and `b` are stored as
text (range `0-255`).

They are referenced in the SLP files, so should be indexed. It's simply an
array, so line 3=>index 0, line 4=>index 1 etc.


## SLP types

### SLP files for moving objects

Unit .SLP files contain 5 states:

1. Attacking
2. Standing Ground
3. Dying
4. Rotting
5. Moving

On average, each state has roughly 50 animation frames -> ~250 textures per unit

For each animation, 5 directions of it are stored, the other 3 directions are
generated later by flipping the sprite on the y axis.

Villagers have way more than the 5 animations (for the different resources),
boats have 2 (boat & sail).


### SLP files for static objects

Contain the image of the building. Some static objects have multiple chunks,
like the Town Center, where units can be under one arm and 'in front of' the
main building.


### SLP files for weapons

Dust, arrows, etc.


### SLP files for shadows

Every object in game has a shadow. Moving units have frames for their shadow in
the same SLP file, but buildings and other objects have their shadows in
separate SLPs.

Large numbers of shadow SLPs are difficult to identify, due to the image only
being in a single color.
