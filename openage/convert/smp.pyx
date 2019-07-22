# Copyright 2013-2019 the openage authors. See copying.md for legal info.
#
# cython: profile=False

from struct import Struct, unpack_from

from enum import Enum
from reportlab.lib.colors import red
from Cython.Utility.MemoryView import offset

cimport cython
import numpy
cimport numpy

from libc.stdint cimport uint8_t
from libcpp cimport bool
from libcpp.vector cimport vector

from ..log import spam, dbg


# SMP files have little endian byte order
endianness = "< "


class SpecialColorValue(Enum):
    shadow = "%"
    transparent = " "
    player_color = "P"

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
    color_transparent    # transparent pixel
    color_player         # non-outline player color pixel
    color_special_1      # player color outline pixel
    color_special_2      # black outline pixel


# One SMP pixel.
cdef struct pixel:
    pixel_type type
    uint32_t rgba


class SMP:
    """
    Class for reading/converting the SMP image format (successor of SLP).
    This format is used to store all graphics within AoE2: Definitive Edition.
    """

    # struct smp_header {
    #   char file_descriptor[4];
    #   ??? 4 bytes;
    #   int frame_count;
    #   ??? 4 bytes;
    #   ??? 4 bytes;
    #   unsigned int unknown_offset_1;
    #   unsigned int unknown_offset_2;
    #   ??? 4 bytes;
    #   char comment[32];
    # };
    smp_header = Struct(endianness + "4s i i i i I I I 32s")

    # struct smp_frame_info_offset {
    #   unsigned int frame_info_offset;
    # };
    smp_frame_header_offset = Struct(endianness + "I")

    # struct smp_frame_info {
    #   int          width;
    #   int          height;
    #   int          hotspot_x;
    #   int          hotspot_y;
    #   ??? 4 bytes;
    #   unsigned int outline_table_offset;
    #   unsigned int qdl_table_offset;
    #   ??? 4 bytes;
    # };
    smp_frame_header = Struct(endianness + "i i i i I I I I")

    def __init__(self, data):
        smp_header = SLP.smp_header.unpack_from(data)
        _, _, frame_count, _, _, _, _, _, comment = smp_header

        dbg("SMP")
        dbg(" frame count: %s", frame_count)
        dbg(" comment:     %s", comment.decode('ascii'))

        frame_offsets = list()

        # read offsets of the smp frames
        for i in range(frame_count):
            frame_header_offset = (SMP.smp_header.size +
                                   i * SMP.smp_frame_header_offset.size)

            frame_offset = SMP.smp_frame_header_offset.unpack_from(
                data, frame_header_offset)
            
            frame_offsets.append(frame_offset)

        # SMP graphic frames are created from overlaying
        # the main graphic frame with a shadow frame 
        self.main_frames = list()
        self.shadow_frames = list()

        spam(FrameInfo.repr_header())

        # read all smp_frame_header structs
        for offset in frame_offsets:

            # first the main graphics...
            main_frame_header = FrameHeader(*SMP.smp_frame_header.unpack_from(
                data, offset
            ))
            spam(main_frame_header)
            self.frames.append(SMPFrame(main_frame_header, data))
            
            # ... then the corresponding shadow
            shadow_offset = offset + SMP.smp_frame_header.size
            
            shadow_frame_header = FrameHeader(*SMP.smp_frame_header.unpack_from(
                data, shadow_offset), offset)
            spam(shadow_frame_header)
            self.frames.append(SMPFrame(shadow_frame_header, data))

    def __str__(self):
        ret = list()

        ret.extend([repr(self), "\n", FrameInfo.repr_header(), "\n"])
        for frame in self.frames:
            ret.extend([repr(frame), "\n"])
        return "".join(ret)

    def __repr__(self):
        # TODO: lookup the image content description
        return "SMP image<%d frames>" % len(self.frames)


class FrameHeader:
    def __init__(self, width, height, hotspot_x,
                 hotspot_y, unknown_value_1,
                 unknown_value_2, outline_table_offset,
                 qdl_table_offset, frame_header_offset):

        self.size = (width, height)
        self.hotspot = (hotspot_x, hotspot_y)
        
        # table offsets are relative to the frame header offset
        self.outline_table_offset = outline_table_offset - frame_header_offset
        self.qdl_table_offset = qdl_table_offset - frame_header_offset
        
        self.header_offset = frame_header_offset
    
    @staticmethod
    def repr_header():
        return ("width x height | hotspot x/y | "
                "unknown value | "
                "offset (outline table|qdl table) | "
                "unknown value")

    def __repr__(self):
        ret = (
            "% 5d x% 7d | " % self.size,
            "% 4d /% 5d" % self.hotspot,
            "% 13d|" % self.outline_table_offset,
            "        % 9d|" % self.qdl_table_offset,
        )
        return "".join(ret)

cdef class SMPFrame:
    """
    one image inside the SMP. you can imagine it as a frame of a video.
    """

    # struct smp_frame_row_edge {
    #   unsigned short left_space;
    #   unsigned short right_space;
    # };
    smp_frame_row_edge = Struct(endianness + "H H")

    # struct smp_command_offset {
    #   unsigned int offset;
    # }
    smp_command_offset = Struct(endianness + "I")
    
    # struct smp_pixel {
    #   unsigned int pixel;
    # }
    smp_pixel = Struct(endianness + "I")

    # for each row:
    # contains (left, right, full_row) number of boundary pixels
    cdef vector[boundary_def] boundaries

    # stores the file offset for the first drawing command
    cdef vector[int] cmd_offsets

    # pixel matrix representing the final image
    cdef vector[vector[pixel]] pcolor

    # memory pointer
    cdef const uint8_t *data_raw

    def __init__(self, frame_header, data):
        self.header = frame_header

        if not (isinstance(data, bytes) or isinstance(data, bytearray)):
            raise ValueError("Frame data must be some bytes object")

        self.data = data

        # convert the bytes obj to char*
        self.data_raw = data

        cdef size_t i
        cdef int cmd_offset

        cdef size_t row_count = self.header.size[1]

        # process bondary table
        for i in range(row_count):
            outline_entry_position = (self.header.outline_table_offset + 
                                      i * SMPFrame.smp_frame_row_edge.size)

            left, right = SMPFrame.smp_frame_row_edge.unpack_from(
                data, outline_entry_position
            )

            # is this row completely transparent?
            if left == 0x8000 or right == 0x8000:
                self.boundaries.push_back(boundary_def(0, 0, True))
            else:
                self.boundaries.push_back(boundary_def(left, right, False))

        # process cmd table
        for i in range(row_count):
            cmd_table_position = (self.header.qdl_table_offset +
                                  i * SMPFrame.smp_command_offset.size)

            cmd_offset = SMPFrame.smp_command_offset.unpack_from(
                data, cmd_table_position)[0] + self.header.header_offset
            self.cmd_offsets.push_back(cmd_offset)

        for i in range(row_count):
            self.pcolor.push_back(self.create_color_row(i))

    cdef vector[pixel] create_color_row(self, Py_ssize_t rowid) except +:
        """
        extract colors (pixels) for the given rowid.
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
        extract colors (pixels) for the drawing commands
        found for this row in the SMP frame.
        """

        # position in the data blob, we start at the first command of this row
        cdef Py_ssize_t dpos = first_cmd_offset

        # is the end of the current row reached?
        cdef bool eor = False

        cdef uint8_t cmd
        cdef uint8_t nextbyte
        cdef uint8_t lower_crumbs
        cdef uint8_t higher_crumb
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

            # Last 2 bits store command type
            lower_crumb = 0x03 & cmd
            
            # First 6 bits store length data
            higher_crumbs = 0xfc & cmd

            # opcode: cmd, rowid: rowid
            
            if lower_crumb == 0x03:
                # eol (end of line) command, this row is finished now.
                eor = True
                continue
            
            if lower_crumb == 0x00:
                # skip command
                # draw 'count' transparent pixels
                # count = (cmd >> 2) + 1
                
                dpos += 1
                nextbyte = self.get_byte_at(dpos)
                
                pixel_count = (higher_crumbs >> 2) + 1
                
                for _ in range(pixel_count):
                    row_data.push_back(pixel(color_transparent, 0))
                    
            if lower_crumb == 0x01:
                # color_list command
                # draw the following 'count' pixels
                # pixels are stored as rgba 32 bit values
                # count = (cmd >> 2) + 1
                
                pixel_count = (higher_crumbs >> 2) + 1

                dpos += 1
                color = self.get_rgba_pixel(dpos)
                row_data.push_back(pixel(color_standard, color))

                for _ in range(pixel_count - 1):
                    dpos += 4
                    color = self.get_rgba_pixel(dpos)
                    row_data.push_back(pixel(color_standard, color))
    
    cdef inline uint32_t get_rgba_pixel(self, Py_ssize_t offset):
        """
        Fetch an RGBA pixel value from the smp (4 bytes).
        """
        return SMP.smp_pixel.unpack_from(self.data, offset)[0]

    def get_picture_data(self, player_palette=None, player_number=0):
        """
        Convert the palette index matrix to a colored image.
        """
        return determine_rgba_matrix(self.pcolor, player_palette, player_number)

    def get_hotspot(self):
        """
        Return the frame's hotspot (the "center" of the image)
        """
        return self.header.hotspot

    def __repr__(self):
        return repr(self.header)

@cython.boundscheck(False)
@cython.wraparound(False)
cdef numpy.ndarray determine_rgba_matrix(vector[vector[pixel]] &image_matrix,
                                         player_palette=None, int player_number=0):
    """
    converts a palette index image matrix to an rgba matrix.
    """

    cdef size_t height = image_matrix.size()
    cdef size_t width = image_matrix[0].size()

