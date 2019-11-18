# SMX files

SMX is the sprite storage format of Age of Empires 2: Definitive Edition.
The SMX format is a compressed version of the SMP format that was
used during the Beta of the game. In the release version almost all
sprite media files are compressed SMX files.

It is recommended to read the [SMP documentation](smp-files.md) first
to get a better understanding of the concepts of the format.

## SMX file format

SMX files are basically a trimmed version of SMPs that removes unnecessary
meta information and padding from the file. They also do not define explicit
offsets for each frame and frame header, so the file has to be read sequentially.
Compression is only used for pixel data.

### Header

The SMX file starts with a header:

Length   | Type   | Description                 | Example
---------|--------|-----------------------------|--------
4 bytes  | string | File descriptor             | SMPX
2 bytes  | int16  | probably version            | 2, 0x0002 (for almost all units, some have 0x0001)
2 bytes  | int16  | Number of frames            | 961, 0x03C1
4 bytes  | int32  | File size SMX (this file)   | 2706603, 0x000294CAB (size without header)
4 bytes  | int32  | File size SMP (source file) | 6051456, 0x0005C5680 (size without header)
16 bytes | string | Comment                     | Always empty



```cpp
struct smx_header {
  char  file_descriptor[4];
  int16 version;
  int16 num_frames;
  int32 file_size_comp;
  int32 file_size_uncomp;
  char  comment[16];
};
```
Python format: `Struct("< 4s H H I I 16s")`


### SMX Frame

The frame definitions start directly after the file header. Like in the
SMP format, the frames come in bundles that consist of up to 3 images:

* main sprite
* shadow for that sprite (optional)
* outline (optional)

Which of these images are present in a bundle is determined by the value
`bundle_type` from the bundle header.

#### SMX Bundle header

The bundle header contains 3 values:

Length   | Type   | Description                | Example
---------|--------|----------------------------|--------
1 bytes  | uint8  | Bundle Type                | 3, 0b00000011 (bitfield values)
1 bytes  | uint8  | Palette number             | 21, 0x15
4 bytes  | uint32 | possibly uncompressed size | 6272, 0x1880

```cpp
struct smx_bundle_header {
  uint8 bundle_type;
  uint8 palette_number;
  uint32 ??;
};
```
Python format: `Struct("< B B I")`

`bundle_type` is a bitfield value. This means that every bit set to `1`
in this values tells us something about the bundle.

Bit index | Description
----------|------------
7         | If set to `1`, the bundle contains a main graphic frame
6         | If set to `1`, the bundle contains a shadow frame
5         | If set to `1`, the bundle contains an outline frame
4         | Determines the compression algorithm for the main graphic frame. `0` = 4plus1; `1` = 8to5 (see the [Compression Algorithms](#compression-algorithms) section)
3         | Unknown, but only used by bridges
0-2       | Unused

Example:

```
bundle_type = 0x0B = 0000 1011
```

This bundle would contain a main graphic sprite, a shadow for that sprite
and use the 8to5 compression algorithm for its main graphic pixels.

**Notes**

* SMX files containing bundles with `bundle_type = 0x01` are essentially
broken because the following frame header does not contain correct metadata information.
These files cannot be read sequentially even though they contain valid pixel data.
The uncompressed SMP versions of these files also have incorrect frame headers.

#### SMX Frame header

After the bundle header the frame definitions start. Every frame begins
with a frame header that stores metadata about the frame.

Length   | Type   | Description                | Example
---------|--------|----------------------------|--------
2 bytes  | uint16 | Width of image             | 168, 0x00A8
2 bytes  | uint16 | Height of image            | 145, 0x0091
2 bytes  | uint16 | Centre of sprite (X coord) | 88, 0x0058
2 bytes  | uint16 | Centre of sprite (Y coord) | 99, 0x0063
4 bytes  | uint32 | Length of frame in bytes   | 1848, 0x00000738
4 bytes  | uint32 | Unknown                    | 950, 0x000003B6

```cpp
struct smx_frame_header {
  uint16 width;
  uint16 height;
  uint16 hotspot_x;
  uint16 hotspot_y;
  uint32 frame_len;
  uint32 ??;
};
```
Python format: `Struct("< 4H 2I")`


#### SMX Frame row edge

Directly after the frame header, an array of `smp_frame_row_edge`
(of length `height`) structs begins. These work exactly like the row edges
in the [SMP files](smp-files.md#smp-frame-row-edge).


#### SMX Pixel data

##### Main graphics type

In the SMX format, drawing commands and pixel data for the
main graphic image are stored **in two separate arrays**.

Immediately after the SMX frame row edge definition there are two values
that define the length of these array in bytes:

Length   | Type   | Description                | Example
---------|--------|----------------------------|--------
4 bytes  | uint32 | Command array length       | 354, 0x00000162
4 bytes  | uint32 | Pixel data array length    | 1270, 0x000004F6

The commands are the same as in the SMP files except that their pixel
data has to be read from the pixel data array. Data from the
pixel data array has to be read sequentially, The first draw
command will start reading at index 0 of the pixel data array.
The next draw command will continue reading where the previous command
stopped.

Pixels in the pixel data array are compressed in chunks using one of
the two [compression algorithms](#compression-algorithms).
Each chunk can store information for multiple pixels.

Command Name     | Byte value    | Pixel Count              | Description
-----------------|---------------|--------------------------|------------
Skip             | `0bXXXXXX00`  | `(cmd_byte >> 2) + 1`    | *Count* transparent pixels should be drawn from the current position.
Draw             | `0bXXXXXX01`  | `(cmd_byte >> 2) + 1`    | Read *Count* entries from the pixel data array as normal pixels.
Playercolor Draw | `0bXXXXXX10`  | `(cmd_byte >> 2) + 1`    | Read *Count* entries from the pixel data array as playercolor pixels.
End of Row       | `0bXXXXXX11`  | 0                        | End of commands for this row. If more commands follow, they are for the next row.

##### Shadow type

Unlike the main graphics type frames, the shadow type frames
**only use one array** for drawing commands and pixel data. They can be
read exactly like [SMP shadow frames](#shadow-type) and are
**not compressed**.

However, they still store the length of the unified array immediately
after the SMX frame row edge definitions end:

Length   | Type   | Description                | Example
---------|--------|----------------------------|--------
4 bytes  | uint32 | Unified array length       | 354, 0x00000162

##### Outline type

Outline types also **only use one array** for drawing commands
and pixel data. The information is stored exactly as in the
[SMP outline frames](#outline-type) and **does not use compression**.

TLike the SMX shadow type frames they store the length of the
unified array immediately after the SMX frame row edge definitions end:

Length   | Type   | Description                | Example
---------|--------|----------------------------|--------
4 bytes  | uint32 | Unified array length       | 354, 0x00000162


## Compression Algorithms

Every SMX bundles uses one of two available compression methods
for the main graphic sprite. Each of the compression methods
will store pixel data in chunks of 5 byte which can either contain
2 (8to5 compression) or 4 compressed pixels (4plus1 compression).
These chunks can be read independently.

Both compression methods only remove metadata of the pixels and
have no effect on the RGBA values of the resulting ingame sprites.

### 4plus1 method

The 4plus1 compression method stores the data of 4 pixels in a
5 byte chunk.

Information lost (compared to [SMP pixel](smp-files.md#smp-pixel)):

* `palette_index`: instead stored in SMX Bundle Header
* `px_damage_mask_1`: completely removed
* `px_damage_mask_2`: completely removed

Length   | Type   | Description                  | Example
---------|--------|------------------------------|--------
1 bytes  | uint32 | Palette index `pixel0`       | 43, 0x2B
1 bytes  | uint32 | Palette index `pixel1`       | 43, 0x2B
1 bytes  | uint32 | Palette index `pixel2`       | 43, 0x2B
1 bytes  | uint32 | Palette index `pixel3`       | 43, 0x2B
1 bytes  | uint32 | Palette sections             | 255, 0xFF (bitfield values)

The last byte stores the palette sections as bitfield values.

Bit index | Description
----------|------------
6-7       | Palette section `pixel0`
4-5       | Palette section `pixel1`
2-3       | Palette section `pixel2`
0-1       | Palette section `pixel3`

This compression method is used for anything that does
not require the damage mask values (basically everything that is
not a building).

### 8to5 method

The 8to5 compression method stores the data of 2 pixels in a
5 byte chunk.

Information lost (compared to [SMP pixel](smp-files.md#smp-pixel)):

* `palette_index`: instead stored in SMX Bundle Header

The chunk is basically a giant bitfield, but the values are not that
hard to extract.

We will first show where the values of `pixel0` and `pixel1` are
before discussing the extraction method as it might help with the
understanding of the compression.

Example:

```
Compressed:
Hex:        90 1E 32 73 AA
Bin:        10010000 00011110 00110010 01110011 10101010

Uncompressed:
Hex pixel0: 90 02 30 33
Bin pixel0: 10010000 00000010 00110000 00110011

Hex pixel1: 87 00 90 2A
Bin pixel1: 10000111 00000000 10010000 00101010
```

Value                       | Bit indices
----------------------------|------------
`pixel0` palette index       | 0-7
`pixel0` palette section     | 14-15
`pixel0` damage mask 1       | 16-19
`pixel0` damage mask 2       | 26-31
`pixel1` palette index       | 22-23, 8-13
`pixel1` palette section     | 20-21
`pixel1` damage mask 1       | 38-39,24-25
`pixel1` damage mask 2       | 32-37

While this might look confusing at first, the two pixels can be easily
extracted. If you look closely, you will notice that the bit positions
in the compressed chunk for the values of `pixel0` are at the exact
same positions as in the uncompressed version of the pixel.

```
Compressed:
Bin:        10010000 00011110 00110010 01110011 10101010

Uncompressed:
Bin pixel0: 10010000 XXXXXX10 0011XXXX XX110011

X = irrelevant to pixel0
```

Thus we can extract `pixel0` by bitmasking the first 4 bytes of the
chunk. The mask is `0xFF03F03F`.

```
chunk[0:3] = 90 1E 32 73

pixel0 = chunk[0:3] & 0xFF03F03F = 90 02 30 33

  10010000 00011110 00110010 01110011
& 11111111 00000011 11110000 00111111
-------------------------------------
  10010000 00000010 00110000 00110011 = pixel0
```

For the second pixel, two extra steps are needed. What we have to do
is to take the second and third byte of the chunk and [rotate](https://en.wikipedia.org/wiki/Bitwise_operation#Rotate) this value by 2 to the right. We also do the same
to the fourth and fifth byte of the chunk.

```
rot_0 = chunk[1:2] ROTR 2
      = 00011110 00110010 ROTR 2
      = 10000111 10001100

rot_1 = chunk[3:4] ROTR 2
      = 01110011 10101010 ROTR 2
      = 10011100 11101010
```

If we now append the results, we get a similar situation for `pixel1`
as we had for `pixel0`.


```
[rot_0,rot_1] = 10000111 10001100 10011100 11101010

Uncompressed:
Bin pixel1:   10000111 XXXXXX00 1001XXXX XX101010

X = irrelevant to pixel1
```

As a result, we can apply the mask on these rotated values again
to extract `pixel1`.

```
[rot_0,rot_1] = 10000111 10001100 10011100 11101010 = 87 8C 9C EA

pixel1 = [rot_0,rot_1] & 0xFF03F03F = 87 00 90 2A

  10000111 10001100 10011100 11101010
& 11111111 00000011 11110000 00111111
-------------------------------------
  10000111 00000000 10010000 00101010 = pixel1
```
