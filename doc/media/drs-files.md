About .drs files
===============

*drs* files are archives containing other files, like a tar archive, but much simpler.
They can only store data for a **file id** and a corresponding **file name extension**.

gamedata.drs
------------

Contains random map scripts (RMS), as well as AI scripts.
Random Map scripts describe how the builtin maps should be generated.
AI scripts tell the computer what to do, such as build units, gather resources, and generally make it "smart".
These are all `bin` files.


graphics.drs
------------

Contains almost every graphic you see in the game. This includes units, buildings, resources, animals, cliffs and shadows.
These are all `slp` graphic (animation) files.


interfac.drs
------------

Contains the main interface graphics of the game.
All of the the borders, buttons, logos, main game screen, etc you see when entering the game to starting one can be found within this file as `slp`s.
The drawing color palettes are also stored in this file as `bin`s.
Finally there's a bunch of `wav` sounds, too.


sounds.drs
----------

Contains every sound in Age of Kings; from the war horn sounding off to the ringing town bell, it's all stored here.
These are all `wav` files.


terrain.drs
-----------

contains all the terrain, in diamond shaped pieces. For each terrain, there are 100 pieces.
These are all `slp` files.



DRS format
==========

DRS files contain search structures for archiving the media files.

A DRS file starts with its main header:

```cpp
const int copyright_size = 40; // for age2:aoc this is 40.
struct drs_header {
  char copyright[copyright_size];
  char version[4];
  char ftype[12];
  int table_count;
  int file_offset; //offset of first file
};
```
```python
drs_header = Struct("< " + str(copyright_size) + "s 4s 12s i i")
```

At the very start is a 40-byte Copyright notice (`Copyright (c) 1997 Ensemble Studios.`), followed by a 4-byte version identifier (`1.00`, note that it's a char sequence and not a number). Then, a 12-byte 'file type', which seems to always be `tribe`.
`table_count` stores how many `drs_table_info` structs will follow the main header.
`file_offset` is the offset of the first actual file in the DRS.

One `drs_table_info` struct is stored for each file type (means: animation, ai script, whatever. Possible file extensions are `bin`, `slp`, `wav`)

```cpp
struct drs_table_info {
  char file_type;
  char file_extension[3]; //reversed extension
  int file_info_offset;   //table offset
  int num_files;          //number of files in table
};
```
```python
drs_table_info = Struct("< c 3s i i")
```

The `file_type` is one magical byte that seems to be either `0x61` when the file extension is `bin`, or `0x20` otherwise.
`file_extension` is the reversed file extension, so one of `nib`, `pls`, `vaw`. (Obviously, reverse to get `bin`, `slp`, `wav`)
`file_info_offset` holds the offset where the actual table resides.
`num_files` contains the amount of files in the table.

The `drs_table_info` tells us, how many files are stored for the `file_type`.

For each file, there is a `drs_file_info` entry.
`drs_file_info` tables start at position `drs_table_info->file_info_offset` for the corresponding table:

```cpp
struct drs_file_info {
  int file_id;
  int file_data_offset;
  int file_size;
};
```
```python
drs_file_info = Struct("< i i i")
```


For every file, the offset in the `.drs` is stored, and also its size.
The unique id is as close to a filename as it gets.
