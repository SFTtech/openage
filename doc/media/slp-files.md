SLP media file specification
============================

.SLP files are stored in the .DRS files.

They contain all the (animation) textures.


SLP files for moving objects
----------------------------

Unit .SLP files contain 5 states:

1. Attacking
1. Standing Ground
1. Dying
1. Rotting
1. Moving

On average, each state has roughly 50 animation frames -> ~250 textures per unit

For each animation, 5 directions of it are stored, the other 3 directions are generated later by flipping the sprite on the y axis.

Villagers have way more than the 5 animations (for different resources), boats have 2 (boat & sail).


SLP files for static objects
----------------------------

Contain the image of the building. Some static objects have multiple chunks, like the Town Center, where units can be under one arm and 'in front of' the main building.


SLP files for weapons
---------------------

Dust, arrows, etc.


SLP files for shadows
---------------------

Every object in game has a shadow. Moving units have frames for their shadow in the same .SLP, but buildings and other objects have their shadows in separate SLPs.

-> large numbers of shadow SLPs unidentifiable.



SLP file format
===============

the slp file starts with a header:

```cpp
struct slp_header {
  char  version[4];
  int   num_frames;
  char  comment[24];
};
```
```python
slp_header = Struct("< 4s i 24s")
```

After the header, there are `num_frames` entries of `slp_frame_info`:

```cpp
struct slp_frame_info {
  unsigned int cmd_table_offset;
  unsigned int outline_table_offset;
  unsigned int palette_offset;
  unsigned int properties; //changes palette to use, but still unknown..
  int          width;
  int          height;
  int          hotspot_x; //center of the unit
  int          hotspot_y; //is referenced as the unit location by the engine
};
```
```python
slp_frame_info = Struct("< I I I I i i i i")
```

Every `slp_frame_info` stores meta-information about a single frame (texture) within the slp.

Palette offset is probably never used.
Properties is more or less unused, but:
*  0x10 means "default game palette",
*  0x00 means "global color palette"

They are probably the same, so you can ignore it.

One image of size `width`×`height` has `height` rows, of course.

At `outline_table_offset` (after the frame_infos), an array of `height` `slp_frame_row_edge` structs begins.

```cpp
struct slp_frame_row_edge {
  unsigned short left_space;
  unsigned short right_space;
};
```
```python
slp_frame_row_edge = Struct("< H H")
```

For every row, `left_space` and `right_space` specify the number of transparent pixels, from each side to the center. For example, in a 50 pixels wide row, with a `slp_frame_row_edge` of `{ .left_space = 20, .right_space = 3 }`, the leftmost 20 pixels will be transparent, and the rightmost 3 will be transparent.

The right or left value is `0x8000`, if the row is completely transparent.
No command bytes will end these transparent rows, you have to skip them later.

`width - left_space - right_space` = number of pixels in this line.

After those padding frames, at `slp_frame_info.cmd_table_offset`, an array of `height` 4byte-ints begins:

```cpp
struct slp_command_offset {
  unsigned int offset;
}
```
```python
slp_command_offset = Struct("< I")
```

Each `offset` defines the offset (beginning) of the first command of a row.
The first `.offset` in this array is offset to the first drawing command for the image.

These are not actually necessary to use, since the commands can be read sequentially.
The command locations can be used for validation purposes though.

The actual command data starts at `slp_frame_info.cmd_table_offset + 4 * slp_frame_info.height`.


SLP drawing commands
====================

The image is drawn line by line, a line is finished with command `eol` (0x0f).

A command is a one-byte number followed by command-specific data.

The command byte can also contain command data. e.g. the `color_list` command (0x00) only checks the least-significant bits for its command type and the rest of the byte is the number of palette indices to follow.
A command is parsed, data values after it are drawn, then the next command byte is reached.

Each commands triggers a drawing method for n=pixel_count pixels.

All the commands tell you to draw a palette_index, for n pixels.

The `pixel_count` value is stored at these locations: (where `cmd_byte` is the value of the command byte, and `next_byte` is the value of the next byte)

Value name     | Pixel Count
---------------|------------
next           | `next_byte`
`>> 2`         | `cmd_byte >> 2` (i.e., most significant bits used as data value)
`>> n` or next | `pixel_count = cmd_byte >> n; if pixel_count == 0: pixel_count = next_byte`. i.e., the `8 - n` most significant bits of `cmd_byte` if they are `!= 0`, else the next byte.
`<< 4 + next`  | `(cmd_byte & 0xf0 << 4) + next_byte`


command_name      | cmd_byte | pixel_count    | Description
------------------|----------|----------------|------------
color_list        | `0x00`   | `>> 2`         | An array of `pixel_count` 1-byte palette indices follows, 1 index per pixel.
skip              | `0x01`   | `>> 2` or next | `pixel_count` transparent pixels should be drawn from the current position.
big_color_list    | `0x02`   | `<< 4 + next`  | An array of `pixel_count` 1-byte palette indices follows, 1 index per pixel.
big_skip          | `0x03`   | `<< 4 + next`  | `pixel_count` transparent pixels should be drawn from the current position.
player_color_list | `0x06`   | `>> 4` or next | `pixel_count` 1-byte player color palette indices follow, 1 index per pixel. The real palette index seems to be `player_color_palette_index + player * 16`, where `player` is the player ID you're drawing for (1-8)
fill              | `0x07`   | `>> 4` or next | One palette index byte follows. This color should be drawn `pixel_count` times from the current position.
fill_player_color | `0x0A`   | `>> 4` or next | One player palette index byte follows. This color should be drawn `pixel_count` times. See `player_color_list (0x06)`
shadow            | `0x0B`   | `>> 4` or next | Draw `pixel_count` shadow pixels (??)
EOL               | `0x0F`   | 0              | End of commands for this row. If more commands follow, they are for the next row.

Extended commands, for outlines behind trees/buildings:

command_name       | cmd_byte | pixel_count    | Description
-------------------|----------|----------------|------------
render_hint_xflip  | `0x0E`   | 0              | draw the following command if sprite is not flipped right to left
render_h_notxflip  | `0x1E`   | 0              | draw following command if this sprite is x-flipped
table_use_normal   | `0x2E`   | 0              | transform color table: for regular `cmd_byte`s → set to normal table
table_use_alternat | `0x3E`   | 0              | transform color table: → set to alternate table
outline_1          | `0x4E`   | 1 (spec_col=1) | `palette_index = player_index = player * 16`, if obstructed, draw player color, else transparent. This is the player color outline you see when a unit is behind a building
outline_2          | `0x6E`   | 1 (spec_col=2) | `palette_index = 0`, if pixel obstructed, draw a pixel as black outline, else transparent
outline_span_2     | `0x5E`   | next           | `palette_index = player_index = player * 16`, can be >=1 pixel
outline_span_2     | `0x7E`   | next           | `palette_index = 0`, same as obstruct_black, >=1 pixel


`spec_col == special_color`: 2 means black (enhance the outline, cartoonlike), 1 is player color.

Note that commands where `pixel_count` is `>> i` reuse only the most significant bits of `cmd_byte`. Check the match for `8 - i` least significant bits only!

`<< 4 + next` also double-uses the more significant bits, but now a 16-bit number is composed:

1. let c be a `cmd_byte` bit. `cmd_byte` = cccccccc;
1. n is a `next_byte` bit. next_byte = nnnnnnnn

Then, `<< 4 + next` = `(cmd_byte & 0xf0) << 4 + next_byte` gives `cccc0000 << 4 + nnnnnnnn` gives `ccccnnnnnnnn`. This number is then used as `pixel_count`.

Bit 'double-using' mask list for all commands:
`r` or `s` means this bit (may) be reused in this command:

    rrrrrr00 == 0x00
    rrrrrr01 == 0x01
    rrrr0010 == 0x02  rrrr << 4
    rrrr0011 == 0x03  rrrr << 4
    ----0100 == 0x04  0x04 and 0x05 was left out, because it would conflict with 0x00 and 0x01 as a prefix
    ----0101 == 0x05   you can read the prefix from right to left, and for 0x00 and 0x04 it would be 0brrrrx100,
    rrrr0110 == 0x06   where x could be 0, which conflicts.
    rrrr0111 == 0x07
    ----1000 == 0x08  = cmd 0x00, draw 2 times. 0x08 and 0x09 are also missing, because of the conflict from above
    ----1001 == 0x09  = cmd 0x01, draw 2 times.
    00001010 == 0x0A
    00001011 == 0x0B
    ----1100 == 0x0C  for the same reason, 0x0C and 0x0D are missing, the prefix woule be 0b0000xx{00,01}
    ----1101 == 0x0D   where xx could be 11 for command 0x00/0x01, that'd conflict with 0x0C/0x0D
    xxxx1110 == 0x0E  xxxx = the extended command identifier
    00001111 == 0x0F

`0x06` comment addition: `player_color` is an index into the palette, in range 0-15.

`0x0A` comment addition:
also called transform_block, may behave different than described above:
```python
color = next
for count:
  draw = ((color & 0xff00ff00) | 0x00ff00ff).
```
0xnn.. are masks used to draw shadow effects, but 0xffffff makes no sense for and and or with a byte.
it's may be used for overlaying the checkerboard shadow sprite onto the current frame.
you'll see that when placing a buildings.

`0x0B` comment addition:
Destination pixels already in the buffer are used as a lookup into a shadow_table. This lookup pixel is then used to draw into the buffer.

Shadow table: color-tinted variation of the real color table.
Also used to draw things like the red-tinted checkerboard when placing buildings at forbidden places.
(drawing real shadows is like drawing black, but with alpha=e.g. 0.5)


Commands `0x{4,5,6,7}E`  indicate that the following command(s) are outlines of the graphic.
this is a potential outline pixel, it's what you see when a unit is behind something.
the engine should only draw them as the player color/black, if the pixel(s) are obstructed.
if they are not obstructed, draw them as transparent.
So the left outline is one pixel more to the left than the first color.

For our purposes, these outline pixels should be stored to a second image, which is rendered
ontop of the tree/building by the fragment shader.

For later drawing, a graphics file needs flags for:

* object has outline
* can show objects behind by outline
* both of them

Now we know the palette indices for all the colors, but we can't draw them yet without a palette.


Palette Files
=============

The drawing palette is stored inside `interfac.drs`. It's basically an array of `(r, g, b)` tuples.

The file id is `50500+x`, the palettes (color tables) are stored the same way as .SLPs are. (see [.DRS Files](drs-files.md)) Here `x` is the palette index, which should be 0, experiment with `[1,10]`...

`interfac.drs` contains many of these files, but the ingame art uses id 50500.

The palette is a JASC Paint Shop Pro file, starting with `"JASC-PAL\r\n"`. Read this line at the very start of a .BIN file to see if it's a palette file.
The second line stores version information, should be `"0100"`.
The third line stores the number of entries, as text. So "256" is stored as 3 bytes, "2", "5" and "6".

The rest is that many "r g b\r\n" lines. Again, `r`, `g` and `b` are stored as text (range `"0"-"255"`)

Index these, they are the references in the .SLP file. It's simply an array, so line 3→index 0, line 4→index 1 etc.

You should now be able to draw stuff :smile:
