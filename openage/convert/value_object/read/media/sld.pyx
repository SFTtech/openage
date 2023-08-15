# Copyright 2022-2023 the openage authors. See copying.md for legal info.
#
# cython: infer_types=True

from enum import Enum
import numpy
from struct import Struct, unpack_from

from .....log import spam, dbg

cimport cython
cimport numpy

from libc.stdint cimport uint8_t
from libcpp cimport bool
from libcpp.pair cimport pair
from libcpp.vector cimport vector

# SLD files have little endian byte order
endianness = "< "

cdef struct pixel:
    uint8_t r
    uint8_t g
    uint8_t b
    uint8_t a


class SLDLayerType(Enum):
    """
    SLD layer types.
    """
    MAIN        = "main"
    SHADOW      = "shadow"
    OUTLINE     = "outline"
    DAMAGE      = "damage"
    PLAYERCOLOR = "playercolor"


cdef public dict LAYER_TYPES = {
    0: SLDLayerType.MAIN,
    1: SLDLayerType.SHADOW,
    2: SLDLayerType.OUTLINE,
    3: SLDLayerType.DAMAGE,
    4: SLDLayerType.PLAYERCOLOR,
}


cdef class SLD:
    """
    Class for reading/converting compressed SLD files (delivered
    with AoE2:DE since version 66692 by default).
    """

    # struct sld_header {
    #   char[4]        signature;
    #   unsigned short version;
    #   unsigned short frame_count;
    #   unsigned short unknown1;
    #   unsigned short unknown2;
    #   unsigned int   unknown3;
    # };
    sld_header = Struct(endianness + "4s 4H I")

    # struct sld_frame_header {
    #   unsigned short canvas_width;
    #   unsigned short canvas_height;
    #   unsigned short canvas_hotspot_x;
    #   unsigned short canvas_hotspot_y;
    #   char           frame_type;
    #   char           unknown5;
    #   unsigned short frame_index;
    # };
    sld_frame_header = Struct(endianness + "4H 2B H")

    # struct sld_layer_length {
    #   unsigned int length;
    # };
    sld_layer_length = Struct(endianness + "I")

    # struct sld_layer_header_graphics {
    #   unsigned short offset_x1;
    #   unsigned short offset_y1;
    #   unsigned short offset_x2;
    #   unsigned short offset_y2;
    #   char           unknown1;
    #   char           unknown2;
    # };
    sld_layer_header_graphics = Struct(endianness + "4H 2B")

    # struct sld_layer_header_mask {
    #   char           unknown1;
    #   char           unknown2;
    # };
    sld_layer_header_mask = Struct(endianness + "2B")

    cdef bytes sld_type
    cdef public list main_frames
    cdef public list shadow_frames
    cdef public list outline_frames
    cdef public list dmg_mask_frames
    cdef public list playercolor_mask_frames

    cdef const uint8_t[::1] data

    def __init__(self, data):
        """
        Read an SLD image file.

        :param data: File content as bytes.
        :type data: bytes, bytearray
        """

        sld_header = SLD.sld_header.unpack_from(data)
        self.sld_type = sld_header[0]
        version, frame_count, _, _, _ = sld_header[1:]

        dbg("SLD")
        dbg(" version:     %s",   version)
        dbg(" frame count: %s",   frame_count)

        # SLD graphic frames are created from several layers
        self.main_frames = list()
        self.shadow_frames = list()
        self.outline_frames = list()
        self.dmg_mask_frames = list()
        self.playercolor_mask_frames = list()

        # File bytes
        self.data = data

        # Reference to previous layer
        # SLD reuses their pixel data on some occasions
        cdef (unsigned short, unsigned short) previous_size = (0, 0)
        cdef (unsigned short, unsigned short) previous_offset = (0, 0)
        cdef vector[vector[pixel]] *previous_layer = NULL
        cdef SLDLayer previous_main
        cdef SLDLayer previous_shadow
        cdef SLDLayer previous_outline
        cdef SLDLayer previous_dmg_mask
        cdef SLDLayer previous_playercolor

        # Header info
        cdef SLDLayerHeader layer_header

        # Dimensions of main layer (reused for dmg mask and playercolor)
        cdef unsigned short main_width
        cdef unsigned short main_height
        cdef short main_hotspot_x
        cdef short main_hotspot_y

        # Our position in the file bytes
        cdef unsigned int current_offset

        spam(SLDLayerHeader.repr_header())

        # SLD files have no offsets, we have to calculate them
        # from length fields
        current_offset = SLD.sld_header.size
        for _ in range(frame_count):

            canvas_width, canvas_height, canvas_hotspot_x, canvas_hotspot_y,\
                frame_type , _, frame_index = SLD.sld_frame_header.unpack_from(
                data, current_offset)

            frame_header = SLDFrameHeader(
                canvas_width,
                canvas_height,
                canvas_hotspot_x,
                canvas_hotspot_y,
                frame_type,
                frame_index
            )

            current_offset += SLD.sld_frame_header.size

            layer_types = []

            if frame_type & 0x01:
                layer_types.append(SLDLayerType.MAIN)

            if frame_type & 0x02:
                layer_types.append(SLDLayerType.SHADOW)

            if frame_type & 0x04:
                layer_types.append(SLDLayerType.OUTLINE)

            if frame_type & 0x08:
                layer_types.append(SLDLayerType.DAMAGE)

            if frame_type & 0x10:
                layer_types.append(SLDLayerType.PLAYERCOLOR)

            main_width = 0
            main_height = 0
            main_hotspot_x = 0
            main_hotspot_y = 0
            for layer_type in layer_types:
                layer_length = SLD.sld_layer_length.unpack_from(data, current_offset)[0]
                start_offset = current_offset
                current_offset += SLD.sld_layer_length.size

                # Header unpacking
                if layer_type in (SLDLayerType.MAIN, SLDLayerType.SHADOW):
                    offset_x1, offset_y1, offset_x2, offset_y2, flag0, flag1 = \
                        SLD.sld_layer_header_graphics.unpack_from(data, current_offset)

                    layer_width = offset_x2 - offset_x1
                    layer_height = offset_y2 - offset_y1
                    layer_hotspot_x = canvas_hotspot_x - offset_x1
                    layer_hotspot_y = canvas_hotspot_y - offset_y1
                    if layer_type is SLDLayerType.MAIN:
                        main_width = layer_width
                        main_height = layer_height
                        main_hotspot_x = layer_hotspot_x
                        main_hotspot_y = layer_hotspot_y

                    current_offset += SLD.sld_layer_header_graphics.size

                elif layer_type in (SLDLayerType.OUTLINE, ):
                    # TODO
                    pass

                elif layer_type in (SLDLayerType.DAMAGE, SLDLayerType.PLAYERCOLOR):
                    flag0, flag1 = SLD.sld_layer_header_mask.unpack_from(data, current_offset)

                    layer_width = main_width
                    layer_height = main_height
                    layer_hotspot_x = main_hotspot_x
                    layer_hotspot_y = main_hotspot_y

                    current_offset += SLD.sld_layer_header_mask.size

                # Command array
                command_array_size = Struct("< H").unpack_from(data, current_offset)[0]
                current_offset += 2
                command_array_offset = current_offset

                # Compressed data block
                current_offset += 2 * command_array_size
                compressed_data_offset = current_offset

                layer_header = SLDLayerHeader(
                    layer_type,
                    frame_type,
                    offset_x1, offset_y1,
                    layer_width, layer_height,
                    layer_hotspot_x, layer_hotspot_y,
                    command_array_size,
                    command_array_offset,
                    compressed_data_offset
                )

                if layer_type is SLDLayerType.MAIN:
                    layer_def = SLDLayerBC1(frame_header, layer_header)
                    self.main_frames.append(layer_def)

                    if flag0 & 0x80 and frame_index > 0:
                        previous = previous_main
                        previous_layer = previous.get_pcolor()
                        previous_size = previous.layer_info.size
                        previous_offset = previous.layer_info.offset

                        layer_def.set_previous_layer(
                            previous_size[0],
                            previous_size[1],
                            previous_offset[0],
                            previous_offset[1],
                            previous_layer
                        )

                    previous_main = layer_def

                elif layer_type is SLDLayerType.SHADOW:
                    layer_def = SLDLayerBC4(frame_header, layer_header)
                    self.shadow_frames.append(layer_def)

                    if flag0 & 0x80 and frame_index > 0:
                        previous = previous_shadow
                        previous_layer = previous.get_pcolor()
                        previous_size = previous.layer_info.size
                        previous_offset = previous.layer_info.offset

                        layer_def.set_previous_layer(
                            previous_size[0],
                            previous_size[1],
                            previous_offset[0],
                            previous_offset[1],
                            previous_layer
                        )

                    previous_shadow = layer_def

                elif layer_type is SLDLayerType.OUTLINE:
                    # TODO
                    pass

                elif layer_type is SLDLayerType.DAMAGE:
                    layer_def = SLDLayerBC1(frame_header, layer_header)
                    self.dmg_mask_frames.append(layer_def)

                    if flag0 & 0x80 and frame_index > 0:
                        previous = previous_dmg_mask
                        previous_layer = previous.get_pcolor()
                        previous_size = previous.layer_info.size
                        previous_offset = previous.layer_info.offset

                        layer_def.set_previous_layer(
                            previous_size[0],
                            previous_size[1],
                            previous_offset[0],
                            previous_offset[1],
                            previous_layer
                        )

                    previous_dmg_mask = layer_def

                elif layer_type is SLDLayerType.PLAYERCOLOR:
                    layer_def = SLDLayerBC4(frame_header, layer_header)
                    self.playercolor_mask_frames.append(layer_def)

                    if flag0 & 0x80 and frame_index > 0:
                        previous = previous_playercolor
                        previous_layer = previous.get_pcolor()
                        previous_size = previous.layer_info.size
                        previous_offset = previous.layer_info.offset

                        layer_def.set_previous_layer(
                            previous_size[0],
                            previous_size[1],
                            previous_offset[0],
                            previous_offset[1],
                            previous_layer
                        )

                    previous_playercolor = layer_def

                # Jump to next layer offset
                current_offset = start_offset + layer_length
                # padding to size % 4
                current_offset += (4 - current_offset) % 4

    cpdef get_frames(self, layer: int = 0):
        """
        Get the frames in the SLD.

        :param layer: Position of the layer (see LAYER_TYPES)
                        - 0 = main graphics
                        - 1 = shadow graphics
                        - 2 = ???
                        - 3 = damage mask
                        - 4 = playercolor mask
        :type layer: int
        """
        cdef list frames
        cdef SLDLayer layer_def

        layer_type = LAYER_TYPES.get(
            layer,
            SLDLayerType.MAIN
        )

        if layer_type is SLDLayerType.MAIN:
            frames = self.main_frames

        elif layer_type is SLDLayerType.SHADOW:
            frames = self.shadow_frames

        elif layer_type is SLDLayerType.OUTLINE:
            frames = self.outline_frames

        elif layer_type is SLDLayerType.DAMAGE:
            frames = self.dmg_mask_frames

        elif layer_type is SLDLayerType.PLAYERCOLOR:
            frames = self.playercolor_mask_frames

        else:
            frames = []

        for layer_def in frames:
            layer_def.process_drawing_cmds(
                self.data,
                layer_def.layer_info.command_array_size,
                layer_def.layer_info.command_array_offset,
                layer_def.layer_info.compressed_data_offset
            )

        return frames

    def __str__(self):
        ret = list()

        ret.extend([repr(self), "\n", SLDLayerHeader.repr_header(), "\n"])
        for frame in self.main_frames:
            ret.extend([repr(frame), "\n"])
        return "".join(ret)

    def __repr__(self):
        return f"{self.sld_type} image<{len(self.main_frames):d} frames>"


cdef class SLDFrameHeader:
    """
    Header info for an SLD frame.
    """
    cdef (unsigned short, unsigned short) canvas_size
    cdef (unsigned short, unsigned short) hotspot
    cdef unsigned char frame_type
    cdef unsigned short frame_index

    def __init__(
        self,
        width, height,
        hotspot_x, hotspot_y,
        frame_type,
        frame_index
    ):
        self.canvas_size = (width, height)
        self.hotspot = (hotspot_x, hotspot_y)

        self.frame_type = frame_type
        self.frame_index = frame_index

    @staticmethod
    def repr_header():
        return " index | frame type | width x height |  hotspot  | "

    def __repr__(self):
        ret = (
            "% 5d | " % self.frame_index,
            "% 10x | " % self.frame_type,
            "% 5d x% 7d | " % self.size,
            "% 3d x% 3d | " % self.hotspot,
        )
        return "".join(ret)


cdef class SLDLayerHeader:
    """
    Header info for an SLD layer.
    """
    cdef (unsigned short, unsigned short) size
    cdef (unsigned short, unsigned short) offset
    cdef (short, short) hotspot
    cdef object layer_type
    cdef unsigned char frame_type
    cdef size_t command_array_size
    cdef unsigned int command_array_offset
    cdef unsigned int compressed_data_offset

    def __init__(
        self,
        layer_type,
        frame_type,
        offset_x, offset_y,
        width, height,
        hotspot_x, hotspot_y,
        command_array_size,
        command_array_offset,
        compressed_data_offset
    ):
        self.size = (width, height)
        self.offset = (offset_x, offset_y)
        self.hotspot = (hotspot_x, hotspot_y)

        self.layer_type = layer_type
        self.frame_type = frame_type

        self.command_array_size = command_array_size
        self.command_array_offset = command_array_offset
        self.compressed_data_offset = compressed_data_offset

    @staticmethod
    def repr_header():
        return (
            "layer type | width x height |  hotspot  | "
            "cmd array size | cmd array offset | data block | "
        )

    def __repr__(self):
        ret = (
            "% 11s | " % self.layer_type.value,
            "% 5d x% 7d | " % self.size,
            "% 3d x% 3d | " % self.hotspot,
            "% 14d | " % self.command_array_size,
            "% 16x | " % self.command_array_offset,
            "% 10x | " % self.compressed_data_offset,
        )
        return "".join(ret)


cdef class SLDLayer:
    """
    Layer inside the SLD frame.
    """
    # frame information
    cdef SLDFrameHeader frame_info

    # layer information
    cdef SLDLayerHeader layer_info

    # matrix representing the 4x4 blocks and the pixels in the image
    cdef vector[vector[pixel]] pcolor

    # Previous layer
    cdef (unsigned short, unsigned short) previous_size
    cdef (unsigned short, unsigned short) previous_offset
    cdef vector[vector[pixel]] *previous_layer

    def __init__(self, frame_header, layer_header):
        """
        SMX layer definition superclass. There can be various types of
        layers inside an SMX frame.

        :param frame_header: Header definition of the frame.
        :type frame_header: SLDFrameHeader
        :param layer_header: Header definition of the layer.
        :type layer_header: SLDLayerHeader
        """
        self.frame_info = frame_header
        self.layer_info = layer_header

        self.pcolor.reserve((self.layer_info.size[0] // 4) * (self.layer_info.size[1] // 4))

        self.previous_size = (0, 0)
        self.previous_offset = (0, 0)
        self.previous_layer = NULL

    @cython.boundscheck(False)
    @cython.wraparound(False)
    cdef void process_drawing_cmds(self,
                                   const uint8_t[::1] &data_raw,
                                   unsigned int cmd_size,
                                   unsigned int first_cmd_offset,
                                   unsigned int first_data_offset):
        """
        Process skip and draw commands from the command array.
        """
        cdef vector[pixel] transparent_block = vector[pixel](16)

        cdef unsigned char skip_count
        cdef unsigned char draw_count

        cdef unsigned int cmd_offset = first_cmd_offset
        cdef unsigned int data_offset = first_data_offset
        cdef unsigned int block_idx = 0

        for _ in range(cmd_size):
            skip_count = data_raw[cmd_offset]
            for _ in range(skip_count):
                if self.previous_layer == NULL:
                    self.pcolor.push_back(transparent_block)

                else:
                    previous_block_idx = get_block_index(
                        self.layer_info.size[0],
                        self.previous_size[0],
                        self.previous_size[1],
                        self.layer_info.offset[0],
                        self.layer_info.offset[1],
                        self.previous_offset[0],
                        self.previous_offset[1],
                        block_idx
                    )
                    if previous_block_idx >= 0:
                        prev_block = self.previous_layer.at(previous_block_idx)
                        self.pcolor.push_back(prev_block)

                    else:
                        self.pcolor.push_back(transparent_block)

                block_idx += 1

            cmd_offset += 1

            draw_count = data_raw[cmd_offset]
            for _ in range(draw_count):
                self.pcolor.push_back(self.decompress_block(data_raw, data_offset))
                data_offset += 8
                block_idx += 1

            cmd_offset += 1

        return

    @cython.boundscheck(False)
    cdef vector[pixel] decompress_block(self,
                                        const uint8_t[::1] &data_raw,
                                        Py_ssize_t block_offset):
        """
        Decompress a 4x4 pixel block.
        """
        pass

    cdef inline void set_previous_layer(self,
        unsigned short width,
        unsigned short height,
        unsigned short offset_x,
        unsigned short offset_y,
        vector[vector[pixel]] *previous
    ):
        """
        Set a reference to the previous layer.
        """
        self.previous_size = (width, height)
        self.previous_offset = (offset_x, offset_y)
        self.previous_layer = previous

    cdef inline vector[vector[pixel]] *get_pcolor(self):
        """
        Get the pixel data for the layer.
        """
        return &self.pcolor

    def get_picture_data(self):
        """
        Convert the palette index matrix to a RGBA image.

        :return: Array of RGBA values.
        :rtype: numpy.ndarray
        """
        return determine_rgba_matrix(
            self.pcolor,
            self.layer_info.size[0],
            self.layer_info.size[1]
        )

    def get_hotspot(self):
        """
        Return the layer's hotspot (the "center" of the image)
        """
        return self.layer_info.hotspot

    def __repr__(self):
        return repr(self.layer_info)


cdef class SLDLayerBC1(SLDLayer):
    """
    Compressed SLD layer using BC1 block compression.
    """
    def __init__(self, frame_header, layer_header):
        super().__init__(frame_header, layer_header)

    @cython.boundscheck(False)
    cdef inline vector[pixel] decompress_block(self,
                                               const uint8_t[::1] &data_raw,
                                               Py_ssize_t block_offset):
        """
        Decompress a 4x4 pixel block.
        """
        cdef size_t offset
        cdef unsigned char byte_val
        cdef unsigned char index
        cdef unsigned char mask = 0b0000_0011
        cdef vector[pixel] block
        block.reserve(16)

        # Lookup table
        cdef pixel c0
        cdef pixel c1
        cdef pixel c2
        cdef pixel c3

        cdef unsigned short c0_val = data_raw[block_offset] + (data_raw[block_offset + 1] << 8)
        cdef unsigned short c1_val = data_raw[block_offset + 2] + (data_raw[block_offset + 3] << 8)

        # Color 0
        c0.r = (data_raw[block_offset + 1] & 0b1111_1000) >> 3
        c0.g = ((data_raw[block_offset + 1] & 0b0000_0111) << 3) +\
            ((data_raw[block_offset] & 0b1110_0000) >> 5)
        c0.b = data_raw[block_offset] & 0b0001_1111

        # Expand to RGBA32 color space
        c0.r *= 8
        c0.g *= 4
        c0.b *= 8
        c0.a = 255

        # Color 1
        c1.r = (data_raw[block_offset + 3] & 0b1111_1000) >> 3
        c1.g = ((data_raw[block_offset + 3] & 0b0000_0111) << 3) +\
            ((data_raw[block_offset + 2] & 0b1110_0000) >> 5)
        c1.b = data_raw[block_offset + 2] & 0b0001_1111

        # Expand to RGBA32 color space
        c1.r *= 8
        c1.g *= 4
        c1.b *= 8
        c1.a = 255

        # Color 2 + 3
        if c0_val > c1_val:
            c2.r = (2 * c0.r + c1.r + 1) // 3
            c2.g = (2 * c0.g + c1.g + 1) // 3
            c2.b = (2 * c0.b + c1.b + 1) // 3
            c2.a = 255

            c3.r = (c0.r + 2 * c1.r + 1) // 3
            c3.g = (c0.g + 2 * c1.g + 1) // 3
            c3.b = (c0.b + 2 * c1.b + 1) // 3
            c3.a = 255

        else:
            c2.r = (c0.r + c1.r) // 2
            c2.g = (c0.g + c1.g) // 2
            c2.b = (c0.b + c1.b) // 2
            c2.a = 255

            c3.r = 0
            c3.g = 0
            c3.b = 0
            c3.a = 0

        # Lookup pixels
        offset = block_offset + 4
        for _ in range(4):
            byte_val = data_raw[offset]
            for _ in range(4):
                index = byte_val & mask

                if index == 0b00:
                    block.push_back(c0)

                elif index == 0b01:
                    block.push_back(c1)

                elif index == 0b10:
                    block.push_back(c2)

                elif index == 0b11:
                    block.push_back(c3)

                byte_val = byte_val >> 2

            offset += 1

        return block

    def get_picture_data(self):
        """
        Convert the palette index matrix to a RGBA image.

        :return: Array of RGBA values.
        :rtype: numpy.ndarray
        """
        return determine_rgba_matrix(
            self.pcolor,
            self.layer_info.size[0],
            self.layer_info.size[1]
        )


cdef class SLDLayerBC4(SLDLayer):
    """
    Compressed SLD layer using BC4 block compression.
    """
    def __init__(self, frame_header, layer_header):
        super().__init__(frame_header, layer_header)

    @cython.boundscheck(False)
    cdef inline vector[pixel] decompress_block(self,
                                               const uint8_t[::1] &data_raw,
                                               Py_ssize_t block_offset):
        """
        Decompress a 4x4 pixel block.
        """
        cdef size_t offset
        cdef unsigned int pixel_indices
        cdef unsigned char index
        cdef unsigned char mask = 0b0000_0111
        cdef vector[pixel] block
        block.reserve(16)

        # Lookup table
        cdef pixel c0
        cdef pixel c1
        cdef pixel c2
        cdef pixel c3
        cdef pixel c4
        cdef pixel c5
        cdef pixel c6
        cdef pixel c7

        # Color 0
        c0.r = data_raw[block_offset]
        c0.g = 0
        c0.b = 0
        c0.a = 255

        # Color 1
        c1.r = data_raw[block_offset + 1]
        c1.g = 0
        c1.b = 0
        c1.a = 255

        # Color 2 + 3
        if c0.r > c1.r:
            c2.r = (6 * c0.r + 1 * c1.r) // 7
            c2.g = 0
            c2.b = 0
            c2.a = 255
            c3.r = (5 * c0.r + 2 * c1.r) // 7
            c3.g = 0
            c3.b = 0
            c3.a = 255
            c4.r = (4 * c0.r + 3 * c1.r) // 7
            c4.g = 0
            c4.b = 0
            c4.a = 255
            c5.r = (3 * c0.r + 4 * c1.r) // 7
            c5.g = 0
            c5.b = 0
            c5.a = 255
            c6.r = (2 * c0.r + 5 * c1.r) // 7
            c6.g = 0
            c6.b = 0
            c6.a = 255
            c7.r = (1 * c0.r + 6 * c1.r) // 7
            c7.g = 0
            c7.b = 0
            c7.a = 255

        else:
            c2.r = (4 * c0.r + 1 * c1.r) // 5
            c2.g = 0
            c2.b = 0
            c2.a = 255
            c3.r = (3 * c0.r + 2 * c1.r) // 5
            c3.g = 0
            c3.b = 0
            c3.a = 255
            c4.r = (2 * c0.r + 3 * c1.r) // 5
            c4.g = 0
            c4.b = 0
            c4.a = 255
            c5.r = (1 * c0.r + 4 * c1.r) // 5
            c5.g = 0
            c5.b = 0
            c5.a = 255
            c6.r = 0
            c6.g = 0
            c6.b = 0
            c6.a = 0
            c7.r = 255
            c7.g = 0
            c7.b = 0
            c7.a = 255

        # Lookup pixels
        offset = block_offset + 2
        for _ in range(2):
            pixel_indices = data_raw[offset] | (data_raw[offset + 1] << 8) | (data_raw[offset + 2] << 16)
            for _ in range(8):
                # apply mask 0b111 to get 3 bits for index
                index = pixel_indices & mask

                if index == 0b000:
                    block.push_back(c0)

                elif index == 0b001:
                    block.push_back(c1)

                elif index == 0b010:
                    block.push_back(c2)

                elif index == 0b011:
                    block.push_back(c3)

                elif index == 0b100:
                    block.push_back(c4)

                elif index == 0b101:
                    block.push_back(c5)

                elif index == 0b110:
                    block.push_back(c6)

                elif index == 0b111:
                    block.push_back(c7)

                # Shift indices to read next 3 bits
                pixel_indices = pixel_indices >> 3

            offset += 3

        return block

    def get_picture_data(self):
        """
        Convert the palette index matrix to a RGBA image.

        :return: Array of RGBA values.
        :rtype: numpy.ndarray
        """
        # TODO: Greyscale export support
        # return determine_greyscale_matrix(self.pcolor, self.layer_info.size[0], self.layer_info.size[1])
        return determine_rgba_matrix(
            self.pcolor,
            self.layer_info.size[0],
            self.layer_info.size[1]
        )


@cython.cdivision(True)
cdef inline short get_block_index(
    unsigned short width1,
    unsigned short width2,
    unsigned short height2,
    unsigned short offset1_x,
    unsigned short offset1_y,
    unsigned short offset2_x,
    unsigned short offset2_y,
    unsigned short block_idx1
):
    """
    Get the vector index of a pixel block for a previous layer.

    SLDs allow layers to reuse/copy pixel blocks from previous frames.
    The pixel block is at the same in the current frame and the
    previous frame. Because we only store the SLD layers and never the
    full frame, we have to find the position of the pixel block
    in the previous layer by diffing the layer offsets and
    calculating the index in the block array of the previous layer.
    """
    cdef unsigned short w1_blocks = width1 // 4
    cdef unsigned short w2_blocks = width2 // 4
    cdef unsigned short h2_blocks = height2 // 4

    # Relative (x,y) coordinates of the block in the current layer
    cdef unsigned short block1_pos_x = block_idx1 % w1_blocks
    cdef unsigned short block1_pos_y = block_idx1 // w1_blocks

    # Difference between the layer offsets (i.e. their positions in the frame)
    # in blocks
    cdef short offset_diff_x = (offset1_x - offset2_x) // 4
    cdef short offset_diff_y = (offset1_y - offset2_y) // 4

    # Relative (x,y) coordinates of the block in the previous layer
    cdef short block2_pos_x = block1_pos_x + offset_diff_x
    cdef short block2_pos_y = block1_pos_y + offset_diff_y

    # Check if the coordinates in the previous layer are outside
    # of the layer
    if not (0 <= block2_pos_x < w2_blocks and 0 <= block2_pos_y < h2_blocks):
        return -1

    cdef unsigned short block_idx2 = block2_pos_x + block2_pos_y * w2_blocks

    return block_idx2


@cython.boundscheck(False)
@cython.wraparound(False)
cdef numpy.ndarray determine_rgba_matrix(vector[vector[pixel]] &block_matrix,
                                         Py_ssize_t width,
                                         Py_ssize_t height):
    """
    converts blocks to an rgba matrix.

    :param image_matrix: A 2-dimensional array of SLD pixels.
    """
    cdef numpy.ndarray[numpy.uint8_t, ndim=3, mode="c"] array_data = \
        numpy.zeros((height, width, 4), dtype=numpy.uint8)

    cdef vector[pixel] current_block

    cdef size_t block_x
    cdef size_t block_y

    cdef size_t img_x = 0
    cdef size_t img_y = 0

    for block_idx in range(block_matrix.size()):
        current_block = block_matrix[block_idx]

        block_x = 0
        block_y = 0
        for x in range(16):
            px = current_block[x]

            # array_data[y, x] = (r, g, b, alpha)
            array_data[img_y + block_y, img_x + block_x, 0] = px.r
            array_data[img_y + block_y, img_x + block_x, 1] = px.g
            array_data[img_y + block_y, img_x + block_x, 2] = px.b
            array_data[img_y + block_y, img_x + block_x, 3] = px.a

            block_x += 1
            if block_x > 3:
                block_x = 0
                block_y += 1

        img_x += 4
        if img_x >= width:
            img_x = 0
            img_y += 4

    return array_data


@cython.boundscheck(False)
@cython.wraparound(False)
cdef numpy.ndarray determine_greyscale_matrix(vector[vector[pixel]] &block_matrix,
                                              Py_ssize_t width,
                                              Py_ssize_t height):
    """
    converts blocks to an rgba matrix.

    :param image_matrix: A 2-dimensional array of SLD pixels.
    """
    cdef numpy.ndarray[numpy.uint8_t, ndim=2, mode="c"] array_data = \
        numpy.zeros((height, width), dtype=numpy.uint8)

    cdef vector[pixel] current_block

    cdef size_t block_x
    cdef size_t block_y

    cdef size_t img_x = 0
    cdef size_t img_y = 0

    for block_idx in range(block_matrix.size()):
        current_block = block_matrix[block_idx]

        block_x = 0
        block_y = 0
        for x in range(16):
            px = current_block[x]

            # array_data[y, x] = (r, g, b, alpha)
            array_data[img_y + block_y, img_x + block_x] = px.r

            block_x += 1
            if block_x > 3:
                block_x = 0
                block_y += 1

        img_x += 4
        if img_x >= width:
            img_x = 0
            img_y += 4

    return array_data
