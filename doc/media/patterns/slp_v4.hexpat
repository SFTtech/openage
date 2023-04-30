// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma endian little
#pragma pattern_limit 3000000

// Decode the secondary offsets if set to 1
// switch off if you have performance problems
#define DECODE_SECONDARY 0

// only show a specific frame (at the specified index)
// if this value is -1 all frames are shown
#define SHOW_FRAME -1

struct slp_header {
    char version[4];
    s16 num_frames;
    s16 type;
    s16 num_directions;
    s16 frames_per_direction;
    s32 palette_id;
    s32 offset_main;
    s32 offset_secondary;
    padding[8];
};

struct slp_frame_info {
    u32 cmd_table_offset;
    u32 outline_table_offset;
    u32 palette_offset;
    u32 properties;
    s32 width;
    s32 height;
    s32 hotspot_x;
    s32 hotspot_y;
};

enum pixel_type : u8 {
    index,
    index_modifier,
    bgra,
};

u32 offset = 0x00;

char version[4] @ offset;
slp_header header @ offset;

offset += sizeof(header);

slp_frame_info infos[header.num_frames] @ offset;

s32 cur_frame = 0;
s32 cur_height = infos[cur_frame].height;
s32 cur_width = infos[cur_frame].width;
s32 cur_outline_table_offset = infos[cur_frame].outline_table_offset;
s32 cur_cmd_table_offset = infos[cur_frame].cmd_table_offset;
pixel_type cur_pixel_type = pixel_type::index;
if (infos[cur_frame].properties & 0x07)
    cur_pixel_type = pixel_type::bgra;
else if (version == "4.1X")
    cur_pixel_type = pixel_type::index_modifier;

char cur_cmd = 0x00;
char low_crumb = 0;
char low_nibble = 0;
char high_nibble = 0;
u16 cmd_len = 0;
u16 payload_len = 0;

fn next_frame() {
    cur_frame += 1;
    cur_height = infos[cur_frame].height;
    cur_width = infos[cur_frame].width;
    cur_outline_table_offset = infos[cur_frame].outline_table_offset;
    cur_cmd_table_offset = infos[cur_frame].cmd_table_offset;

    if (infos[cur_frame].properties & 0x07)
        cur_pixel_type = pixel_type::bgra;
    else if (version == "4.1X")
        cur_pixel_type = pixel_type::index_modifier;
    else
        cur_pixel_type = pixel_type::index;
};

struct slp_frame_row_edge {
    u16 left_space;
    u16 right_space;
};

struct slp_cmd {
    u8 cmd;

    low_crumb = cmd & 0x03;
    low_nibble = cmd & 0x0f;
    high_nibble = cmd & 0xf0;

    if (low_crumb == 0b00) {
        cmd_len = cmd >> 2;
        payload_len = cmd_len;
    }
    else if (low_crumb == 0b01) {
        cmd_len = cmd >> 2;
        payload_len = 0;
        if (cmd_len == 0) {
            u8 next;
            cmd_len = next;
        }
    }
    else if (low_nibble == 0b0010) {
        cmd_len = cmd << 4;
        u8 next;
        cmd_len += next;
        payload_len = cmd_len;
    }
    else if (low_nibble == 0b0011) {
        cmd_len = cmd << 4;
        u8 next;
        cmd_len += next;
        payload_len = 0;
    }
    else if (low_nibble == 0b0110) {
        cmd_len = cmd >> 4;
        payload_len = cmd_len;
        if (cmd_len == 0) {
            u8 next;
            cmd_len = next;
            payload_len = cmd_len;
        }
    }
    else if (low_nibble == 0b0111) {
        cmd_len = cmd >> 4;
        payload_len = 1;
        if (cmd_len == 0) {
            u8 next;
            cmd_len = next;
        }
    }
    else if (low_nibble == 0b1010) {
        cmd_len = cmd >> 4;
        payload_len = 1;
        if (cmd_len == 0) {
            u8 next;
            cmd_len = next;
        }
    }
    else if (low_nibble == 0b1011) {
        cmd_len = cmd >> 4;
        payload_len = 0;
        if (cmd_len == 0) {
            u8 next;
            cmd_len = next;
        }
    }
    else if (cmd == 0x5E) {
        u8 next;
        cmd_len = next;
        payload_len = 0;
    }
    else if (cmd == 0x7E) {
        u8 next;
        cmd_len = next;
        payload_len = 0;
    }
    else if (cmd == 0x9E) {
        u8 next;
        cmd_len = next;
        payload_len = 0;
    }
    else {
        cmd_len = 0;
        payload_len = 0;
    }

    if (cur_pixel_type == pixel_type::index_modifier)
        u16 payload[payload_len];
    else if (cur_pixel_type == pixel_type::bgra)
        u32 payload[payload_len];
    else
        u8 payload[payload_len];

    cur_cmd = cmd;
};

struct slp_cmd_row {
    slp_cmd commands[while(cur_cmd != 0x0F)];

    cur_cmd = 0x00;
};

struct slp_cmd_table {
    slp_cmd_row rows[cur_height];
};

struct slp_frame {
    $ = cur_outline_table_offset;
    slp_frame_row_edge outline_offsets[cur_height];

    $ = cur_cmd_table_offset;
    u32 cmd_offsets[cur_height];

    $ = cmd_offsets[0];
    slp_cmd_table table;

    if (cur_frame < header.num_frames - 1)
        next_frame();

    if (SHOW_FRAME >= 0 && cur_frame - 1 != SHOW_FRAME)
        continue;
};

offset += sizeof(infos);

slp_frame frames[header.num_frames] @ offset;

offset = header.offset_secondary;

if (offset == 0x00 || !DECODE_SECONDARY)
    return;

slp_frame_info infos_secondary[header.num_frames] @ offset;

cur_frame = 0;
cur_height = infos_secondary[cur_frame].height;
cur_width = infos_secondary[cur_frame].width;
cur_outline_table_offset = infos_secondary[cur_frame].outline_table_offset;
cur_cmd_table_offset = infos_secondary[cur_frame].cmd_table_offset;
cur_pixel_type = pixel_type::index;

fn next_frame_secondary() {
    cur_frame += 1;
    cur_height = infos_secondary[cur_frame].height;
    cur_width = infos_secondary[cur_frame].width;
    cur_outline_table_offset = infos_secondary[cur_frame].outline_table_offset;
    cur_cmd_table_offset = infos_secondary[cur_frame].cmd_table_offset;
};

struct slp_frame_secondary {
    $ = cur_outline_table_offset;
    slp_frame_row_edge outline_offsets[cur_height];

    $ = cur_cmd_table_offset;
    u32 cmd_offsets[cur_height];
    slp_cmd_table table;

    if (cur_frame < header.num_frames - 1)
        next_frame_secondary();

    if (SHOW_FRAME >= 0 && cur_frame - 1 != SHOW_FRAME)
        continue;
};

offset += sizeof(infos_secondary);

slp_frame_secondary frames_secondary[header.num_frames] @ offset;
