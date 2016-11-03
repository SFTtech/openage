# STemplet.dat File Format

Used for elevation tiles.


```
struct templets {
	struct templet {
		long header_length;         // number of bytes to read this Templet
		long tile_size_x;           // seems to be always 97
		long tile_size_y;           // mainly 49, but 25 for shorter tiles, and 73 for longer tiles.
		long unknown1;              // seems to be a length used internally
		long unknown2;
		long tiles_total;           // used for allocating rhombus tile count in bytes
		long offset_start1;         // colors1 start from header
		long offset_start2;         // colors2 start from header
		RGBA colors1[tile_size_x];  // seems to be the color mapping used with ICM
		RGBA colors2[tile_size_y];
	}
}[17];                              // always seems to be 17
```
