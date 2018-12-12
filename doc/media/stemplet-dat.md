# STemplet.dat File Format

Used for elevation tiles.


```
struct templets {
	struct templet {
		uint32_t header_length;         // number of bytes to read this Templet
		uint32_t tile_size_x;           // seems to be always 97
		uint32_t tile_size_y;           // mainly 49, but 25 for shorter tiles, and 73 for longer tiles.
		int32_t hotspot_x;              // rendering offset
		int32_t hotspot_y;
		uint32_t tiles_total;           // used for allocating rhombus tile count in bytes
		uint32_t offset_start1;         // colors1 start from header
		uint32_t offset_start2;         // colors2 start from header
		RGBA colors1[tile_size_x];  // seems to be the color mapping used with ICM
		RGBA colors2[tile_size_y];
	}
}[17];                              // always seems to be 17
```
