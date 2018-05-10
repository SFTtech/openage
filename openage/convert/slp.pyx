# Copyright 2013-2017 the openage authors. See copying.md for legal info.
#
# cython: profile=False

from struct import Struct, unpack_from

from enum import Enum

cimport cython
import numpy
cimport numpy

from libc.stdint cimport uint8_t
from libcpp cimport bool
from libcpp.vector cimport vector

from ..log import spam, dbg


# SLP files have little endian byte order
endianness = "< "


class SpecialColorValue(Enum):
    shadow = "%"
    transparent = " "
    player_color = "P"
    black_color = "#"

    def __str__(self):
        return self.value

    def __repr__(self):
        return self.value


# command ids may have encoded the pixel length.
# this is used when unpacked.
cdef struct cmd_pack:
    uint8_t count
    Py_ssize_t dpos


cdef struct boundary_def:
    Py_ssize_t left
    Py_ssize_t right
    bool full_row


# SLP pixels can be very special.
cdef enum pixel_type:
    color_standard       # standard pixel
    color_shadow         # shadow pixel
    color_transparent    # transparent pixel
    color_player         # non-outline player color pixel
    color_black          # black outline pixel
    color_special_1      # player color outline pixel
    color_special_2      # black outline pixel


# One SLP pixel.
cdef struct pixel:
    pixel_type type
    uint8_t value


class SLP:
    """
    Class for reading/converting the greatest image format ever: SLP.
    This format is used to store all graphics within AOE.
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

        dbg("SLP")
        dbg(" version:     " + version.decode('ascii'))
        dbg(" frame count: " + str(frame_count))
        dbg(" comment:     " + comment.decode('ascii'))

        self.frames = list()

        spam(FrameInfo.repr_header())

        # read all slp_frame_info structs
        for i in range(frame_count):
            frame_header_offset = (SLP.slp_header.size +
                                   i * SLP.slp_frame_info.size)

            frame_info = FrameInfo(*SLP.slp_frame_info.unpack_from(
                data, frame_header_offset
            ))
            spam(frame_info)
            self.frames.append(SLPFrame(frame_info, data))

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

    @staticmethod
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


cdef class SLPFrame:
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

    # frame information
    cdef object info

    # for each row:
    # contains (left, right, full_row) number of boundary pixels
    cdef vector[boundary_def] boundaries

    # stores the file offset for the first drawing command
    cdef vector[int] cmd_offsets

    # palette index matrix representing the final image
    cdef vector[vector[pixel]] pcolor

    # memory pointer
    cdef const uint8_t *data_raw

    def __init__(self, frame_info, data):
        self.info = frame_info

        if not (isinstance(data, bytes) or isinstance(data, bytearray)):
            raise ValueError("Frame data must be some bytes object")

        # convert the bytes obj to char*
        self.data_raw = data

        cdef size_t i
        cdef int cmd_offset

        cdef size_t row_count = self.info.size[1]

        # process bondary table
        for i in range(row_count):
            outline_entry_position = (self.info.outline_table_offset + i *
                                      SLPFrame.slp_frame_row_edge.size)

            left, right = SLPFrame.slp_frame_row_edge.unpack_from(
                data, outline_entry_position
            )

            # is this row completely transparent?
            if left == 0x8000 or right == 0x8000:
                self.boundaries.push_back(boundary_def(0, 0, True))
            else:
                self.boundaries.push_back(boundary_def(left, right, False))

        # process cmd table
        for i in range(row_count):
            cmd_table_position = (self.info.qdl_table_offset + i *
                                  SLPFrame.slp_command_offset.size)
            cmd_offset = SLPFrame.slp_command_offset.unpack_from(
                data, cmd_table_position
            )[0]
            self.cmd_offsets.push_back(cmd_offset)

        for i in range(row_count):
            self.pcolor.push_back(self.create_palette_color_row(i))

    cdef vector[pixel] create_palette_color_row(self, Py_ssize_t rowid) except +:
        """
        create palette indices (colors) for the given rowid.
        """

        cdef vector[pixel] row_data
        cdef Py_ssize_t i

        first_cmd_offset = self.cmd_offsets[rowid]
        cdef boundary_def bounds = self.boundaries[rowid]
        cdef size_t pixel_count = self.info.size[0]

        # preallocate memory
        row_data.reserve(pixel_count)

        # row is completely transparent
        if bounds.full_row:
            for _ in range(pixel_count):
                row_data.push_back(pixel(color_transparent, 0))

            return row_data

        # start drawing the left transparent space
        for i in range(bounds.left):
            row_data.push_back(pixel(color_transparent, 0))

        # process the drawing commands for this row.
        self.process_drawing_cmds(row_data, rowid,
                                  first_cmd_offset,
                                  pixel_count - bounds.right)

        # finish by filling up the right transparent space
        for i in range(bounds.right):
            row_data.push_back(pixel(color_transparent, 0))

        # verify size of generated row
        if row_data.size() != pixel_count:
            got = row_data.size()
            summary = "%d/%d -> row %d, offset %d / %#x" % (
                got, pixel_count, rowid, first_cmd_offset, first_cmd_offset)
            txt = "got %%s pixels than expected: %s, missing: %d" % (
                summary, abs(pixel_count - got))

            raise Exception(txt % ("LESS" if got < pixel_count else "MORE"))

        return row_data

    cdef process_drawing_cmds(self, vector[pixel] &row_data,
                              Py_ssize_t rowid,
                              Py_ssize_t first_cmd_offset,
                              size_t expected_size):
        """
        create palette indices (colors) for the drawing commands
        found for this row in the SLP frame.
        """

        # position in the data blob, we start at the first command of this row
        cdef Py_ssize_t dpos = first_cmd_offset

        # is the end of the current row reached?
        cdef bool eor = False

        cdef uint8_t cmd
        cdef uint8_t nextbyte
        cdef uint8_t lower_nibble
        cdef uint8_t higher_nibble
        cdef uint8_t lower_bits
        cdef cmd_pack cpack
        cdef int pixel_count

        # work through commands till end of row.
        while not eor:
            if row_data.size() > expected_size:
                raise Exception(
                    "Only %d pixels should be drawn in row %d, "
                    "but we have %d already!" % (
                        expected_size, rowid, row_data.size()
                    )
                )

            # fetch drawing instruction
            cmd = self.get_byte_at(dpos)

            lower_nibble = 0x0f & cmd
            higher_nibble = 0xf0 & cmd
            lower_bits = 0b00000011 & cmd

            # opcode: cmd, rowid: rowid

            if lower_nibble == 0x0f:
                # eol (end of line) command, this row is finished now.
                eor = True
                continue

            elif lower_bits == 0b00000000:
                # color_list command
                # draw the following bytes as palette colors

                pixel_count = cmd >> 2
                for _ in range(pixel_count):
                    dpos += 1
                    color = self.get_byte_at(dpos)
                    row_data.push_back(pixel(color_standard, color))

            elif lower_bits == 0b00000001:
                # skip command
                # draw 'count' transparent pixels
                # count = cmd >> 2; if count == 0: count = nextbyte

                cpack = self.cmd_or_next(cmd, 2, dpos)
                dpos = cpack.dpos
                for _ in range(cpack.count):
                    row_data.push_back(pixel(color_transparent, 0))

            elif lower_nibble == 0x02:
                # big_color_list command
                # draw (higher_nibble << 4 + nextbyte) following palette colors

                dpos += 1
                nextbyte = self.get_byte_at(dpos)
                pixel_count = (higher_nibble << 4) + nextbyte

                for _ in range(pixel_count):
                    dpos += 1
                    color = self.get_byte_at(dpos)
                    row_data.push_back(pixel(color_standard, color))

            elif lower_nibble == 0x03:
                # big_skip command
                # draw (higher_nibble << 4 + nextbyte)
                # transparent pixels

                dpos += 1
                nextbyte = self.get_byte_at(dpos)
                pixel_count = (higher_nibble << 4) + nextbyte

                for _ in range(pixel_count):
                    row_data.push_back(pixel(color_transparent, 0))

            elif lower_nibble == 0x06:
                # player_color_list command
                # we have to draw the player color for cmd>>4 times,
                # or if that is 0, as often as the next byte says.

                cpack = self.cmd_or_next(cmd, 4, dpos)
                dpos = cpack.dpos
                for _ in range(cpack.count):
                    dpos += 1
                    color = self.get_byte_at(dpos)

                    # the SpecialColor class preserves the calculation with
                    # player * 16 + color, this is the palette offset
                    # for tinted player colors.
                    row_data.push_back(pixel(color_player, color))

            elif lower_nibble == 0x07:
                # fill command
                # draw 'count' pixels with color of next byte

                cpack = self.cmd_or_next(cmd, 4, dpos)
                dpos = cpack.dpos

                dpos += 1
                color = self.get_byte_at(dpos)

                for _ in range(cpack.count):
                    row_data.push_back(pixel(color_standard, color))

            elif lower_nibble == 0x0A:
                # fill player color command
                # draw the player color for 'count' times

                cpack = self.cmd_or_next(cmd, 4, dpos)
                dpos = cpack.dpos

                dpos += 1
                color = self.get_byte_at(dpos)

                # TODO: verify this. might be incorrect.
                # color = ((color & 0b11001100) | 0b00110011)

                # SpecialColor class preserves the calculation of
                # player*16 + color
                for _ in range(cpack.count):
                    row_data.push_back(pixel(color_player, color))

            elif lower_nibble == 0x0B:
                # shadow command
                # draw a transparent shadow pixel for 'count' times

                cpack = self.cmd_or_next(cmd, 4, dpos)
                dpos = cpack.dpos

                for _ in range(cpack.count):
                    row_data.push_back(pixel(color_shadow, 0))

            elif lower_nibble == 0x0E:
                # "extended" commands. higher nibble specifies the instruction.

                if higher_nibble == 0x00:
                    # render hint xflip command
                    # render hint: only draw the following command,
                    # if this sprite is not flipped left to right
                    spam("render hint: xfliptest")

                elif higher_nibble == 0x10:
                    # render h notxflip command
                    # render hint: only draw the following command,
                    # if this sprite IS flipped left to right.
                    spam("render hint: !xfliptest")

                elif higher_nibble == 0x20:
                    # table use normal command
                    # set the transform color table to normal,
                    # for the standard drawing commands
                    spam("image wants normal color table now")

                elif higher_nibble == 0x30:
                    # table use alternat command
                    # set the transform color table to alternate,
                    # this affects all following standard commands
                    spam("image wants alternate color table now")

                elif higher_nibble == 0x40:
                    # outline_1 command
                    # the next pixel shall be drawn as special color 1,
                    # if it is obstructed later in rendering
                    row_data.push_back(pixel(color_special_1, 0))

                elif higher_nibble == 0x60:
                    # outline_2 command
                    # same as above, but special color 2
                    row_data.push_back(pixel(color_special_2, 0))

                elif higher_nibble == 0x50:
                    # outline_span_1 command
                    # same as above, but span special color 1 nextbyte times.

                    dpos += 1
                    pixel_count = self.get_byte_at(dpos)

                    for _ in range(pixel_count):
                        row_data.push_back(pixel(color_special_1, 0))

                elif higher_nibble == 0x70:
                    # outline_span_2 command
                    # same as above, using special color 2

                    dpos += 1
                    pixel_count = self.get_byte_at(dpos)

                    for _ in range(pixel_count):
                        row_data.push_back(pixel(color_special_2, 0))

            else:
                raise Exception(
                    "unknown slp drawing command: " +
                    "%#x in row %d" % (cmd, rowid))

            dpos += 1

        # end of row reached, return the created pixel array.
        return

    cdef inline uint8_t get_byte_at(self, Py_ssize_t offset):
        """
        Fetch a byte from the slp.
        """
        return self.data_raw[offset]

    cdef inline cmd_pack cmd_or_next(self, uint8_t cmd,
                                     uint8_t n, Py_ssize_t pos):
        """
        to save memory, the draw amount may be encoded into
        the drawing command itself in the upper n bits.
        """

        cdef uint8_t packed_in_cmd = cmd >> n

        if packed_in_cmd != 0:
            return cmd_pack(packed_in_cmd, pos)

        else:
            pos += 1
            return cmd_pack(self.get_byte_at(pos), pos)

    def get_picture_data(self, palette, player_number=0):
        """
        Convert the palette index matrix to a colored image.
        """
        return determine_rgba_matrix(self.pcolor, palette, player_number)

    def get_hotspot(self):
        """
        Return the frame's hotspot (the "center" of the image)
        """
        return self.info.hotspot

    def __repr__(self):
        return repr(self.info)


@cython.boundscheck(False)
@cython.wraparound(False)
cdef numpy.ndarray determine_rgba_matrix(vector[vector[pixel]] &image_matrix,
                                         palette, int player_number=0):
    """
    converts a palette index image matrix to an rgba matrix.
    """
    cdef size_t height = image_matrix.size()
    cdef size_t width = image_matrix[0].size()

    cdef numpy.ndarray[numpy.uint8_t, ndim=3] array_data = \
        numpy.zeros((height, width, 4), dtype=numpy.uint8)

    # micro optimization to avoid call to ColorTable.__getitem__()
    cdef list lookup = palette.palette

    cdef uint8_t r
    cdef uint8_t g
    cdef uint8_t b
    cdef uint8_t a

    cdef vector[pixel] current_row
    cdef pixel px
    cdef pixel_type px_type
    cdef int px_val

    cdef size_t x
    cdef size_t y

    for y in range(height):

        current_row = image_matrix[y]

        for x in range(width):
            px = current_row[x]
            px_type = px.type
            px_val = px.value

            if px_type == color_standard:
                # simply look up the color index in the table
                r, g, b = lookup[px_val]
                alpha = 255

            elif px_type == color_transparent:
                r, g, b, alpha = 0, 0, 0, 0

            elif px_type == color_shadow:
                r, g, b, alpha = 0, 0, 0, 100

            else:
                if px_type == color_player:
                    # mark this pixel as player color
                    alpha = 254

                elif px_type == color_special_2 or\
                     px_type == color_black:
                    # mark this pixel as outline
                    alpha = 253

                    # black outline pixel, we will probably never encounter this.
                    #  -16 ensures palette[16+(-16)=0] will be used.
                    px_val = -16

                elif px_type == color_special_1:
                    alpha = 253  # mark this pixel as outline

                else:
                    raise ValueError("unknown pixel type: %d" % px_type)

                # get rgb base color from the color table
                # store it the preview player color
                # in the table: [16*player, 16*player+7]
                r, g, b = lookup[px_val + (16 * player_number)]

            # array_data[y, x] = (r, g, b, alpha)
            array_data[y, x, 0] = r
            array_data[y, x, 1] = g
            array_data[y, x, 2] = b
            array_data[y, x, 3] = alpha

    return array_data
