# Copyright 2013-2014 the openage authors. See copying.md for legal info.

import os
from struct import Struct, unpack_from
import sys

from ..util import NamedObject
from openage.log import dbg, ifdbg

# SLP files have little endian byte order
endianness = "< "


class SpecialColor:
    """
    special color class to preserve the player number variable.

    also used for storing unit outline colors.
    """

    shadow = NamedObject("%")
    transparent = NamedObject(" ")
    player_color = NamedObject("P")
    black_color = NamedObject("#")

    def __init__(self, special_id, base_color=2):
        """
        base_color: value for the base player color used for outlines.
        2 is lighter and suits better for outline display.
        try to experiment with [0, 7]..
        """

        self.special_id = special_id
        self.base_color = base_color

    def get_pcolor_for_player(self, player):
        """
        determine the player color palette index for a given player.

        the base color of this pixel is stored,
        the remaining degree of freedom is passed as function argument.
        """

        if self.special_id in (2, self.black_color):
            # this ensures palette[16 -16] will be taken
            return -16

        elif self.special_id in (1, self.player_color):
            # return final color for outline or player
            return 16 * player + self.base_color

        else:
            raise Exception("unknown special color")

    def get_pcolor(self):
        """
        look up the special color purpose.

        returns (base_color, is_outline_pixel)
        """

        if self.special_id in (2, self.black_color):
            # black outline pixel, we will probably never encounter this.
            #  -16 ensures palette[16+(-16)=0] will be used.
            return (-16, True)

        elif self.special_id == 1:
            # this is an player-colored outline pixel
            return (self.base_color, True)

        elif self.special_id == self.player_color:
            # this is a playercolor pixel base color
            return (self.base_color, False)

        else:
            raise Exception("unknown special color")

    def __hash__(self):
        return hash((self.special_id, self.base_color))

    def __eq__(self, other):
        return (
            self.special_id == other.special_id
            and self.base_color == other.base_color
        )

    def __repr__(self):
        return "S%s%d" % (self.special_id, self.base_color)


class SLP:
    """
    represents the best image format ever: SLP
    this format is used to store all graphics within AOE.
    """

    # struct slp_header {
    #   char version[4];
    #   int frame_count;
    #   char comment[24];
    # };
    slp_header = Struct(endianness + "4s i 24s")

    # struct slp_frame_info {
    #   unsigned int qdl_table_offset;
    #   unsigned int outline_table_offset;
    #   unsigned int palette_offset;
    #   unsigned int properties;
    #   int          width;
    #   int          height;
    #   int          hotspot_x;
    #   int          hotspot_y;
    # };
    slp_frame_info = Struct(endianness + "I I I I i i i i")

    def __init__(self, data):
        header = SLP.slp_header.unpack_from(data)
        version, frame_count, comment = header

        dbg("SLP header", 2, push="slp")
        dbg("version:     " + version.decode('ascii'))
        dbg("frame count: " + str(frame_count))
        dbg("comment:     " + comment.decode('ascii'))
        dbg("")

        self.frames = list()

        dbg(FrameInfo.repr_header())

        # read all slp_frame_info structs
        for i in range(frame_count):
            frame_header_offset = (SLP.slp_header.size +
                                   i * SLP.slp_frame_info.size)

            frame_info = FrameInfo(*SLP.slp_frame_info.unpack_from(
                data, frame_header_offset
            ))
            dbg(frame_info)
            self.frames.append(SLPFrame(frame_info, data))

        dbg("", pop="slp")

    def __str__(self):
        ret = list()

        ret.extend([repr(self), "\n", FrameInfo.repr_header(), "\n"])
        for frame in self.frames:
            ret.extend([repr(frame), "\n"])
        return "".join(ret)

    def __repr__(self):
        # TODO: lookup the image content description
        return "SLP image<%d frames>" % len(self.frames)


class FrameInfo:
    def __init__(self, qdl_table_offset, outline_table_offset,
                 palette_offset, properties,
                 width, height, hotspot_x, hotspot_y):
        self.qdl_table_offset = qdl_table_offset
        self.outline_table_offset = outline_table_offset
        self.palette_offset = palette_offset
        self.properties = properties  # TODO what are properties good for?
        self.size = (width, height)
        self.hotspot = (hotspot_x, hotspot_y)

    def repr_header():
        return ("offset (qdl table|outline table|palette) |"
                " properties | width x height | hotspot x/y")

    def __repr__(self):
        ret = (
            "        % 9d|" % self.qdl_table_offset,
            "% 13d|" % self.outline_table_offset,
            "% 7d) | " % self.palette_offset,
            "% 10d | " % self.properties,
            "% 5d x% 7d | " % self.size,
            "% 4d /% 5d" % self.hotspot,
        )
        return "".join(ret)


class SLPFrame:
    """
    one image inside the SLP. you can imagine it as a frame of a video.
    """

    # struct slp_frame_row_edge {
    #   unsigned short left_space;
    #   unsigned short right_space;
    # };
    slp_frame_row_edge = Struct(endianness + "H H")

    # struct slp_command_offset {
    #   unsigned int offset;
    # }
    slp_command_offset = Struct(endianness + "I")

    def __init__(self, frame_info, data):
        self.info = frame_info

        # for each row:
        # contains (left, right) number of boundary pixels
        self.boundaries = []
        # stores the file offset for the first drawing command
        self.cmd_offsets = []

        # palette index matrix representing the final image
        self.pcolor = list()

        dbg(push="frame", lvl=3)

        # process bondary table
        for i in range(self.info.size[1]):
            outline_entry_position = (self.info.outline_table_offset + i
                                      * SLPFrame.slp_frame_row_edge.size)

            left, right = SLPFrame.slp_frame_row_edge.unpack_from(
                data, outline_entry_position
            )

            # is this row completely transparent?
            if left == 0x8000 or right == 0x8000:
                # TODO: -1 or like should be enough
                self.boundaries.append(SpecialColor.transparent)
            else:
                self.boundaries.append((left, right))

        dbg("boundary values: %s" % self.boundaries)

        # process cmd table
        for i in range(self.info.size[1]):
            cmd_table_position = (self.info.qdl_table_offset + i
                                  * SLPFrame.slp_command_offset.size)
            cmd_offset, = SLPFrame.slp_command_offset.unpack_from(
                data, cmd_table_position
            )
            self.cmd_offsets.append(cmd_offset)

        dbg("cmd_offsets:     %s" % self.cmd_offsets)

        for i in range(self.info.size[1]):
            self.pcolor.append(self.create_palette_color_row(data, i))

        dbg(lazymsg=lambda: "frame color index data:\n%s" % str(self.pcolor),
            lvl=4)
        dbg(pop="frame")

    def create_palette_color_row(self, data, rowid):
        """
        create palette indices (colors) for the given rowid.
        """

        first_cmd_offset = self.cmd_offsets[rowid]
        bounds = self.boundaries[rowid]
        pixel_count = self.info.size[0]

        # row is completely transparent
        if bounds == SpecialColor.transparent:
            return [bounds] * pixel_count

        left_boundary, right_boundary = bounds
        missing_pixels = pixel_count - left_boundary - right_boundary

        # start drawing the left transparent space
        pcolor_row_beginning = [SpecialColor.transparent] * left_boundary

        # process the drawing commands for this row.
        pcolor_row_content = self.process_drawing_cmds(data, rowid,
                                                       first_cmd_offset,
                                                       missing_pixels)

        # finish by filling up the right transparent space
        pcolor_row_trailing = [SpecialColor.transparent] * right_boundary

        # this is the resulting row data
        row_data = sum((pcolor_row_beginning,
                        pcolor_row_content,
                        pcolor_row_trailing), [])

        # verify size of generated row
        if len(row_data) != pixel_count:
            summary = "%d/%d -> row %d, offset %d / %#x" % (
                got, self.width, rowid, first_cmd_offset, first_cmd_offset)
            txt = "got %%s pixels than expected: %s, missing: %d" % (
                summary, abs(pixel_count - got))

            raise Exception(txt % ("LESS" if got < pixel_count else "MORE"))

        return row_data

    def process_drawing_cmds(self, data, rowid,
                             first_cmd_offset, missing_pixels):
        """
        create palette indices (colors) for the drawing commands
        found for this row in the SLP frame.
        """

        # position in the data blob, we start at the first command of this row
        dpos = first_cmd_offset

        # this array gets filled with palette indices by the cmds
        pcolor_list = []

        # is the end of the current row reached?
        eor = False

        # work through commands till end of row.
        while not eor:
            if len(pcolor_list) > missing_pixels:
                raise Exception("Only %d pixels should be drawn in row %d!" % (
                    missing_pixels, rowid))

            # fetch drawing instruction
            cmd = self.get_byte_at(data, dpos)

            lower_nibble = 0x0f & cmd
            higher_nibble = 0xf0 & cmd
            lower_bits = 0b00000011 & cmd

            dbg(lazymsg=lambda: "opcode: %#x, rowid: %d" % (cmd, rowid), lvl=4)

            if lower_nibble == 0x0f:
                # eol (end of line) command, this row is finished now.

                dbg("end of row reached.", 4)
                eor = True
                continue

            elif lower_bits == 0b00000000:
                # color_list command
                # draw the following bytes as palette colors

                pixel_count = cmd >> 2
                for i in range(pixel_count):
                    dpos += 1
                    color = self.get_byte_at(data, dpos)
                    pcolor_list.append(color)

            elif lower_bits == 0b00000001:
                # skip command
                # draw 'count' transparent pixels
                # count = cmd >> 2; if count == 0: count = nextbyte

                pixel_count, dpos = self.cmd_or_next(cmd, 2, data, dpos)
                pcolor_list += [SpecialColor.transparent] * pixel_count

            elif lower_nibble == 0x02:
                # big_color_list command
                # draw (higher_nibble << 4 + nextbyte) following palette colors

                dpos += 1
                nextbyte = self.get_byte_at(data, dpos)
                pixel_count = (higher_nibble << 4) + nextbyte

                for i in range(pixel_count):
                    dpos += 1
                    color = self.get_byte_at(data, dpos)
                    pcolor_list.append(color)

            elif lower_nibble == 0x03:
                # big_skip command
                # draw (higher_nibble << 4 + nextbyte)
                # transparent pixels

                dpos += 1
                nextbyte = self.get_byte_at(data, dpos)
                pixel_count = (higher_nibble << 4) + nextbyte

                pcolor_list += [SpecialColor.transparent] * pixel_count

            elif lower_nibble == 0x06:
                # player_color_list command
                # we have to draw the player color for cmd>>4 times,
                # or if that is 0, as often as the next byte says.

                pixel_count, dpos = self.cmd_or_next(cmd, 4, data, dpos)
                for i in range(pixel_count):
                    dpos += 1
                    color = self.get_byte_at(data, dpos)

                    # the SpecialColor class preserves the calculation with
                    # player * 16 + color, this is the palette offset
                    # for tinted player colors.
                    entry = SpecialColor(special_id=SpecialColor.player_color,
                                         base_color=color)
                    pcolor_list.append(entry)

            elif lower_nibble == 0x07:
                # fill command
                # draw 'count' pixels with color of next byte

                pixel_count, dpos = self.cmd_or_next(cmd, 4, data, dpos)

                dpos += 1
                color = self.get_byte_at(data, dpos)

                pcolor_list += [color] * pixel_count

            elif lower_nibble == 0x0A:
                # fill player color command
                # draw the player color for 'count' times

                pixel_count, dpos = self.cmd_or_next(cmd, 4, data, dpos)

                dpos += 1
                color = self.get_byte_at(data, dpos)

                # TODO: verify this. might be incorrect.
                # color = ((color & 0b11001100) | 0b00110011)

                # SpecialColor class preserves the calculation of
                # player*16 + color
                entry = SpecialColor(special_id=SpecialColor.player_color,
                                     base_color=color)
                pcolor_list += [entry] * pixel_count

            elif lower_nibble == 0x0B:
                # shadow command
                # draw a transparent shadow pixel for 'count' times

                pixel_count, dpos = self.cmd_or_next(cmd, 4, data, dpos)

                pcolor_list += [SpecialColor.shadow] * pixel_count

            elif lower_nibble == 0x0E:
                # "extended" commands. higher nibble specifies the instruction.

                if higher_nibble == 0x00:
                    # render hint xflip command
                    # render hint: only draw the following command,
                    # if this sprite is not flipped left to right
                    dbg("render hint: xfliptest", 2)

                elif higher_nibble == 0x10:
                    # render h notxflip command
                    # render hint: only draw the following command,
                    # if this sprite IS flipped left to right.
                    dbg("render hint: !xfliptest", 2)

                elif higher_nibble == 0x20:
                    # table use normal command
                    # set the transform color table to normal,
                    # for the standard drawing commands
                    dbg("image wants normal color table now", 2)

                elif higher_nibble == 0x30:
                    # table use alternat command
                    # set the transform color table to alternate,
                    # this affects all following standard commands
                    dbg("image wants alternate color table now", 2)

                elif higher_nibble == 0x40:
                    # outline_1 command
                    # the next pixel shall be drawn as special color 1,
                    # if it is obstructed later in rendering
                    pcolor_list.append(SpecialColor(1))

                elif higher_nibble == 0x60:
                    # outline_2 command
                    # same as above, but special color 2
                    pcolor_list.append(SpecialColor(2))

                elif higher_nibble == 0x50:
                    # outline_span_1 command
                    # same as above, but span special color 1 nextbyte times.

                    dpos += 1
                    pixel_count = self.get_byte_at(data, dpos)

                    pcolor_list += [SpecialColor(1)] * pixel_count

                elif higher_nibble == 0x70:
                    # outline_span_2 command
                    # same as above, using special color 2

                    dpos += 1
                    pixel_count = self.get_byte_at(data, dpos)

                    pcolor_list += [SpecialColor(2)] * pixel_count

            else:
                dbg("stored in this row so far: %s" % pcolor_list, 2)
                raise Exception("wtf! unknown slp drawing command read:"
                                " %#x in row %d" % (cmd, rowid))

            dpos += 1

        # end of row reached, return the created pixel array.
        return pcolor_list

    def get_byte_at(self, data, offset):
        return unpack_from("B", data, offset)[0]

    def cmd_or_next(self, cmd, n, data, pos):
        """
        to save memory, the draw amount may be encoded into
        the drawing command itself in the upper n bits.
        """

        packed_in_cmd = cmd >> n

        if packed_in_cmd != 0:
            return packed_in_cmd, pos

        else:
            pos += 1
            return self.get_byte_at(data, pos), pos

    def draw_pcolor_to_row(self, rowid, color_list, count=1):
        if type(color_list) != list:
            color_list = [color_list] * count

        self.pcolor[rowid] += color_list

    def get_picture_data(self, palette, player_number=0):
        return determine_rgba_matrix(self.pcolor, palette, player_number)

    def __repr__(self):
        return repr(self.info)


def determine_rgba_matrix(image_matrix, palette, player_number=0):
    """
    converts a palette index image matrix to an rgb matrix.
    """

    import numpy

    rgba_data = list()

    for row in image_matrix:
        for pixel in row:
            if type(pixel) == int:
                # simply look up the color index in the table
                r, g, b = palette[pixel]
                color = (r, g, b, 255)

            elif isinstance(pixel, SpecialColor):
                base_pcolor, is_outline = pixel.get_pcolor()
                if is_outline:
                    alpha = 253  # mark this pixel as outline
                else:
                    alpha = 254  # mark this pixel as player color

                # get rgb base color from the color table
                # store it the preview player color
                # in the table: [16*player, 16*player+7]
                r, g, b = palette[base_pcolor + (16 * player_number)]
                color = (r, g, b, alpha)

            elif pixel is SpecialColor.transparent:
                color = (0, 0, 0, 0)

            elif pixel is SpecialColor.shadow:
                color = (0, 0, 0, 100)

            else:
                raise Exception("Unknown color: %s (%s)" % (
                    pixel, type(pixel)))

            rgba_data.append(color)

    output_shape = (len(image_matrix), len(image_matrix[0]), 4)
    array_data = numpy.array(rgba_data, dtype=numpy.uint8)
    return array_data.reshape(*output_shape)
