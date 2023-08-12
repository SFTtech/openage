// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma endian little
#pragma pattern_limit 500000

u16 cur_total_draw_length = 0;

// unknown layer tests
u32 entries_size = 0;
u32 cur_offset = 0;
u16 cur_index = 0;
u16 prev_loc = 0;

u16 cur_main_height = 0;
u16 cur_main_height_blocks = 0;

fn pad_length() {
    return (4 - $) & 0b11;
};

struct sld_header {
    char signature[4];
    u16  version;
    u16  num_frames;
    u16  unkown1;
    u16  unkown2;
    u32  unkown3;
} [[static, inline]];

struct sld_frame_header {
    u16 canvas_width;
    u16 canvas_height;
    s16 canvas_hotspot_x;
    s16 canvas_hotspot_y;
    u8  frame_type;
    u8  unknown5;
    u16 frame_index;
} [[static, inline]];

struct sld_graphics_header {
    u16 offset_x1;
    u16 offset_y1;
    u16 offset_x2;
    u16 offset_y2;
    u8  flag1;
    u8  unknown1;
} [[static, inline]];

struct sld_mask_header {
    u8  flag1;
    u8  unknown1;
} [[static, inline]];

struct cmd_pack {
    u8 skip_length;
    u8 draw_length;

    cur_total_draw_length += draw_length;
};

struct sld_command_array {
    cur_total_draw_length = 0;

    u16 len;
    cmd_pack cmds[len];
};

struct bc1_block {
    u16 color0;
    u16 color1;
    u8 pixel_data[4];
} [[static]];

struct bc4_block {
    u8 color0;
    u8 color1;
    u8 pixel_data[6];
} [[static]];

struct sld_layer {
    u32 content_length;
};

struct sld_main_layer : sld_layer {
    sld_graphics_header header;
    sld_command_array cmds;
    bc1_block compr_blocks[cur_total_draw_length];
};

struct sld_shadow_layer : sld_layer {
    sld_graphics_header header;
    sld_command_array cmds;
    bc4_block compr_blocks[cur_total_draw_length];
};

struct unknown_entry {
    u16 entry_loc @ cur_offset + 2 [[hidden]];
    u16 cur_entry_length = entry_loc - prev_loc;
    if (cur_index == cur_main_height_blocks - 1) {
        cur_entry_length = entries_size - prev_loc;
    }

    u8 entry[cur_entry_length];

    cur_offset += 2;
    cur_index += 1;
    prev_loc = entry_loc;
} [[inline]];

struct sld_unknown_layer : sld_layer {
    u16 unknown0;

    cur_offset = $;
    cur_index = 0;
    prev_loc = 0;

    u8 offset_size = cur_main_height_blocks;
    u16 offsets[offset_size];

    entries_size = content_length - sizeof(unknown0) - sizeof(offsets) - 4;

    unknown_entry content[offset_size];
};

struct sld_dmg_mask_layer : sld_layer {
    sld_mask_header header;
    sld_command_array cmds;
    bc1_block compr_blocks[cur_total_draw_length];
};

struct sld_playcol_mask_layer : sld_layer {
    sld_mask_header header;
    sld_command_array cmds;
    bc4_block compr_blocks[cur_total_draw_length];
};

struct sld_frame {
    sld_frame_header header;

    if (header.frame_type & 0x01) {
        sld_main_layer main_layer;

        cur_main_height = main_layer.header.offset_y2 - main_layer.header.offset_y1;
        cur_main_height_blocks = cur_main_height / 4;

        padding[pad_length()];
    }
    if (header.frame_type & 0x02) {
        sld_shadow_layer shadow_layer;

        padding[pad_length()];
    }
    if (header.frame_type & 0x04) {
        sld_unknown_layer unknown_layer;

        padding[pad_length()];
    }
    if (header.frame_type & 0x08) {
        sld_dmg_mask_layer dmg_mask_layer;

        padding[pad_length()];
    }
    if (header.frame_type & 0x10) {
        sld_playcol_mask_layer playcol_mask_layer;

        padding[pad_length()];
    }
};

struct sld_file {
    sld_header header;
    sld_frame frames[header.num_frames];
} [[inline]];

sld_file file @ 0x00;
