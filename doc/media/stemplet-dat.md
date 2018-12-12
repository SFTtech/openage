# STemplet.dat File Format

Used for elevation tiles.


```
struct templets {
	struct templet {
		int32_t header_length;         // number of bytes to read this Templet
		int32_t tile_size_x;           // seems to be always 97
		int32_t tile_size_y;           // mainly 49, but 25 for shorter tiles, and 73 for longer tiles.
		int32_t hotspot_x;              // rendering offset
		int32_t hotspot_y;
		int32_t tiles_total;           // used for allocating rhombus tile count in bytes
		int32_t offset_start1;         // colors1 start from header
		int32_t offset_start2;         // colors2 start from header
		RGBA colors1[tile_size_x];  // seems to be the color mapping used with ICM
		RGBA colors2[tile_size_y];
	}
}[17];                              // always seems to be 17
```
