# Copyright 2019-2025 the openage authors. See copying.md for legal info.
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



# SMX files have little endian byte order
endianness = "< "


# Boundary of a row in a SMX layer.
cdef struct boundary_def:
    Py_ssize_t left
    Py_ssize_t right
    bool full_row


# SMX uses SMP pixel tytes
cdef enum pixel_type:
    color_standard      # standard pixel
    color_shadow        # shadow pixel
    color_transparent   # transparent pixel
    color_player        # non-outline player color pixel
    color_outline       # player color outline pixel


# One uncompressed SMP pixel.
cdef struct pixel:
    pixel_type type
    uint8_t index               # index in a palette section
    uint8_t palette             # palette number and palette section
    uint8_t damage_modifier_1   # modifier for damage (part 1)
    uint8_t damage_modifier_2   # modifier for damage (part 2)


class SMXLayerType(Enum):
    """
    SMX layer types.
    """
    MAIN    = "main"
    SHADOW  = "shadow"
    OUTLINE = "outline"


cdef public dict LAYER_TYPES = {
    0: SMXLayerType.MAIN,
    1: SMXLayerType.SHADOW,
    2: SMXLayerType.OUTLINE,
}


cdef class SMXMainLayer8to5:
    """
    Main graphics layer of an SMX (compressed with 8to5).
    """
    pass

cdef class SMXMainLayer4plus1:
    """
    Main graphics layer of an SMX (compressed with 4plus1).
    """
    pass

cdef class SMXShadowLayer:
    """
    Shadow layer of an SMX.
    """
    pass

cdef class SMXOutlineLayer:
    """
    Outline layer of an SMX.
    """
    pass


# fused type for SMX layer variants
ctypedef fused SMXLayerVariant:
    SMXMainLayer8to5
    SMXMainLayer4plus1
    SMXShadowLayer
    SMXOutlineLayer


class SMX:
    """
    Class for reading/converting compressed SMX files (delivered
    with AoE2:DE by default).
    """

    # struct smx_header {
    #   char[4]        signature;
    #   unsigned short version;
    #   unsigned short frame_count;
    #   unsigned int   file_size_comp;
    #   unsigned int   file_size_uncomp;
    #   char[16]       comment;
    # };
    smx_header = Struct(endianness + "4s H H I I 16s")

    # struct smx_frame_header {
    #   unsigned char frame_type;
    #   unsigned char palette_index;
    #   unsigned int  uncomp_size;
    # };
    smx_frame_header = Struct(endianness + "B B I")

    # struct smx_layer_header {
    #   unsigned short width;
    #   unsigned short height;
    #   short          hotspot_x;
    #   short          hotspot_y;
    #   unsigned int   distance_next_frame;
    #   int            4 bytes;
    # };
    smx_layer_header = Struct(endianness + "H H h h I i")

    def __init__(self, data: bytes):
        """
        Read an SMX image file and store the frames in the object.

        :param data: SMX file data.
        """
        smx_header = SMX.smx_header.unpack_from(data)
        self.smp_type, version, frame_count, file_size_comp,\
            file_size_uncomp, comment = smx_header

        dbg("SMX")
        dbg(" version:                %s",   version)
        dbg(" frame count:            %s",   frame_count)
        dbg(" file size compressed:   %s B", file_size_comp + 0x20)   # 0x20 = SMX header size
        dbg(" file size uncompressed: %s B", file_size_uncomp + 0x40) # 0x80 = SMP header size
        dbg(" comment:                %s",   comment.decode('ascii'))

        # SMX graphic frames are created from overlaying
        # the main graphic frame with a shadow layer and
        # and (for units) an outline layer
        self.main_frames = list()
        self.shadow_frames = list()
        self.outline_frames = list()

        spam(SMXLayerHeader.repr_header())

        # SMX files have no offsets, we have to calculate them
        current_offset = SMX.smx_header.size
        for i in range(frame_count):
            frame_header = SMX.smx_frame_header.unpack_from(
                data, current_offset)

            frame_type , palette_number, _ = frame_header

            current_offset += SMX.smx_frame_header.size

            layer_types = []

            if frame_type & 0x01:
                layer_types.append(SMXLayerType.MAIN)

            if frame_type & 0x02:
                layer_types.append(SMXLayerType.SHADOW)

            if frame_type & 0x04:
                layer_types.append(SMXLayerType.OUTLINE)

            for layer_type in layer_types:
                layer_header_data = SMX.smx_layer_header.unpack_from(
                    data, current_offset)

                width, height, hotspot_x, hotspot_y,\
                    distance_next_frame, _ = layer_header_data

                current_offset += SMX.smx_layer_header.size

                outline_table_offset = current_offset

                # Skip outline table
                current_offset += 4 * height

                qdl_command_array_size = Struct("< I").unpack_from(data, current_offset)[0]
                current_offset += 4

                # Read length of color table
                if layer_type is SMXLayerType.MAIN:
                    qdl_color_table_size = Struct("< I").unpack_from(data, current_offset)[0]
                    current_offset += 4
                    qdl_color_table_offset = current_offset + qdl_command_array_size

                else:
                    qdl_color_table_size = 0
                    qdl_color_table_offset = current_offset

                qdl_command_table_offset = current_offset
                current_offset += qdl_color_table_size + qdl_command_array_size

                layer_header = SMXLayerHeader(layer_type, frame_type,
                                              palette_number,
                                              width, height, hotspot_x,
                                              hotspot_y, outline_table_offset,
                                              qdl_command_table_offset,
                                              qdl_color_table_offset)

                if layer_type is SMXLayerType.MAIN:
                    if layer_header.compression_type == 0x08:
                        self.main_frames.append(SMXLayer(SMXMainLayer8to5(), layer_header, data))

                    elif layer_header.compression_type == 0x00:
                        self.main_frames.append(SMXLayer(SMXMainLayer4plus1(), layer_header, data))

                elif layer_type is SMXLayerType.SHADOW:
                    self.shadow_frames.append(SMXLayer(SMXShadowLayer(), layer_header, data))

                elif layer_type is SMXLayerType.OUTLINE:
                    self.outline_frames.append(SMXLayer(SMXOutlineLayer(), layer_header, data))

    def get_frames(self, layer: int = 0) -> list[SMXLayer]:
        """
        Get the frames in the SMX.

        :param layer: Position of the layer (see LAYER_TYPES)
                        - 0 = main graphics
                        - 1 = shadow graphics
                        - 2 = outline
        """
        cdef list frames

        layer_type = LAYER_TYPES.get(
            layer,
            SMXLayerType.MAIN
        )

        if layer_type is SMXLayerType.MAIN:
            frames = self.main_frames

        elif layer_type is SMXLayerType.SHADOW:
            frames = self.shadow_frames

        elif layer_type is SMXLayerType.OUTLINE:
            frames = self.outline_frames

        else:
            frames = []

        return frames

    def __str__(self):
        ret = list()

        ret.extend([repr(self), "\n", SMXLayerHeader.repr_header(), "\n"])
        for frame in self.main_frames:
            ret.extend([repr(frame), "\n"])
        return "".join(ret)

    def __repr__(self):
        return f"{self.smp_type} image<{len(self.main_frames):d} frames>"


class SMXLayerHeader:
    def __init__(
        self,
        layer_type: SMXLayerType,
        frame_type: int,
        palette_number: int,
        width: int,
        height: int,
        hotspot_x: int,
        hotspot_y: int,
        outline_table_offset: int,
        qdl_command_table_offset: int,
        qdl_color_table_offset: int
    ) -> None:
        """
        Stores the header of a layer including additional info about its frame.

        :param layer_type: Type of layer.
        :param frame_type: Type of the frame the layer belongs to.
        :param palette_number: Palette number used for pixels in the frame.
        :param width: Width of layer in pixels.
        :param height: Height of layer in pixels.
        :param hotspot_x: x coordinate of the hotspot used for anchoring.
        :param hotspot_y: y coordinate of the hotspot used for anchoring.
        :param outline_table_offset: Absolute position of the layer's outline table in the file.
        :param qdl_command_table_offset: Absolute position of the layer's command table in the file.
        :param qdl_color_table_offset: Absolute position of the layer's pixel data table in the file.
        """

        self.size = (width, height)
        self.hotspot = (hotspot_x, hotspot_y)

        self.layer_type = layer_type
        self.frame_type = frame_type
        self.compression_type = frame_type & 8
        self.palette_number = palette_number

        self.outline_table_offset = outline_table_offset
        self.qdl_command_table_offset = qdl_command_table_offset
        self.qdl_color_table_offset = qdl_color_table_offset

    @staticmethod
    def repr_header() -> str:
        return ("layer type | width x height | "
                "hotspot x/y | "
                )

    def __repr__(self) -> str:
        ret = (
            "% s | " % self.layer_type,
            "% 5d x% 7d | " % self.size,
            "% 4d /% 5d | " % self.hotspot,
            "% 13d | " % self.outline_table_offset,
            "        % 9d |" % self.qdl_command_table_offset,
            "        % 9d |" % self.qdl_color_table_offset,
        )
        return "".join(ret)


cdef class SMXLayer:
    """
    Layer inside the compressed SMX.
    """

    # struct smp_layer_row_edge {
    #   unsigned short left_space;
    #   unsigned short right_space;
    # };
    smp_layer_row_edge = Struct(endianness + "H H")

    # layer and frame information
    cdef object info

    # for each row:
    # contains (left, right, full_row) number of boundary pixels
    cdef vector[boundary_def] boundaries

    # pixel matrix representing the final image
    cdef vector[vector[pixel]] pcolor

    def __init__(
        self,
        variant, # this argument must not be typed because cython can't handle it
        layer_header: SMXLayerHeader,
        data: bytes
    ) -> None:
        """
        Create a SMX layer.

        :param variant: Type of the layer.
        :param layer_header: Header information of the layer.
        :param data: Layer data.
        """
        self.init(variant, layer_header, data)

    def init(self, SMXLayerVariant variant, layer_header: SMXLayerHeader, data: bytes) -> None:
        """
        SMX layer definition. There can be various types of layers inside an SMX frame.

        :param variant: Type of the layer.
        :param layer_header: Header definition of the layer.
        :param data: File content as bytes.
        """
        self.info = layer_header

        if not (isinstance(data, bytes) or isinstance(data, bytearray)):
            raise ValueError("Layer data must be some bytes object")

        # memory pointer
        # convert the bytes obj to char*
        cdef const uint8_t[::1] data_raw = data

        cdef unsigned short left
        cdef unsigned short right

        cdef size_t i
        cdef size_t row_count = self.info.size[1]
        self.pcolor.reserve(row_count)

        # process bondary table
        for i in range(row_count):
            outline_entry_position = (self.info.outline_table_offset +
                                      i * SMXLayer.smp_layer_row_edge.size)

            left, right = SMXLayer.smp_layer_row_edge.unpack_from(
                data, outline_entry_position
            )

            # is this row completely transparent?
            if left == 0xFFFF or right == 0xFFFF:
                self.boundaries.push_back(boundary_def(0, 0, True))
            else:
                self.boundaries.push_back(boundary_def(left, right, False))

        cdef int cmd_offset = self.info.qdl_command_table_offset
        cdef int color_offset = self.info.qdl_color_table_offset
        cdef int chunk_pos = 0

        # process cmd table
        for i in range(row_count):
            cmd_offset, color_offset, chunk_pos, row_data = \
                self.create_color_row(variant, data_raw, i, cmd_offset, color_offset, chunk_pos)
            self.pcolor.push_back(row_data)


    cdef inline (int, int, int, vector[pixel]) create_color_row(self,
                                                                SMXLayerVariant variant,
                                                                const uint8_t[::1] &data_raw,
                                                                Py_ssize_t rowid,
                                                                int cmd_offset,
                                                                int color_offset,
                                                                int chunk_pos):
        """
        Extract colors (pixels) for a pixel row in the layer.

        :param variant: Type of the layer.
        :param data_raw: Raw data of the layer.
        :param rowid: Index of the current row in the layer.
        :param cmd_offset: Offset of the command table of the layer.
        :param color_offset: Offset of the color table of the layer.
        :param chunk_pos: Current position in the compressed chunk.
        """

        cdef vector[pixel] row_data
        cdef Py_ssize_t i

        cdef int first_cmd_offset = cmd_offset
        cdef int first_color_offset = color_offset
        cdef boundary_def bounds = self.boundaries[rowid]
        cdef size_t pixel_count = self.info.size[0]

        # preallocate memory
        row_data.reserve(pixel_count)

        # row is completely transparent
        if bounds.full_row:
            for _ in range(pixel_count):
                row_data.push_back(pixel(color_transparent, 0, 0, 0, 0))

            return cmd_offset, color_offset, chunk_pos, row_data

        # start drawing the left transparent space
        for i in range(bounds.left):
            row_data.push_back(pixel(color_transparent, 0, 0, 0, 0))

        # process the drawing commands for this row.
        next_cmd_offset, next_color_offset, chunk_pos, row_data = \
            self.process_drawing_cmds(
                variant,
                data_raw,
                row_data,
                rowid,
                first_cmd_offset,
                first_color_offset,
                chunk_pos,
                pixel_count - bounds.right
            )

        # finish by filling up the right transparent space
        for i in range(bounds.right):
            row_data.push_back(pixel(color_transparent, 0, 0, 0, 0))

        # verify size of generated row
        if row_data.size() != pixel_count:
            got = row_data.size()
            summary = "%d/%d -> row %d, layer type %s, offset %d / %#x" % (
                got, pixel_count, rowid, repr(self.info.layer_type),
                first_cmd_offset, first_cmd_offset
                )
            txt = "got %%s pixels than expected: %s, missing: %d" % (
                summary, abs(pixel_count - got))

            raise Exception(txt % ("LESS" if got < pixel_count else "MORE"))

        return next_cmd_offset, next_color_offset, chunk_pos, row_data


    @cython.boundscheck(False)
    cdef inline (int, int, int, vector[pixel]) process_drawing_cmds(self,
                                                                    SMXLayerVariant variant,
                                                                    const uint8_t[::1] &data_raw,
                                                                    vector[pixel] &row_data,
                                                                    Py_ssize_t rowid,
                                                                    Py_ssize_t first_cmd_offset,
                                                                    Py_ssize_t first_color_offset,
                                                                    int chunk_pos,
                                                                    size_t expected_size):
        """
        extract colors (pixels) for the drawing commands that were
        compressed with 8to5 compression.

        :param variant: Type of the layer.
        :param data_raw: Raw data of the layer.
        :param row_data: Stores the extracted pixels. May be prefilled with transparent pixels.
        :param rowid: Row index.
        :param first_cmd_offset: Offset of the first drawing command in the data.
        :param first_color_offset: Offset of the first color command in the data.
        :param chunk_pos: Current position in the compressed chunk.
        :param expected_size: Expected number of pixels in the row.
        """
        # position in the command array, we start at the first command of this row
        cdef Py_ssize_t dpos_cmd = first_cmd_offset

        # Position in the pixel data array
        cdef Py_ssize_t dpos_color = first_color_offset

        # Position in the compression chunk.
        cdef bool odd = chunk_pos
        cdef int px_dpos = 0 # For loop iterator

        # is the end of the current row reached?
        cdef bool eor = False

        cdef uint8_t cmd = 0
        cdef uint8_t lower_crumb = 0
        cdef int pixel_count = 0
        cdef vector[uint8_t] pixel_data
        pixel_data.reserve(4)

        # Pixel data temporary values that need further decompression
        cdef uint8_t pixel_data_odd_0 = 0
        cdef uint8_t pixel_data_odd_1 = 0
        cdef uint8_t pixel_data_odd_2 = 0
        cdef uint8_t pixel_data_odd_3 = 0

        # Mask for even indices
        # cdef uint8_t pixel_mask_even_0 = 0xFF
        cdef uint8_t pixel_mask_even_1 = 0b00000011
        cdef uint8_t pixel_mask_even_2 = 0b11110000
        cdef uint8_t pixel_mask_even_3 = 0b00111111

        if SMXLayerVariant is SMXMainLayer8to5 or SMXLayerVariant is SMXMainLayer4plus1:
            # Position in the pixel data array
            dpos_color = first_color_offset

        cdef uint8_t palette_section_block = 0
        cdef uint8_t palette_section = 0
        cdef uint8_t nextbyte = 0

        # work through commands till end of row.
        while not eor:
            if row_data.size() > expected_size:
                raise Exception(
                    f"Only {expected_size:d} pixels should be drawn in row {rowid:d} "
                    f"with layer type {self.info.layer_type:#x}, but we have {row_data.size():d} "
                    f"already!"
                )

            # fetch drawing instruction
            cmd = data_raw[dpos_cmd]

            # Last 2 bits store command type
            lower_crumb = 0b00000011 & cmd

            if lower_crumb == 0b00000011:
                # eor (end of row) command, this row is finished now.
                eor = True
                dpos_cmd += 1

                # shadows sometimes need an extra pixel at
                # the end
                if SMXLayerVariant is SMXShadowLayer:
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
                # pixels are stored in 5 byte chunks
                # even pixel indices have their info stored
                # in byte[0] - byte[3]. odd pixel indices have
                # their info stored in byte[1] - byte[4].
                # count = (cmd >> 2) + 1

                pixel_count = (cmd >> 2) + 1

                if SMXLayerVariant is SMXMainLayer8to5:
                    pixel_data.reserve(4)
                    for _ in range(pixel_count):
                        # Start fetching pixel data
                        if odd:
                            # Odd indices require manual extraction of each of the 4 values

                            # Palette index. Essentially a rotation of (byte[1]byte[2])
                            # by 6 to the left, then masking with 0x00FF.
                            pixel_data_odd_0 = data_raw[dpos_color + 1]
                            pixel_data_odd_1 = data_raw[dpos_color + 2]
                            pixel_data.push_back((pixel_data_odd_0 >> 2) | (pixel_data_odd_1 << 6))

                            # Palette section. Described in byte[2] in bits 4-5.
                            pixel_data.push_back((pixel_data_odd_1 >> 2) & 0x03)

                            # Damage mask 1. Essentially a rotation of (byte[3]byte[4])
                            # by 6 to the left, then masking with 0x00F0.
                            pixel_data_odd_2 = data_raw[dpos_color + 3]
                            pixel_data_odd_3 = data_raw[dpos_color + 4]
                            pixel_data.push_back(((pixel_data_odd_2 >> 2) | (pixel_data_odd_3 << 6)) & 0xF0)

                            # Damage mask 2. Described in byte[4] in bits 0-5.
                            pixel_data.push_back((pixel_data_odd_3 >> 2) & 0x3F)

                            row_data.push_back(pixel(color_standard,
                                                    pixel_data[0],
                                                    pixel_data[1],
                                                    pixel_data[2],
                                                    pixel_data[3]))

                            # Go to next pixel
                            dpos_color += 5

                        else:
                            # Even indices can be read "as is". They just have to be masked.
                            for i in range(4):
                                pixel_data.push_back(data_raw[dpos_color + i])

                            row_data.push_back(pixel(color_standard,
                                                    pixel_data[0],
                                                    pixel_data[1] & pixel_mask_even_1,
                                                    pixel_data[2] & pixel_mask_even_2,
                                                    pixel_data[3] & pixel_mask_even_3))

                        odd = not odd
                        pixel_data.clear()

                if SMXLayerVariant is SMXMainLayer4plus1:
                    palette_section_block = data_raw[dpos_color + (4 - chunk_pos)]

                    for _ in range(pixel_count):
                        # Start fetching pixel data
                        palette_section = (
                            palette_section_block >> (2 * chunk_pos)) & 0x03
                        row_data.push_back(pixel(color_standard,
                                                data_raw[dpos_color],
                                                palette_section,
                                                0,
                                                0))

                        dpos_color += 1
                        chunk_pos += 1

                        # Skip to next chunk
                        if chunk_pos > 3:
                            chunk_pos = 0
                            dpos_color += 1  # Skip palette section block
                            palette_section_block = data_raw[dpos_color + 4]

                if SMXLayerVariant is SMXShadowLayer:
                    for _ in range(pixel_count):
                        dpos_cmd += 1
                        nextbyte = data_raw[dpos_cmd]

                        row_data.push_back(pixel(color_shadow,
                                                nextbyte, 0, 0, 0))

                if SMXLayerVariant is SMXOutlineLayer:
                    # we don't know the color the game wants
                    # so we just draw index 0
                    for _ in range(pixel_count):
                        row_data.push_back(pixel(color_outline,
                                                0, 0, 0, 0))

            elif lower_crumb == 0b00000010:
                if SMXLayerVariant is SMXMainLayer8to5:
                    # player_color command
                    # draw the following 'count' pixels
                    # pixels are stored in 5 byte chunks
                    # even pixel indices have their info stored
                    # in byte[0] - byte[3]. odd pixel indices have
                    # their info stored in byte[1] - byte[4].
                    # count = (cmd >> 2) + 1

                    pixel_count = (cmd >> 2) + 1

                    for _ in range(pixel_count):
                        # Start fetching pixel data
                        if odd:
                            # Odd indices require manual extraction of each of the 4 values

                            # Palette index. Essentially a rotation of (byte[1]byte[2])
                            # by 6 to the left, then masking with 0x00FF.
                            pixel_data_odd_0 = data_raw[dpos_color + 1]
                            pixel_data_odd_1 = data_raw[dpos_color + 2]
                            pixel_data.push_back((pixel_data_odd_0 >> 2) | (pixel_data_odd_1 << 6))

                            # Palette section. Described in byte[2] in bits 4-5.
                            pixel_data.push_back((pixel_data_odd_1 >> 2) & 0x03)

                            # Damage modifier 1. Essentially a rotation of (byte[3]byte[4])
                            # by 6 to the left, then masking with 0x00F0.
                            pixel_data_odd_2 = data_raw[dpos_color + 3]
                            pixel_data_odd_3 = data_raw[dpos_color + 4]
                            pixel_data.push_back(((pixel_data_odd_2 >> 2) | (pixel_data_odd_3 << 6)) & 0xF0)

                            # Damage modifier 2. Described in byte[4] in bits 0-5.
                            pixel_data.push_back((pixel_data_odd_3 >> 2) & 0x3F)

                            row_data.push_back(pixel(color_player,
                                                    pixel_data[0],
                                                    pixel_data[1],
                                                    pixel_data[2],
                                                    pixel_data[3]))

                            # Go to next pixel
                            dpos_color += 5

                        else:
                            # Even indices can be read "as is". They just have to be masked.
                            for px_dpos in range(4):
                                pixel_data.push_back(data_raw[dpos_color + px_dpos])

                            row_data.push_back(pixel(color_player,
                                                    pixel_data[0],
                                                    pixel_data[1] & pixel_mask_even_1,
                                                    pixel_data[2] & pixel_mask_even_2,
                                                    pixel_data[3] & pixel_mask_even_3))

                        odd = not odd
                        pixel_data.clear()

                elif SMXLayerVariant is SMXMainLayer4plus1:
                    # player_color command
                    # draw the following 'count' pixels
                    # 4 pixels are stored in every 5 byte chunk.
                    # palette indices are contained in byte[0] - byte[3]
                    # palette sections are stored in byte[4]
                    # count = (cmd >> 2) + 1

                    pixel_count = (cmd >> 2) + 1

                    for _ in range(pixel_count):
                        # Start fetching pixel data
                        palette_section = (palette_section_block >> (2 * chunk_pos)) & 0x03
                        row_data.push_back(pixel(color_player,
                                                data_raw[dpos_color],
                                                palette_section,
                                                0,
                                                0))

                        dpos_color += 1
                        chunk_pos += 1

                        # Skip to next chunk
                        if chunk_pos > 3:
                            chunk_pos = 0
                            dpos_color += 1 # Skip palette section block
                            palette_section_block = data_raw[dpos_color + 4]

            else:
                raise Exception(
                    f"unknown smx main graphics layer drawing command: " +
                    f"{cmd:#x} in row {rowid:d}"
                )

            # Process next command
            dpos_cmd += 1

        if SMXLayerVariant is SMXMainLayer8to5 or SMXLayerVariant is SMXMainLayer4plus1:
            return dpos_cmd, dpos_color, chunk_pos, row_data
        elif SMXLayerVariant is SMXOutlineLayer or SMXLayerVariant is SMXShadowLayer:
            return dpos_cmd, dpos_cmd, chunk_pos, row_data


    def get_picture_data(self, palette) -> numpy.ndarray:
        """
        Convert the palette index matrix to a RGBA image.

        :param palette: Color palette used for pixels in the sprite.
        :type palette: .colortable.ColorTable
        :return: Array of RGBA values.
        """
        return determine_rgba_matrix(self.pcolor, palette)

    def get_hotspot(self) -> tuple[int, int]:
        """
        Return the layer's hotspot (the "center" of the image).

        :return: Hotspot of the layer.
        """
        return self.info.hotspot

    def get_palette_number(self) -> int:
        """
        Return the layer's palette number.

        :return: Palette number of the layer.
        """
        return self.info.palette_number

    def __repr__(self):
        return repr(self.info)


@cython.boundscheck(False)
@cython.wraparound(False)
cdef numpy.ndarray determine_rgba_matrix(vector[vector[pixel]] &image_matrix,
                                         numpy.ndarray[numpy.uint8_t, ndim=2, mode="c"] palette):
    """
    converts a palette index image matrix to an rgba matrix.

    :param image_matrix: A 2-dimensional array of SMP pixels.
    :param palette: Color palette used for normal pixels in the sprite.
    """

    cdef size_t height = image_matrix.size()
    cdef size_t width = image_matrix[0].size()

    cdef numpy.ndarray[numpy.uint8_t, ndim=3, mode="c"] array_data = \
        numpy.zeros((height, width, 4), dtype=numpy.uint8)

    cdef uint8_t[:, ::1] m_lookup = palette

    cdef uint8_t r = 0
    cdef uint8_t g = 0
    cdef uint8_t b = 0
    cdef uint8_t alpha = 0

    cdef vector[pixel] current_row
    cdef pixel px
    cdef pixel_type px_type
    cdef uint8_t px_index = 0
    cdef uint8_t px_palette = 0

    cdef uint16_t palette_section = 0

    cdef size_t x = 0
    cdef size_t y = 0

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
                palette_section = px_palette

                # the index has to be adjusted
                # to the palette section
                index = px_index + (palette_section * 256)

                # look up the color index in the
                # main graphics table
                r = m_lookup[index][0]
                g = m_lookup[index][1]
                b = m_lookup[index][2]
                alpha = m_lookup[index][3]

                # alpha values are unused
                # in 0x0C and 0x0B version of SMP/SMX
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
                    raise ValueError(f"unknown pixel type: {px_type:#x}")

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

    cdef uint8_t r = 0
    cdef uint8_t g = 0
    cdef uint8_t b = 0
    cdef uint8_t alpha = 0

    cdef vector[pixel] current_row
    cdef pixel px

    cdef size_t x = 0
    cdef size_t y = 0

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
