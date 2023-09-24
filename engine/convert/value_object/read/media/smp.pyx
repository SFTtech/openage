# Copyright 2013-2023 the openage authors. See copying.md for legal info.
#
# cython: infer_types=True

from enum import Enum
import numpy
from struct import Struct, unpack_from

from .....log import spam, dbg


cimport cython
cimport numpy

from libc.stdint cimport uint8_t, uint16_t
from libcpp cimport bool
from libcpp.vector cimport vector



# SMP files have little endian byte order
endianness = "< "


cdef struct boundary_def:
    Py_ssize_t left
    Py_ssize_t right
    bool full_row


# SMP pixels are super special.
cdef enum pixel_type:
    color_standard      # standard pixel
    color_shadow        # shadow pixel
    color_transparent   # transparent pixel
    color_player        # non-outline player color pixel
    color_outline       # player color outline pixel


# One SMP pixel.
cdef struct pixel:
    pixel_type type
    uint8_t index               # index in a palette section
    uint8_t palette             # palette number and palette section
    uint8_t damage_modifier_1   # modifier for damage (part 1)
    uint8_t damage_modifier_2   # modifier for damage (part 2)


class SMPLayerType(Enum):
    """
    SMP layer types.
    """
    MAIN    = "main"
    SHADOW  = "shadow"
    OUTLINE = "outline"


cdef public dict LAYER_TYPES = {
    0: SMPLayerType.MAIN,
    1: SMPLayerType.SHADOW,
    2: SMPLayerType.OUTLINE,
}


class SMP:
    """
    Class for reading/converting the SMP image format (successor of SLP).
    This format is used to store all graphics within AoE2: Definitive Edition.
    """

    # struct smp_header {
    #   char         signature[4];
    #   unsigned int version;
    #   unsigned int frame_count;
    #   unsigned int facet_count;
    #   unsigned int frames_per_facet;
    #   unsigned int checksum;
    #   unsigned int file_size;
    #   unsigned int source_format;
    #   char         comment[32];
    # };
    smp_header = Struct(endianness + "4s 7I 32s")

    # struct smp_frame_header_offset {
    #   unsigned int frame_info_offset;
    # };
    smp_frame_header_offset = Struct(endianness + "I")

    # struct smp_frame_header_size {
    #   padding 28 bytes;
    #   int frame_header_size;
    # };
    smp_frame_header_size = Struct(endianness + "28x i")

    # struct smp_layer_header {
    #   int          width;
    #   int          height;
    #   int          hotspot_x;
    #   int          hotspot_y;
    #   int          layer_type;
    #   unsigned int outline_table_offset;
    #   unsigned int qdl_table_offset;
    #   ???          flags;
    # };
    smp_layer_header = Struct(endianness + "i i i i I I I I")

    def __init__(self, data):
        smp_header = SMP.smp_header.unpack_from(data)
        signature, version, frame_count, facet_count, frames_per_facet,\
            checksum, file_size, source_format, comment = smp_header

        dbg("SMP")
        dbg(" frame count:          %s", frame_count)
        dbg(" facet count:          %s", facet_count)
        dbg(" facets per animation: %s", frames_per_facet)
        dbg(" file size:            %s B", file_size)
        dbg(" source format:        %s", source_format)
        dbg(" comment:              %s", comment.decode('ascii'))

        # Frames store main graphic, shadow and outline layer headers
        frame_header_offsets = list()

        # read offsets of the smp frames
        for i in range(frame_count):
            frame_header_pointer = (SMP.smp_header.size +
                                    i * SMP.smp_frame_header_offset.size)

            frame_header_offset = SMP.smp_frame_header_offset.unpack_from(
                data, frame_header_pointer)[0]

            frame_header_offsets.append(frame_header_offset)

        # SMP graphic frames are created from overlaying
        # the main graphic layer with a shadow layer and
        # and (for units) an outline layer
        self.main_frames = list()
        self.shadow_frames = list()
        self.outline_frames = list()

        spam(SMPLayerHeader.repr_header())

        # read all smp_frame_header structs in a frame
        for frame_offset in frame_header_offsets:

            # how many layer headers are in the frame
            frame_header_size = SMP.smp_frame_header_size.unpack_from(
                data, frame_offset)[0]

            for i in range(1, frame_header_size + 1):
                layer_header_offset = (frame_offset +
                                       i * SMP.smp_layer_header.size)

                layer_header = SMPLayerHeader(*SMP.smp_layer_header.unpack_from(
                    data, layer_header_offset), frame_offset)

                if layer_header.layer_type == 0x02:
                    # layer that store the main graphic
                    self.main_frames.append(SMPMainLayer(layer_header, data))

                elif layer_header.layer_type == 0x04:
                    # layer that stores a shadow
                    self.shadow_frames.append(SMPShadowLayer(layer_header, data))

                elif layer_header.layer_type == 0x08 or \
                     layer_header.layer_type == 0x10:
                    # layer that stores an outline
                    self.outline_frames.append(SMPOutlineLayer(layer_header, data))

                else:
                    raise Exception(
                        f"unknown layer type: {layer_header.layer_type:#x} at offset {layer_header_offset:#x}"
                    )
                spam(layer_header)

    def get_frames(self, layer: int = 0):
        """
        Get the frames in the SMP.

        :param layer: Position of the layer (see LAYER_TYPES)
                        - 0 = main graphics
                        - 1 = shadow graphics
                        - 2 = outline
        :type layer: int
        """
        cdef list frames

        layer_type = LAYER_TYPES.get(
            layer,
            SMPLayerType.MAIN
        )

        if layer_type is SMPLayerType.MAIN:
            frames = self.main_frames

        elif layer_type is SMPLayerType.SHADOW:
            frames = self.shadow_frames

        elif layer_type is SMPLayerType.OUTLINE:
            frames = self.outline_frames

        else:
            frames = []

        return frames

    def __str__(self):
        ret = list()

        ret.extend([repr(self), "\n", SMPLayerHeader.repr_header(), "\n"])
        for frame in self.main_frames:
            ret.extend([repr(frame), "\n"])
        return "".join(ret)

    def __repr__(self):
        return f"SMP image<{len(self.main_frames):d} frames>"


class SMPLayerHeader:
    def __init__(self, width, height, hotspot_x,
                 hotspot_y, layer_type, outline_table_offset,
                 qdl_table_offset, flags,
                 frame_offset):

        self.size = (width, height)
        self.hotspot = (hotspot_x, hotspot_y)

        # 2 = normal, 4 = shadow, 8 = outline
        self.layer_type = layer_type

        # table offsets are relative to the frame offset
        self.outline_table_offset = outline_table_offset + frame_offset
        self.qdl_table_offset = qdl_table_offset + frame_offset

        # the absolute offset of the frame
        self.frame_offset = frame_offset

        self.palette_number = -1

    @staticmethod
    def repr_header():
        return ("width x height | hotspot x/y | "
                "layer type | "
                "offset (outline table|qdl table)"
                )

    def __repr__(self):
        ret = (
            "% 5d x% 7d | " % self.size,
            "% 4d /% 5d | " % self.hotspot,
            "% 4d | " % self.layer_type,
            "% 13d| " % self.outline_table_offset,
            "        % 9d|" % self.qdl_table_offset,
        )
        return "".join(ret)

cdef class SMPLayer:
    """
    one layer inside the SMP. you can imagine it as a frame of a video.
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

    # layer and frame information
    cdef object info

    # for each row:
    # contains (left, right, full_row) number of boundary pixels
    cdef vector[boundary_def] boundaries

    # stores the file offset for the first drawing command
    cdef vector[int] cmd_offsets

    # pixel matrix representing the final image
    cdef vector[vector[pixel]] pcolor

    def __init__(self, layer_header, data):
        self.info = layer_header

        if not (isinstance(data, bytes) or isinstance(data, bytearray)):
            raise ValueError("Layer data must be some bytes object")

        # memory pointer
        # convert the bytes obj to char*
        cdef const uint8_t[::1] data_raw = data

        cdef unsigned short left
        cdef unsigned short right

        cdef size_t i
        cdef int cmd_offset

        cdef size_t row_count = self.info.size[1]
        self.pcolor.reserve(row_count)

        # process bondary table
        for i in range(row_count):
            outline_entry_position = (self.info.outline_table_offset +
                                      i * SMPLayer.smp_frame_row_edge.size)

            left, right = SMPLayer.smp_frame_row_edge.unpack_from(
                data, outline_entry_position
            )

            # is this row completely transparent?
            if left == 0xFFFF or right == 0xFFFF:
                self.boundaries.push_back(boundary_def(0, 0, True))
            else:
                self.boundaries.push_back(boundary_def(left, right, False))

        # process cmd table
        for i in range(row_count):
            cmd_table_position = (self.info.qdl_table_offset +
                                  i * SMPLayer.smp_command_offset.size)

            cmd_offset = SMPLayer.smp_command_offset.unpack_from(
                data, cmd_table_position)[0] + self.info.frame_offset
            self.cmd_offsets.push_back(cmd_offset)

        for i in range(row_count):
            self.pcolor.push_back(self.create_color_row(data_raw, i))

    cdef vector[pixel] create_color_row(self,
                                        const uint8_t[::1] &data_raw,
                                        Py_ssize_t rowid):
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
                row_data.push_back(pixel(color_transparent, 0, 0, 0, 0))

            return row_data

        # start drawing the left transparent space
        for i in range(bounds.left):
            row_data.push_back(pixel(color_transparent, 0, 0, 0, 0))

        # process the drawing commands for this row.
        self.process_drawing_cmds(data_raw,
                                  row_data, rowid,
                                  first_cmd_offset,
                                  pixel_count - bounds.right)

        # finish by filling up the right transparent space
        for i in range(bounds.right):
            row_data.push_back(pixel(color_transparent, 0, 0, 0, 0))

        # verify size of generated row
        if row_data.size() != pixel_count:
            got = row_data.size()
            summary = (
                f"{got:d}/{pixel_count:d} -> row {rowid:d}, "
                f"layer type {self.info.layer_type:x}, "
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
                                   const uint8_t[::1] &data_raw,
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
        Return the layer's hotspot (the "center" of the image)
        """
        return self.info.hotspot

    def get_palette_number(self):
        """
        Return the layer's palette number.

        :return: Palette number of the layer.
        :rtype: int
        """
        return self.pcolor[0][0].palette & 0b00111111

    def __repr__(self):
        return repr(self.info)


cdef class SMPMainLayer(SMPLayer):
    """
    SMPLayer for the main graphics sprite.
    """

    def __init__(self, layer_header, data):
        super().__init__(layer_header, data)

    @cython.boundscheck(False)
    cdef void process_drawing_cmds(self,
                                   const uint8_t[::1] &data_raw,
                                   vector[pixel] &row_data,
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
        cdef uint8_t lower_crumb
        cdef int pixel_count

        cdef vector[uint8_t] pixel_data
        pixel_data.reserve(4)

        # work through commands till end of row.
        while not eor:
            if row_data.size() > expected_size:
                raise Exception(
                    f"Only {expected_size:d} pixels should be drawn in row {rowid:d} " +
                    f"with layer type {self.info.layer_type:#x}, but we have {row_data.size():d}"
                )

            # fetch drawing instruction
            cmd = data_raw[dpos]

            # Last 2 bits store command type
            lower_crumb = 0b00000011 & cmd

            # opcode: cmd, rowid: rowid

            if lower_crumb == 0b00000011:
                # eol (end of line) command, this row is finished now.
                eor = True

                continue

            elif lower_crumb == 0b00000000:
                # skip command
                # draw 'count' transparent pixels
                # count = (cmd >> 2) + 1

                pixel_count = (cmd >> 2) + 1

                for _ in range(pixel_count):
                    row_data.push_back(pixel(color_transparent, 0, 0, 0, 0))

            elif lower_crumb == 0b00000001:
                # color_list command
                # draw the following 'count' pixels
                # pixels are stored as 4 byte palette and meta infos
                # count = (cmd >> 2) + 1

                pixel_count = (cmd >> 2) + 1

                for _ in range(pixel_count):
                    for _ in range(4):
                        dpos += 1
                        pixel_data.push_back(data_raw[dpos])

                    row_data.push_back(pixel(color_standard,
                                             pixel_data[0],
                                             pixel_data[1],
                                             pixel_data[2],
                                             pixel_data[3] & 0x1F)) # remove "usage" bit here

                    pixel_data.clear()

            elif lower_crumb == 0b00000010:
                # player_color command
                # draw the following 'count' pixels
                # pixels are stored as 4 byte palette and meta infos
                # count = (cmd >> 2) + 1

                pixel_count = (cmd >> 2) + 1

                for _ in range(pixel_count):
                    for _ in range(4):
                        dpos += 1
                        pixel_data.push_back(data_raw[dpos])

                    row_data.push_back(pixel(color_player,
                                             pixel_data[0],
                                             pixel_data[1],
                                             pixel_data[2],
                                             pixel_data[3] & 0x1F)) # remove "usage" bit here

                    pixel_data.clear()

            else:
                raise Exception(
                    f"unknown smp main graphics layer drawing command: " +
                    f"{cmd:#x} in row {rowid:d}"
                )

            # process next command
            dpos += 1

        # end of row reached, return the created pixel array.
        return

    def get_damage_mask(self):
        """
        Convert the 4th pixel byte to a mask used for damaged units.
        """
        return determine_damage_matrix(self.pcolor)


cdef class SMPShadowLayer(SMPLayer):
    """
    SMPLayer for the shadow graphics.
    """

    def __init__(self, layer_header, data):
        super().__init__(layer_header, data)

    @cython.boundscheck(False)
    cdef void process_drawing_cmds(self,
                                   const uint8_t[::1] &data_raw,
                                   vector[pixel] &row_data,
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
        cdef uint8_t lower_crumb
        cdef int pixel_count

        # work through commands till end of row.
        while not eor:
            if row_data.size() > expected_size:
                raise Exception(
                    f"Only {expected_size:d} pixels should be drawn in row {rowid:d} " +
                    f"with layer type {self.info.layer_type:#x}, but we have {row_data.size():d} " +
                    f"already!"
                )

            # fetch drawing instruction
            cmd = data_raw[dpos]

            # Last 2 bits store command type
            lower_crumb = 0b00000011 & cmd

            # opcode: cmd, rowid: rowid

            if lower_crumb == 0b00000011:
                # eol (end of line) command, this row is finished now.
                eor = True

                # shadows sometimes need an extra pixel at
                # the end
                if row_data.size() < expected_size:
                    # copy the last drawn pixel
                    # (still stored in nextbyte)
                    #
                    # TODO: confirm that this is the
                    #       right way to do it
                    row_data.push_back(pixel(color_shadow,
                                             nextbyte, 0, 0, 0))

                continue

            elif lower_crumb == 0b00000000:
                # skip command
                # draw 'count' transparent pixels
                # count = (cmd >> 2) + 1

                pixel_count = (cmd >> 2) + 1

                for _ in range(pixel_count):
                    row_data.push_back(pixel(color_transparent, 0, 0, 0, 0))

            elif lower_crumb == 0b00000001:
                # color_list command
                # draw the following 'count' pixels
                # pixels are stored as 1 byte alpha values
                # count = (cmd >> 2) + 1

                pixel_count = (cmd >> 2) + 1

                for _ in range(pixel_count):

                    dpos += 1
                    nextbyte = data_raw[dpos]

                    row_data.push_back(pixel(color_shadow,
                                             nextbyte, 0, 0, 0))

            else:
                raise Exception(
                    f"unknown smp shadow layer drawing command: " +
                    f"{cmd:#x} in row {rowid:d}")

            # process next command
            dpos += 1

        # end of row reached, return the created pixel array.
        return


cdef class SMPOutlineLayer(SMPLayer):
    """
    SMPLayer for the outline graphics.
    """

    def __init__(self, layer_header, data):
        super().__init__(layer_header, data)

    @cython.boundscheck(False)
    cdef void process_drawing_cmds(self,
                                   const uint8_t[::1] &data_raw,
                                   vector[pixel] &row_data,
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
        cdef uint8_t lower_crumb
        cdef int pixel_count

        # work through commands till end of row.
        while not eor:
            if row_data.size() > expected_size:
                raise Exception(
                    f"Only {expected_size:d} pixels should be drawn in row {rowid:d} " +
                    f"with layer type {self.info.layer_type:#x}, but we have {row_data.size():d} "
                    f"already!"
                )

            # fetch drawing instruction
            cmd = data_raw[dpos]

            # Last 2 bits store command type
            lower_crumb = 0b00000011 & cmd

            # opcode: cmd, rowid: rowid

            if lower_crumb == 0b00000011:
                # eol (end of line) command, this row is finished now.
                eor = True

                continue

            elif lower_crumb == 0b00000000:
                # skip command
                # draw 'count' transparent pixels
                # count = (cmd >> 2) + 1

                pixel_count = (cmd >> 2) + 1

                for _ in range(pixel_count):
                    row_data.push_back(pixel(color_transparent, 0, 0, 0, 0))

            elif lower_crumb == 0b00000001:
                # color_list command
                # draw the following 'count' pixels
                # as player outline colors.
                # count = (cmd >> 2) + 1

                pixel_count = (cmd >> 2) + 1

                for _ in range(pixel_count):
                    # we don't know the color the game wants
                    # so we just draw index 0
                    row_data.push_back(pixel(color_outline,
                                             0, 0, 0, 0))

            else:
                raise Exception(
                    f"unknown smp outline layer drawing command: " +
                    f"{cmd:#x} in row {rowid:d}")
            # process next command
            dpos += 1

        # end of row reached, return the created pixel array.
        return


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

    cdef uint8_t m_color_size = len(m_lookup[0])

    cdef uint8_t r
    cdef uint8_t g
    cdef uint8_t b
    cdef uint8_t alpha

    cdef vector[pixel] current_row
    cdef pixel px
    cdef pixel_type px_type
    cdef uint8_t px_index
    cdef uint8_t px_palette

    cdef uint16_t palette_section

    cdef size_t x
    cdef size_t y

    for y in range(height):

        current_row = image_matrix[y]

        for x in range(width):
            px = current_row[x]
            px_type = px.type
            px_index = px.index
            px_palette = px.palette

            if px_type == color_standard:
                # look up the palette secition
                # palettes have 1024 entries
                # divided into 4 sections
                palette_section = px_palette & 0x03

                # the index has to be adjusted
                # to the palette section
                index = px_index + (palette_section * 256)

                # look up the color index in the
                # main graphics table
                if m_color_size == 3:
                    # RGB tables (leftover from HD edition)
                    r = m_lookup[index][0]
                    g = m_lookup[index][1]
                    b = m_lookup[index][2]

                elif m_color_size == 4:
                    # RGBA tables (but alpha is often unused)
                    r = m_lookup[index][0]
                    g = m_lookup[index][1]
                    b = m_lookup[index][2]
                    alpha = m_lookup[index][3]

                # alpha values are unused
                # in 0x0C and 0x0B version of SMPs
                alpha = 255

            elif px_type == color_transparent:
                r, g, b, alpha = 0, 0, 0, 0

            elif px_type == color_shadow:
                r, g, b, alpha = 0, 0, 0, px_index

                # change alpha values to match openage texture formats
                # even alphas are used for commands marking *special* pixels (player color, etc.)
                # odd alphas are used for normal pixels (= displayed as-is with transparency)
                alpha = alpha | 0x01

            else:
                if px_type == color_player:
                    alpha = 254

                elif px_type == color_outline:
                    alpha = 252

                else:
                    raise ValueError("unknown pixel type: %d" % px_type)

                # Store player color index in g channel
                r, b = 0, 0
                g = px_index

            # array_data[y, x] = (r, g, b, alpha)
            array_data[y, x, 0] = r
            array_data[y, x, 1] = g
            array_data[y, x, 2] = b
            array_data[y, x, 3] = alpha

    return array_data

@cython.boundscheck(False)
@cython.wraparound(False)
cdef numpy.ndarray determine_damage_matrix(vector[vector[pixel]] &image_matrix):
    """
    converts the damage modifier values to an image using the RG values.

    :param image_matrix: A 2-dimensional array of SMP pixels.
    """

    cdef size_t height = image_matrix.size()
    cdef size_t width = image_matrix[0].size()

    cdef numpy.ndarray[numpy.uint8_t, ndim=3, mode="c"] array_data = \
        numpy.zeros((height, width, 4), dtype=numpy.uint8)

    cdef uint8_t r
    cdef uint8_t g
    cdef uint8_t b
    cdef uint8_t alpha

    cdef vector[pixel] current_row
    cdef pixel px

    cdef size_t x
    cdef size_t y

    for y in range(height):

        current_row = image_matrix[y]

        for x in range(width):
            px = current_row[x]

            r, g, b, alpha = px.damage_modifier_1, px.damage_modifier_2, 0, 255

            # array_data[y, x] = (r, g, b, alpha)
            array_data[y, x, 0] = r
            array_data[y, x, 1] = g
            array_data[y, x, 2] = b
            array_data[y, x, 3] = alpha

    return array_data
