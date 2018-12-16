# STemplet.dat File Format

Used for elevation tiles.


```
struct templets {
    struct templet {
        int32_t header_length;         // number of bytes to read this Templet
        int32_t tile_size_x;           // seems to be always 97
        int32_t tile_size_y;           // mainly 49, but 25 for shorter tiles, and 73 for longer tiles.
        int32_t hotspot_x;             // rendering offset
        int32_t hotspot_y;             //
        int32_t tiles_total;           // used for allocating rhombus tile count in bytes
        int32_t offset_start1;         // colors1 start from header
        int32_t offset_start2;         // colors2 start from header

        // Now follows an outline table, exactly how they are found in SLP files.
        struct {
            uint16_t outline_left;     // number of transparent pixels at the start of this row (0x8000 must be interpreted as 0)
            uint16_t outline_right;    // number of transparent pixels at the end of this row (item dito)
        }[tile_size_y];

        // Now follows a command table, exactly how they are found in SLP files.
        // the next tile_size_y ints, (0 <= i < tile_size_y) indicate the offsets where the commands for row i are found,
        // within an imaginary SLP file that could be generated from this data.
        // The offsets would be 0x40 (header) + tile_size_y * 4 (length of outline table) + command offset
        int32_t slp_command_offsets[tile_size_y];

        // Normally, this is where the commands follow in slp files, but these are not included.
        // The game is procedurally generating the missing commands.
        // For example, at offset 0x00419AAB
        // It doesn't make much sense because I don't think it updates the slp_command_offsets, but I may be wrong.
    }[17];                                 // always seems to be 17
};
```
