# FilterMaps File Format

The most annoying `.dat` file.

`FilterMaps.dat` contains 17 rhombus tiles, each mostly 49 rows high,
with each row having a stride of columns as pixels, similar to Blendomatic tiles.
Each pixel contains a reference to an ICM which uses corresponding LightMaps for shading.

``` cpp
struct filter_maps {

	struct filter_map {

		long header_length;                // number of bytes to read this filter map
		long tile_size_y;                  // seems to be 49, 25, or 73

		while (tile_size_y--) {
			uint8 crap1;                   // read 1 byte
			int remaining = crap1 & 0xFF;  // bitwise for reading more crap
			do {
				uint16 crap2;              // read 2 bytes
				int check2 = crap2 & 0x0F; // bitwise for reading further crap
				do {
					uint24 crap3;          // read 3 bytes
					// some of this crap is then loaded into the graphics rendering
					check2--;
				} while (check2);
				check1--;
			} while (check1);
		}
	}
}[17];
```
