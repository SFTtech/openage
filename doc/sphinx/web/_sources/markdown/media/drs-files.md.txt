# DRS files

## DRS format

*drs* files are archives containing other files, like a tar archive, but much
simpler. Helpfully, the format is sequential in nature, which helps speed up
decoding time.

### DRS Header

A DRS file starts with its main header:

Length   | Type   | Description        | Example
---------|--------|--------------------|--------
40 bytes | string | Copyright info     | Copyright (c) 1997 Ensemble Studios\032.
4 bytes  | string | File version       | 1.00
12 bytes | string | File type          | tribe
4 bytes  | int32  | Number of tables   | 4, 0x00000004
4 bytes  | int32  | Offset of 1st file | 2188, 0x0000088C

```cpp
struct drs_header {
	char copyright[40];
	char version[4];
	char ftype[12];
	int32 table_count;
	int32 file_offset;
};
```
Python format: `Struct("< 40s 4s 12s i i")`

* It is important to note that in later versions of the DRS format
  (used for SW:GB), it appears that `copyright` has a length of 60 instead of
  the usual 40.
* `copyright`, `version` & `ftype` seem to always have the same value.
* `table_count` stores how many `drs_table_info` structs will follow the main header.
* `file_offset` is the offset of the first actual file in the DRS.

### DRS Table info

One `drs_table_info` struct is stored for each file type (Possible file
extensions are `bina`, `slp `, `wav `).

Length  | Type   | Description            | Example
--------|--------|------------------------|--------
4 bytes | string | File extension         | 'anib', ' pls'
4 bytes | int32  | Table offset           | 112, 0x00000070
4 bytes | int32  | Number of files        | 71, 0x00000047

```cpp
struct drs_table_info {
	char file_extension[4];
	int32 file_info_offset;
	int32 num_files;
};
```
Python format: `Struct("< 4s i i")`

* `file_extension` is reversed and padded by spaces (`0x20`) to reach 4 bytes.
  Internally, Age of Empires does something like this when reading files
  from a DRS archive:

  ```c
  read_drs_file('bina', 50500);
  ```

  Note how it uses a C char with multiple bytes. This is stored on disk as a
  little-endian integer, so the 'a' byte ends up at the lowest address,
  and the 'b' byte ends up at the highest address.
* `file_info_offset` holds the offset where the actual table resides.
* `num_files` contains the amount of files in the table.

The `drs_table_info` tells us, how many files are stored for the `file_type`.

### DRS File info

Each table contains entries for the files in it. The table entry
consists of: the unique integer identifier of the embedded file,
the offset of the embedded file and the size of the embedded file.

`drs_file_info` tables start at position
`drs_table_info->file_info_offset` for the corresponding table:

Length  | Type  | Description | Example
--------|-------|-------------|--------
4 bytes | int32 | File id     | 50001, 0x0000C351
4 bytes | int32 | File offset | 2188, 0x0000088C
4 bytes | int32 | File size   | 625, 0x00000271

```cpp
struct drs_file_info {
	int32 file_id;
	int32 file_data_offset;
	int32 file_size;
};
```
Python format: `Struct("< i i i")`

For every file, the offset in the `.drs` is stored, and also its size.
The file's ID can be assumed to be its name.


## Files

DRS files that come with the base AoK game.

### gamedata.drs

Contains random map scripts (RMS), as well as AI scripts and some graphic files.
Random Map scripts describe how the builtin maps should be generated.
AI scripts tell the computer what to do, such as build units, gather resources,
and generally make it "smart". These are all `bina` files.
The graphic files (`slp`) are described below.

### graphics.drs

Contains almost every graphic you see in the game. This includes units,
buildings, resources, animals, cliffs and shadows. These are all `slp` graphic
(animation) files.

### interfac.drs

Contains the main interface graphics of the game.
All of the the borders, buttons, logos, main game screen, etc you see when
entering the game to starting one can be found within this file as `slp`
graphics. The drawing color palettes are also stored in this file as `bina`.
Finally there's a bunch of `wav` sounds, too.

### sounds.drs

Contains every sound in Age of Kings. These are all `wav` files.

### terrain.drs

Contains all the terrain, in diamond shaped pieces. For each terrain, there are
100 pieces. These are all `slp` files.

## References

* [Age of Empires .drs format](http://artho.com/age/drs.html)
