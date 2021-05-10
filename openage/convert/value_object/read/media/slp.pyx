# Copyright 2013-2021 the openage authors. See copying.md for legal info.
#
# cython: infer_types=True

from enum import Enum
import numpy
from struct import Struct, unpack_from

import lz4.block

from .....log import spam, dbg


cimport cython
cimport numpy

from libc.stdint cimport uint8_t
from libcpp cimport bool
from libcpp.vector cimport vector



# SLP files have little endian byte order
endianness = "< "


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
    color_standard      # standard pixel
    color_shadow        # shadow pixel
    color_shadow_v4     # shadow pixel in the 4.0X version
    color_transparent   # transparent pixel
    color_player        # non-outline player color pixel
    color_player_v4     # non-outline player color pixel
    color_black         # black outline pixel
    color_special_1     # player color outline pixel
    color_special_2     # black outline pixel


# SLPs with version 4.0+ have special
# rules for shadows
cdef enum slp_type:
    slp_standard        # standard type
    slp_shadow          # shadow SLP (v4.0 and higher)

# One SLP pixel.
cdef struct pixel:
    pixel_type type
    uint8_t value


cdef struct pixel32:
    pixel_type type
    uint8_t r
    uint8_t g
    uint8_t b
    uint8_t a


class SLP:
    """
    Class for reading/converting the greatest image format ever: SLP.
    This format is used to store all graphics within AOE.
    """

    # struct slp_version {
    #   char version[4];
    # };
    slp_version = Struct(endianness + "4s")

    # struct slp_header {
    #   int frame_count;
    #   char comment[24];
    # };
    slp_header = Struct(endianness + "i 24s")

    # struct slp_header_v4 {
    #   unsigned short frame_count;
    #   unsigned short angles;
    #   unsigned short unknown;
    #   unsigned short frame_count_alt;
    #   unsigned int checksum;
    #   int offset_main;
    #   int offset_shadow;
    #   padding 8 bytes;
    # };
    slp_header_v4 = Struct(endianness + "H H H H i i i 8x")

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

    # struct slp42_uncompressed_size {
    #   unsigned int uncompressed_size;
    # };
    slp42_uncompressed_size = Struct(endianness + "I")

    def __init__(self, data):
        self.version = SLP.slp_version.unpack_from(data)[0]
        self.compressed = False

        if self.version == b'4.2P':
            self.compressed = True

            # Extract LZ4 container
            uncompressed_size = SLP.slp42_uncompressed_size.unpack_from(data, SLP.slp_version.size)

            dbg("Decompressing SLP")
            dbg(" version:               %s",  self.version.decode('ascii'))
            dbg(" uncompressed size:     %sB", uncompressed_size)

            data = lz4.block.decompress(data[4:])

            # Read decompressed version
            self.version = SLP.slp_version.unpack_from(data)[0]

        if self.version in (b'4.0X', b'4.1X'):
            header = SLP.slp_header_v4.unpack_from(data, SLP.slp_version.size)
            frame_count, angles, _, _, checksum, offset_main, offset_shadow = header

            dbg("SLP")
            dbg(" version:               %s", self.version.decode('ascii'))
            dbg(" frame count:           %s", frame_count)
            dbg(" offset main graphic:   %s", offset_main)
            dbg(" offset shadow graphic: %s", offset_shadow)

        else:
            header = SLP.slp_header.unpack_from(data, SLP.slp_version.size)
            frame_count, comment = header

            dbg("SLP")
            dbg(" version:     %s", self.version.decode('ascii'))
            dbg(" frame count: %s", frame_count)
            dbg(" comment:     %s", comment.decode('ascii'))

        self.main_frames = list()
        self.shadow_frames = list()

        spam(FrameInfo.repr_header())

        # read all slp_frame_info structs
        for i in range(frame_count):
            frame_header_offset = (SLP.slp_version.size +
                                   SLP.slp_header.size +
                                   i * SLP.slp_frame_info.size)

            frame_info = FrameInfo(*SLP.slp_frame_info.unpack_from(
                data, frame_header_offset), self.version, slp_standard)
            spam(frame_info)

            if frame_info.properties & 0x07:
                self.main_frames.append(SLPFrame32(frame_info, data))

            elif self.version in (b'3.0\x00', b'4.0X', b'4.1X'):
                self.main_frames.append(SLPMainFrameDE(frame_info, data))

            else:
                self.main_frames.append(SLPMainFrameAoC(frame_info, data))

        if self.version in (b'4.0X', b'4.1X') and offset_shadow != 0x00000000:
            # 4.0X SLPs contain a shadow SLP inside them
            # read all slp_frame_info of shadow
            for i in range(frame_count):
                frame_header_offset = (offset_shadow +
                                       i * SLP.slp_frame_info.size)

                frame_info = FrameInfo(*SLP.slp_frame_info.unpack_from(
                    data, frame_header_offset), self.version, slp_shadow)
                spam(frame_info)
                self.shadow_frames.append(SLPShadowFrame(frame_info, data))

    def __str__(self):
        ret = list()

        ret.extend([repr(self), "\n", FrameInfo.repr_header(), "\n"])
        for frame in self.main_frames:
            ret.extend([repr(frame), "\n"])
        return "".join(ret)

    def __repr__(self):
        return f"SLP image<{len(self.main_frames):d} frames>"


class FrameInfo:
    def __init__(self, qdl_table_offset, outline_table_offset,
                 palette_offset, properties, width, height,
                 hotspot_x, hotspot_y, version, frame_type):

        # offset of command table
        self.qdl_table_offset = qdl_table_offset

        # offset of transparent outline table
        self.outline_table_offset = outline_table_offset

        self.palette_offset = palette_offset

        # used for palette index in DE1
        self.properties = properties

        self.size = (width, height)
        self.hotspot = (hotspot_x, hotspot_y)

        # meta info
        self.version = version
        self.frame_type = frame_type

    @staticmethod
    def repr_header():
        return ("offset (qdl table|outline table|palette) |"
                " properties | width x height | hotspot x/y |"
                " version")

    def __repr__(self):
        ret = (
            "        % 9d|" % self.qdl_table_offset,
            "% 13d|" % self.outline_table_offset,
            "% 7d) | " % self.palette_offset,
            "% 10d | " % self.properties,
            "% 5d x% 7d | " % self.size,
            "% 4d /% 5d" % self.hotspot,
            "% 4s" % self.version.decode('ascii'),
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

    def __init__(self, frame_info, data):
        self.info = frame_info

        if not (isinstance(data, bytes) or isinstance(data, bytearray)):
            raise ValueError("Frame data must be some bytes object")

        # memory pointer
        # convert the bytes obj to char*
        cdef const uint8_t[:] data_raw = data

        cdef unsigned short left
        cdef unsigned short right

        cdef size_t i
        cdef int cmd_offset

        cdef size_t row_count = self.info.size[1]
        self.pcolor.reserve(row_count)

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
            self.pcolor.push_back(self.create_palette_color_row(data_raw, i))

    cdef vector[pixel] create_palette_color_row(self,
                                                const uint8_t[:] &data_raw,
                                                Py_ssize_t rowid) except +:
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
        self.process_drawing_cmds(data_raw,
                                  row_data,
                                  rowid,
                                  first_cmd_offset,
                                  pixel_count - bounds.right)

        # finish by filling up the right transparent space
        for i in range(bounds.right):
            row_data.push_back(pixel(color_transparent, 0))

        # verify size of generated row
        if row_data.size() != pixel_count:
            got = row_data.size()
            summary = (
                f"{got:d}/{pixel_count:d} -> row {rowid:d}, "
                f"offset {first_cmd_offset:d} / {first_cmd_offset:#x}"
            )
            message = (
                f"got {'LESS' if got < pixel_count else 'MORE'} pixels than expected: {summary}, "
                f"missing: {abs(pixel_count - got):d}"
            )

            raise Exception(message)

        return row_data

    @cython.boundscheck(False)
    cdef void process_drawing_cmds(self,
                                   const uint8_t[:] &data_raw,
                                   vector[pixel] &row_data,
                                   Py_ssize_t rowid,
                                   Py_ssize_t first_cmd_offset,
                                   size_t expected_size):
        pass

    def get_picture_data(self, palette):
        """
        Convert the palette index matrix to a colored image.
        """
        return determine_rgba_matrix(self.pcolor, palette)

    def get_hotspot(self):
        """
        Return the frame's hotspot (the "center" of the image)
        """
        return self.info.hotspot

    def get_palette_number(self):
        """
        Return the frame's palette number.

        :return: Palette number of the frame.
        :rtype: int
        """
        if self.info.version in (b'3.0\x00', b'4.0X', b'4.1X'):
            if self.info.properties > 0xFFFFFF:
                # DE1 effects palette
                return 54

            return self.info.properties >> 16

        else:
            return self.info.palette_offset + 50500

    def __repr__(self):
        return repr(self.info)


cdef class SLPMainFrameAoC(SLPFrame):
    """
    SLPFrame for the main graphics sprite up to SLP version 2.0.
    """

    def __init__(self, frame_info, data):
        super().__init__(frame_info, data)

    @cython.boundscheck(False)
    cdef void process_drawing_cmds(self,
                                   const uint8_t[:] &data_raw,
                                   vector[pixel] &row_data,
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
        cdef uint8_t color
        cdef uint8_t nextbyte
        cdef uint8_t lower_nibble
        cdef uint8_t higher_nibble
        cdef uint8_t lowest_crumb
        cdef cmd_pack cpack
        cdef int pixel_count

        # work through commands till end of row.
        while not eor:
            if row_data.size() > expected_size:
                raise Exception(
                    f"Only {expected_size:d} pixels should be drawn in row {rowid:d}, " +
                    f"but we have {row_data.size():d} already!"
                )

            # fetch drawing instruction
            cmd = data_raw[dpos]

            lower_nibble = 0x0f & cmd
            higher_nibble = 0xf0 & cmd
            lowest_crumb = 0b00000011 & cmd

            # opcode: cmd, rowid: rowid

            if lower_nibble == 0x0F:
                # eol (end of line) command, this row is finished now.
                eor = True
                continue

            elif lowest_crumb == 0b00000000:
                # color_list command
                # draw the following bytes as palette colors

                pixel_count = cmd >> 2
                for _ in range(pixel_count):
                    dpos += 1
                    color = data_raw[dpos]

                    row_data.push_back(pixel(color_standard, color))

            elif lowest_crumb == 0b00000001:
                # skip command
                # draw 'count' transparent pixels
                # count = cmd >> 2; if count == 0: count = nextbyte

                cpack = cmd_or_next(data_raw, cmd, 2, dpos)
                dpos = cpack.dpos
                for _ in range(cpack.count):
                    row_data.push_back(pixel(color_transparent, 0))

            elif lower_nibble == 0x02:
                # big_color_list command
                # draw (higher_nibble << 4 + nextbyte) following palette colors

                dpos += 1
                nextbyte = data_raw[dpos]
                pixel_count = (higher_nibble << 4) + nextbyte

                for _ in range(pixel_count):
                    dpos += 1
                    color = data_raw[dpos]
                    row_data.push_back(pixel(color_standard, color))

            elif lower_nibble == 0x03:
                # big_skip command
                # draw (higher_nibble << 4 + nextbyte)
                # transparent pixels

                dpos += 1
                nextbyte = data_raw[dpos]
                pixel_count = (higher_nibble << 4) + nextbyte

                for _ in range(pixel_count):
                    row_data.push_back(pixel(color_transparent, 0))

            elif lower_nibble == 0x06:
                # player_color_list command
                # we have to draw the player color for cmd>>4 times,
                # or if that is 0, as often as the next byte says.

                cpack = cmd_or_next(data_raw, cmd, 4, dpos)
                dpos = cpack.dpos
                for _ in range(cpack.count):
                    dpos += 1
                    color = data_raw[dpos]

                    row_data.push_back(pixel(color_player, color))

            elif lower_nibble == 0x07:
                # fill command
                # draw 'count' pixels with color of next byte

                cpack = cmd_or_next(data_raw, cmd, 4, dpos)
                dpos = cpack.dpos

                dpos += 1
                color = data_raw[dpos]

                for _ in range(cpack.count):
                    row_data.push_back(pixel(color_standard, color))

            elif lower_nibble == 0x0A:
                # fill player color command
                # draw the player color for 'count' times

                cpack = cmd_or_next(data_raw, cmd, 4, dpos)
                dpos = cpack.dpos

                dpos += 1
                color = data_raw[dpos]

                for _ in range(cpack.count):
                    row_data.push_back(pixel(color_player, color))

            elif lower_nibble == 0x0B:
                # shadow command
                # draw a transparent shadow pixel for 'count' times

                cpack = cmd_or_next(data_raw, cmd, 4, dpos)
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
                    pixel_count = data_raw[dpos]

                    for _ in range(pixel_count):
                        row_data.push_back(pixel(color_special_1, 0))

                elif higher_nibble == 0x70:
                    # outline_span_2 command
                    # same as above, using special color 2

                    dpos += 1
                    pixel_count = data_raw[dpos]

                    for _ in range(pixel_count):
                        row_data.push_back(pixel(color_special_2, 0))

                elif higher_nibble == 0x80:
                    # dither command
                    raise NotImplementedError("dither not implemented")

                elif higher_nibble in (0x90, 0xA0):
                    # 0x90: premultiplied alpha
                    # 0xA0: original alpha
                    raise NotImplementedError("extended alpha not implemented")

            else:
                raise Exception(
                    f"unknown slp drawing command: " +
                    f"{cmd:#x} in row {rowid:d}")

            dpos += 1

        # end of row reached, return the created pixel array.
        return


cdef class SLPMainFrameDE(SLPFrame):
    """
    SLPFrame for the main graphics sprite since SLP version 3.0.
    """

    def __init__(self, frame_info, data):
        super().__init__(frame_info, data)

    @cython.boundscheck(False)
    cdef void process_drawing_cmds(self,
                                   const uint8_t[:] &data_raw,
                                   vector[pixel] &row_data,
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
        cdef uint8_t color
        cdef uint8_t nextbyte
        cdef uint8_t lower_nibble
        cdef uint8_t higher_nibble
        cdef uint8_t lowest_crumb
        cdef cmd_pack cpack
        cdef int pixel_count

        # work through commands till end of row.
        while not eor:
            if row_data.size() > expected_size:
                raise Exception(
                    f"Only {expected_size:d} pixels should be drawn in row {rowid:d}, "
                    f"but we have {row_data.size():d} already!"
                )

            # fetch drawing instruction
            cmd = data_raw[dpos]

            lower_nibble = 0x0f & cmd
            higher_nibble = 0xf0 & cmd
            lowest_crumb = 0b00000011 & cmd

            # opcode: cmd, rowid: rowid

            if lower_nibble == 0x0F:
                # eol (end of line) command, this row is finished now.
                eor = True
                continue

            elif lowest_crumb == 0b00000000:
                # color_list command
                # draw the following bytes as palette colors

                pixel_count = cmd >> 2
                for _ in range(pixel_count):
                    dpos += 1
                    color = data_raw[dpos]

                    row_data.push_back(pixel(color_standard, color))

            elif lowest_crumb == 0b00000001:
                # skip command
                # draw 'count' transparent pixels
                # count = cmd >> 2; if count == 0: count = nextbyte

                cpack = cmd_or_next(data_raw, cmd, 2, dpos)
                dpos = cpack.dpos
                for _ in range(cpack.count):
                    row_data.push_back(pixel(color_transparent, 0))

            elif lower_nibble == 0x02:
                # big_color_list command
                # draw (higher_nibble << 4 + nextbyte) following palette colors

                dpos += 1
                nextbyte = data_raw[dpos]
                pixel_count = (higher_nibble << 4) + nextbyte

                for _ in range(pixel_count):
                    dpos += 1
                    color = data_raw[dpos]
                    row_data.push_back(pixel(color_standard, color))

            elif lower_nibble == 0x03:
                # big_skip command
                # draw (higher_nibble << 4 + nextbyte)
                # transparent pixels

                dpos += 1
                nextbyte = data_raw[dpos]
                pixel_count = (higher_nibble << 4) + nextbyte

                for _ in range(pixel_count):
                    row_data.push_back(pixel(color_transparent, 0))

            elif lower_nibble == 0x06:
                # player_color_list command
                # we have to draw the player color for cmd>>4 times,
                # or if that is 0, as often as the next byte says.
                cpack = cmd_or_next(data_raw, cmd, 4, dpos)
                dpos = cpack.dpos
                for _ in range(cpack.count):
                    dpos += 1
                    color = data_raw[dpos]

                    # version 3.0 uses extra palettes for player colors
                    row_data.push_back(pixel(color_player_v4, color))

            elif lower_nibble == 0x07:
                # fill command
                # draw 'count' pixels with color of next byte

                cpack = cmd_or_next(data_raw, cmd, 4, dpos)
                dpos = cpack.dpos

                dpos += 1
                color = data_raw[dpos]

                for _ in range(cpack.count):
                    row_data.push_back(pixel(color_standard, color))

            elif lower_nibble == 0x0A:
                # fill player color command
                # draw the player color for 'count' times

                cpack = cmd_or_next(data_raw, cmd, 4, dpos)
                dpos = cpack.dpos

                dpos += 1
                color = data_raw[dpos]

                for _ in range(cpack.count):
                    # version 3.0 uses extra palettes for player colors
                    row_data.push_back(pixel(color_player_v4, color))

            elif lower_nibble == 0x0B:
                # shadow command
                # draw a transparent shadow pixel for 'count' times

                cpack = cmd_or_next(data_raw, cmd, 4, dpos)
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
                    pixel_count = data_raw[dpos]

                    for _ in range(pixel_count):
                        row_data.push_back(pixel(color_special_1, 0))

                elif higher_nibble == 0x70:
                    # outline_span_2 command
                    # same as above, using special color 2

                    dpos += 1
                    pixel_count = data_raw[dpos]

                    for _ in range(pixel_count):
                        row_data.push_back(pixel(color_special_2, 0))

                elif higher_nibble == 0x80:
                    # dither command
                    raise NotImplementedError("dither not implemented")

                elif higher_nibble in (0x90, 0xA0):
                    # 0x90: premultiplied alpha
                    # 0xA0: original alpha
                    raise NotImplementedError("extended alpha not implemented")

            else:
                raise Exception(
                    f"unknown slp drawing command: " +
                    f"{cmd:#x} in row {rowid:d}")

            dpos += 1

        # end of row reached, return the created pixel array.
        return

cdef class SLPShadowFrame(SLPFrame):
    """
    SLPFrame for the shadow graphics in SLP version 4.0 and 4.1.
    """

    def __init__(self, frame_info, data):
        super().__init__(frame_info, data)

    @cython.boundscheck(False)
    cdef void process_drawing_cmds(self,
                                   const uint8_t[:] &data_raw,
                                   vector[pixel] &row_data,
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
        cdef uint8_t color
        cdef uint8_t nextbyte
        cdef uint8_t lower_nibble
        cdef uint8_t higher_nibble
        cdef uint8_t lowest_crumb
        cdef cmd_pack cpack
        cdef int pixel_count

        # work through commands till end of row.
        while not eor:
            if row_data.size() > expected_size:
                raise Exception(
                    f"Only {expected_size} pixels should be drawn in row {rowid:d}, "
                    f"but we have {row_data.size():d} already!"
                )

            # fetch drawing instruction
            cmd = data_raw[dpos]

            lower_nibble = 0x0f & cmd
            higher_nibble = 0xf0 & cmd
            lowest_crumb = 0b00000011 & cmd

            # opcode: cmd, rowid: rowid

            if lower_nibble == 0x0F:
                # eol (end of line) command, this row is finished now.
                eor = True
                continue

            elif lowest_crumb == 0b00000000:
                # color_list command
                # draw the following bytes as palette colors

                pixel_count = cmd >> 2
                for _ in range(pixel_count):
                    dpos += 1
                    color = data_raw[dpos]

                    # shadows in v4.0 draw a different color
                    row_data.push_back(pixel(color_shadow_v4, color))

            elif lowest_crumb == 0b00000001:
                # skip command
                # draw 'count' transparent pixels
                # count = cmd >> 2; if count == 0: count = nextbyte

                cpack = cmd_or_next(data_raw, cmd, 2, dpos)
                dpos = cpack.dpos
                for _ in range(cpack.count):
                    row_data.push_back(pixel(color_transparent, 0))

            elif lower_nibble == 0x02:
                # big_color_list command
                # draw (higher_nibble << 4 + nextbyte) following palette colors

                dpos += 1
                nextbyte = data_raw[dpos]
                pixel_count = (higher_nibble << 4) + nextbyte

                for _ in range(pixel_count):
                    dpos += 1
                    color = data_raw[dpos]
                    row_data.push_back(pixel(color_shadow_v4, color))

            elif lower_nibble == 0x03:
                # big_skip command
                # draw (higher_nibble << 4 + nextbyte)
                # transparent pixels

                dpos += 1
                nextbyte = data_raw[dpos]
                pixel_count = (higher_nibble << 4) + nextbyte

                for _ in range(pixel_count):
                    row_data.push_back(pixel(color_transparent, 0))

            elif lower_nibble == 0x06:
                # player_color_list command
                # we have to draw the player color for cmd>>4 times,
                # or if that is 0, as often as the next byte says.
                cpack = cmd_or_next(data_raw, cmd, 4, dpos)
                dpos = cpack.dpos
                for _ in range(cpack.count):
                    dpos += 1
                    color = data_raw[dpos]

                    # version 3.0 uses extra palettes for player colors
                    row_data.push_back(pixel(color_player_v4, color))

            elif lower_nibble == 0x07:
                # fill command
                # draw 'count' pixels with color of next byte

                cpack = cmd_or_next(data_raw, cmd, 4, dpos)
                dpos = cpack.dpos

                dpos += 1
                color = data_raw[dpos]

                for _ in range(cpack.count):
                    # shadows in v4.0 draw a different color
                    row_data.push_back(pixel(color_shadow_v4, color))

            else:
                raise Exception(
                    f"unknown slp shadow drawing command: " +
                    f"{cmd:#x} in row {rowid:d}"
                )

            dpos += 1

        # end of row reached, return the created pixel array.
        return


cdef class SLPFrame32:
    """
    An RGBA color frames inside the SLP.
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
    cdef vector[vector[pixel32]] pcolor

    def __init__(self, frame_info, data):
        self.info = frame_info

        if not (isinstance(data, bytes) or isinstance(data, bytearray)):
            raise ValueError("Frame data must be some bytes object")

        # memory pointer
        # convert the bytes obj to char*
        cdef const uint8_t[:] data_raw = data

        cdef unsigned short left
        cdef unsigned short right

        cdef size_t i
        cdef int cmd_offset

        cdef size_t row_count = self.info.size[1]
        self.pcolor.reserve(row_count)

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
            self.pcolor.push_back(self.create_palette_color_row(data_raw, i))

    cdef vector[pixel32] create_palette_color_row(self,
                                                  const uint8_t[:] &data_raw,
                                                  Py_ssize_t rowid) except +:
        """
        create palette indices (colors) for the given rowid.
        """
        cdef vector[pixel32] row_data
        cdef Py_ssize_t i

        first_cmd_offset = self.cmd_offsets[rowid]
        cdef boundary_def bounds = self.boundaries[rowid]
        cdef size_t pixel_count = self.info.size[0]

        # preallocate memory
        row_data.reserve(pixel_count)

        # row is completely transparent
        if bounds.full_row:
            for _ in range(pixel_count):
                row_data.push_back(pixel32(color_transparent, 0, 0, 0, 0))

            return row_data

        # start drawing the left transparent space
        for i in range(bounds.left):
            row_data.push_back(pixel32(color_transparent, 0, 0, 0, 0))

        # process the drawing commands for this row.
        self.process_drawing_cmds(data_raw,
                                  row_data,
                                  rowid,
                                  first_cmd_offset,
                                  pixel_count - bounds.right)

        # finish by filling up the right transparent space
        for i in range(bounds.right):
            row_data.push_back(pixel32(color_transparent, 0, 0, 0, 0))

        # verify size of generated row
        if row_data.size() != pixel_count:
            got = row_data.size()
            summary = (
                f"{got:d}/{pixel_count:d} -> row {rowid:d}, "
                f"offset {first_cmd_offset:d} / {first_cmd_offset:#x}"
            )
            message = (
                f"got {'LESS' if got < pixel_count else 'MORE'} pixels than expected: {summary}, "
                f"missing: {abs(pixel_count - got):d}"
            )

            raise Exception(message)

        return row_data

    @cython.boundscheck(False)
    cdef void process_drawing_cmds(self,
                                   const uint8_t[:] &data_raw,
                                   vector[pixel32] &row_data,
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
        cdef uint8_t player_color
        cdef uint8_t nextbyte
        cdef uint8_t lower_nibble
        cdef uint8_t higher_nibble
        cdef uint8_t lowest_crumb
        cdef cmd_pack cpack
        cdef int pixel_count

        # Color channels are saved in BGRA order
        cdef vector[uint8_t] pixel_data
        pixel_data.reserve(4)

        # work through commands till end of row.
        while not eor:
            if row_data.size() > expected_size:
                raise Exception(
                    f"Only {expected_size:d} pixels should be drawn in row {rowid:d}, " +
                    f"but we have {row_data.size():d} already!"
                )

            # fetch drawing instruction
            cmd = data_raw[dpos]

            lower_nibble = 0x0f & cmd
            higher_nibble = 0xf0 & cmd
            lowest_crumb = 0b00000011 & cmd

            # opcode: cmd, rowid: rowid

            if lower_nibble == 0x0F:
                # eol (end of line) command, this row is finished now.
                eor = True
                continue

            elif lowest_crumb == 0b00000000:
                # color_list command
                # draw the following bytes as palette colors
                pixel_count = cmd >> 2
                for _ in range(pixel_count):
                    for _ in range(4):
                        dpos += 1
                        pixel_data.push_back(data_raw[dpos])

                    row_data.push_back(pixel32(color_standard,
                                               pixel_data[2],
                                               pixel_data[1],
                                               pixel_data[0],
                                               255))

                    pixel_data.clear()

            elif lowest_crumb == 0b00000001:
                # skip command
                # draw 'count' transparent pixels
                # count = cmd >> 2; if count == 0: count = nextbyte
                cpack = cmd_or_next(data_raw, cmd, 2, dpos)
                dpos = cpack.dpos
                for _ in range(cpack.count):
                    row_data.push_back(pixel32(color_transparent, 0, 0, 0, 0))

            elif lower_nibble == 0x02:
                # big_color_list command
                # draw (higher_nibble << 4 + nextbyte) following palette colors
                dpos += 1
                nextbyte = data_raw[dpos]
                pixel_count = (higher_nibble << 4) + nextbyte

                for _ in range(pixel_count):
                    for _ in range(4):
                        dpos += 1
                        pixel_data.push_back(data_raw[dpos])

                    row_data.push_back(pixel32(color_standard,
                                               pixel_data[2],
                                               pixel_data[1],
                                               pixel_data[0],
                                               255))

                    pixel_data.clear()

            elif lower_nibble == 0x03:
                # big_skip command
                # draw (higher_nibble << 4 + nextbyte)
                # transparent pixels

                dpos += 1
                nextbyte = data_raw[dpos]
                pixel_count = (higher_nibble << 4) + nextbyte

                for _ in range(pixel_count):
                    row_data.push_back(pixel32(color_transparent, 0, 0, 0, 0))

            elif lower_nibble == 0x06:
                # player_color_list command
                # we have to draw the player color for cmd>>4 times,
                # or if that is 0, as often as the next byte says.
                cpack = cmd_or_next(data_raw, cmd, 4, dpos)
                dpos = cpack.dpos
                for _ in range(cpack.count):
                    dpos += 1
                    player_color = data_raw[dpos]

                    row_data.push_back(pixel32(color_player, player_color, 0, 0, 0))

            elif lower_nibble == 0x07:
                # fill command
                # draw 'count' pixels with color of next byte
                cpack = cmd_or_next(data_raw, cmd, 4, dpos)
                dpos = cpack.dpos

                for _ in range(cpack.count):
                    for _ in range(4):
                        dpos += 1
                        pixel_data.push_back(data_raw[dpos])

                    row_data.push_back(pixel32(color_standard,
                                               pixel_data[2],
                                               pixel_data[1],
                                               pixel_data[0],
                                               255))

                    pixel_data.clear()

            elif lower_nibble == 0x0A:
                # fill player color command
                # draw the player color for 'count' times
                cpack = cmd_or_next(data_raw, cmd, 4, dpos)
                dpos = cpack.dpos

                dpos += 1
                player_color = data_raw[dpos]

                for _ in range(cpack.count):
                    row_data.push_back(pixel32(color_player, player_color, 0, 0, 0))

            elif lower_nibble == 0x0B:
                # shadow command
                # draw a transparent shadow pixel for 'count' times
                cpack = cmd_or_next(data_raw, cmd, 4, dpos)
                dpos = cpack.dpos

                for _ in range(cpack.count):
                    row_data.push_back(pixel32(color_shadow, 0, 0, 0, 0))

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
                    row_data.push_back(pixel32(color_special_1, 0, 0, 0, 0))

                elif higher_nibble == 0x60:
                    # outline_2 command
                    # same as above, but special color 2
                    row_data.push_back(pixel32(color_special_2, 0, 0, 0, 0))

                elif higher_nibble == 0x50:
                    # outline_span_1 command
                    # same as above, but span special color 1 nextbyte times.

                    dpos += 1
                    pixel_count = data_raw[dpos]

                    for _ in range(pixel_count):
                        row_data.push_back(pixel32(color_special_1, 0, 0, 0, 0))

                elif higher_nibble == 0x70:
                    # outline_span_2 command
                    # same as above, using special color 2

                    dpos += 1
                    pixel_count = data_raw[dpos]

                    for _ in range(pixel_count):
                        row_data.push_back(pixel32(color_special_2, 0, 0, 0, 0))

                elif higher_nibble == 0x80:
                    # dither command
                    raise NotImplementedError("dither not implemented")

                elif higher_nibble == 0x90:
                    # 0x90: premultiplied alpha
                    dpos += 1
                    nextbyte = data_raw[dpos]
                    pixel_count = nextbyte

                    for _ in range(pixel_count):
                        for _ in range(4):
                            dpos += 1
                            pixel_data.push_back(data_raw[dpos])

                        row_data.push_back(pixel32(color_standard,
                                                   pixel_data[2],
                                                   pixel_data[1],
                                                   pixel_data[0],
                                                   255 - pixel_data[3]))

                        pixel_data.clear()

                elif higher_nibble == 0xA0:
                    # 0xA0: original alpha
                    raise NotImplementedError("original alpha not implemented")

            else:
                raise Exception(
                    f"unknown slp drawing command: " +
                    f"{cmd:#x} in row {rowid:d}")

            dpos += 1

        # end of row reached, return the created pixel array.
        return


    def get_picture_data(self, palette):
        """
        Convert the palette index matrix to a colored image.
        """
        return determine_rgba_matrix32(self.pcolor)

    def get_hotspot(self):
        """
        Return the frame's hotspot (the "center" of the image)
        """
        return self.info.hotspot

    def get_palette_number(self):
        """
        Return the frame's palette number.

        :return: Palette number of the frame.
        :rtype: int
        """
        return None

    def __repr__(self):
        return repr(self.info)


@cython.boundscheck(False)
cdef inline cmd_pack cmd_or_next(const uint8_t[:] &data_raw,
                                 uint8_t cmd,
                                 uint8_t n,
                                 Py_ssize_t pos):
    """
    to save memory, the draw amount may be encoded into
    the drawing command itself in the upper n bits.
    """
    cdef uint8_t packed_in_cmd = cmd >> n

    if packed_in_cmd != 0:
        return cmd_pack(packed_in_cmd, pos)

    else:
        pos += 1
        return cmd_pack(data_raw[pos], pos)


@cython.boundscheck(False)
@cython.wraparound(False)
cdef numpy.ndarray determine_rgba_matrix(vector[vector[pixel]] &image_matrix,
                                         numpy.ndarray[numpy.uint8_t, ndim=2, mode="c"] palette):
    """
    converts a palette index image matrix to an rgba matrix.
    """
    cdef size_t height = image_matrix.size()
    cdef size_t width = image_matrix[0].size()

    cdef numpy.ndarray[numpy.uint8_t, ndim=3, mode="c"] array_data = \
        numpy.zeros((height, width, 4), dtype=numpy.uint8)

    cdef uint8_t[:, ::1] m_lookup = palette

    cdef uint8_t r
    cdef uint8_t g
    cdef uint8_t b
    cdef uint8_t alpha

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
                r = m_lookup[px_val][0]
                g = m_lookup[px_val][1]
                b = m_lookup[px_val][2]
                alpha = 255

            elif px_type == color_transparent:
                r, g, b, alpha = 0, 0, 0, 0

            elif px_type == color_shadow:
                r, g, b, alpha = 0, 0, 0, 100

            elif px_type == color_shadow_v4:
                r, g, b = 0, 0, 0
                alpha = 255 - (px_val << 2)

            else:
                if px_type == color_player_v4 or px_type == color_player:
                    # mark this pixel as player color
                    alpha = 255

                elif px_type == color_special_2 or\
                     px_type == color_black:
                    alpha = 251  # mark this pixel as special outline

                elif px_type == color_special_1:
                    alpha = 253  # mark this pixel as outline

                else:
                    raise ValueError("unknown pixel type: %d" % px_type)

                # Store player color index in g channel
                r, b = 0, 0
                g = px_val

            # array_data[y, x] = (r, g, b, alpha)
            array_data[y, x, 0] = r
            array_data[y, x, 1] = g
            array_data[y, x, 2] = b
            array_data[y, x, 3] = alpha

    return array_data


@cython.boundscheck(False)
@cython.wraparound(False)
cdef numpy.ndarray determine_rgba_matrix32(vector[vector[pixel32]] &image_matrix):
    """
    converts a 32-Bit SLP image matrix to an rgba matrix.
    """
    cdef size_t height = image_matrix.size()
    cdef size_t width = image_matrix[0].size()

    cdef numpy.ndarray[numpy.uint8_t, ndim=3, mode="c"] array_data = \
        numpy.zeros((height, width, 4), dtype=numpy.uint8)

    cdef uint8_t r
    cdef uint8_t g
    cdef uint8_t b
    cdef uint8_t alpha

    cdef vector[pixel32] current_row
    cdef pixel32 px
    cdef pixel_type px_type
    cdef int px_val

    cdef size_t x
    cdef size_t y

    for y in range(height):
        current_row = image_matrix[y]

        for x in range(width):
            px = current_row[x]
            px_type = px.type

            if px_type == color_standard:
                # simply look up the color index in the table
                r = px.r
                g = px.g
                b = px.b
                alpha = px.a

            elif px_type == color_transparent:
                r, g, b, alpha = 0, 0, 0, 0

            elif px_type == color_shadow:
                r, g, b, alpha = 0, 0, 0, 100

            elif px_type == color_shadow_v4:
                r, g, b = 0, 0, 0
                alpha = 255 - (px.r << 2)

            else:
                if px_type == color_player_v4 or px_type == color_player:
                    # mark this pixel as player color
                    alpha = 255

                elif px_type == color_special_2 or\
                     px_type == color_black:
                    alpha = 251  # mark this pixel as special outline

                elif px_type == color_special_1:
                    alpha = 253  # mark this pixel as outline

                else:
                    raise ValueError("unknown pixel type: %d" % px_type)

                # Store player color index in g channel
                r, b = 0, 0
                g = px.r

            # array_data[y, x] = (r, g, b, alpha)
            array_data[y, x, 0] = r
            array_data[y, x, 1] = g
            array_data[y, x, 2] = b
            array_data[y, x, 3] = alpha

    return array_data
