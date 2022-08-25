# Copyright 2022-2022 the openage authors. See copying.md for legal info.
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
from libcpp.vector cimport vector

# SLD files have little endian byte order
endianness = "< "

cdef struct pixel:
    uint8_t r
    uint8_t g
    uint8_t b
    uint8_t a


class SLD:
    """
    Class for reading/converting compressed SLD files (delivered
    with AoE2:DE since version ASDF by default).
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
    #   unsigned short unknown1;
    #   unsigned short unknown2;
    #   unsigned short unknown3;
    #   unsigned short unknown4;
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
    #   unsigned short alpha_padding_width;
    #   unsigned short alpha_padding_height;
    #   unsigned short total_width;
    #   unsigned short total_height;
    #   char           unknown1;
    #   char           unknown2;
    # };
    sld_layer_header_graphics = Struct(endianness + "4H 2B")

    # struct sld_layer_header_mask {
    #   char           unknown1;
    #   char           unknown2;
    # };
    sld_layer_header_mask = Struct(endianness + "2B")

    def __init__(self, data):
        """
        Read an SLD image file.

        :param data: File content as bytes.
        :type data: bytes, bytearray
        """

        sld_header = SLD.sld_header.unpack_from(data)
        _, version, frame_count, _, _, _ = sld_header

        dbg("SLD")
        dbg(" version:     %s",   version)
        dbg(" frame count: %s",   frame_count)

        # SLD graphic frames are created from several layers
        self.main_frames = list()
        self.shadow_frames = list()
        self.outline_frames = list()
        self.dmg_mask_frames = list()
        self.playercolor_mask_frames = list()

        spam(SLDLayerHeader.repr_header())

        # SLD files have no offsets, we have to calculate them
        current_offset = SLD.sld_header.size
        for i in range(frame_count):
            frame_header = SLD.sld_frame_header.unpack_from(
                data, current_offset)

            self.sld_type, _, _, _, frame_type , _, frame_index = frame_header

            current_offset += SLD.sld_frame_header.size

            layer_types = []

            if frame_type & 0x01:
                layer_types.append("main")

            if frame_type & 0x02:
                layer_types.append("shadow")

            if frame_type & 0x04:
                layer_types.append("outline")

            if frame_type & 0x08:
                layer_types.append("damage")

            if frame_type & 0x10:
                layer_types.append("playercolor")

            main_width = 0
            main_height = 0
            for layer_type in layer_types:
                layer_length = SLD.sld_layer_length.unpack_from(
                    data, current_offset)
                start_offset = current_offset
                current_offset += SLD.sld_layer_length.size

                # Header unpacking
                if layer_type in ("main", "shadow"):
                    layer_header = SLD.sld_layer_header_graphics.unpack_from(data, current_offset)
                    a_width, a_height, t_width, t_height, _, _ = layer_header

                    layer_width = t_width - a_width
                    layer_height = t_height - a_height

                    if layer_type == "main":
                        main_width = layer_width
                        main_height = layer_height

                    current_offset += SLD.sld_layer_header_graphics.size

                elif layer_type in ("outline", ):
                    # TODO
                    pass

                elif layer_type in ("damage", "playercolor"):
                    layer_header = SLD.sld_layer_header_mask.unpack_from(data, current_offset)

                    layer_width = main_width
                    layer_height = main_height

                    current_offset += SLD.sld_layer_header_graphics.size

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
                    layer_width, layer_height,
                    command_array_size,
                    command_array_offset,
                    compressed_data_offset
                )

                if layer_type == "main":
                    self.main_frames.append(SLDLayerBC1(layer_header, data))

                elif layer_type == "shadow":
                    self.shadow_frames.append(SLDLayerBC4(layer_header, data))

                elif layer_type == "outline":
                    # TODO
                    pass

                elif layer_type == "damage":
                    self.dmg_mask_frames.append(SLDLayerBC1(layer_header, data))

                elif layer_type == "playercolor":
                    self.shadow_frames.append(SLDLayerBC4(layer_header, data))

                # Jump to next layer offset
                current_offset = start_offset + layer_length
                current_offset += current_offset % 4            # padded to size 4

    def __str__(self):
        ret = list()

        ret.extend([repr(self), "\n", SLDLayerHeader.repr_header(), "\n"])
        for frame in self.main_frames:
            ret.extend([repr(frame), "\n"])
        return "".join(ret)

    def __repr__(self):
        return f"{self.sld_type} image<{len(self.main_frames):d} frames>"




class SLDLayerHeader:
    def __init__(
        self,
        layer_type,
        frame_type,
        width, height,
        command_array_size,
        command_array_offset,
        compressed_data_offset
    ):
        self.size = (width, height)

        self.layer_type = layer_type
        self.frame_type = frame_type

        self.command_array_size = command_array_size
        self.command_array_offset = command_array_offset
        self.compressed_data_offset = compressed_data_offset

    @staticmethod
    def repr_header():
        return (
            "layer type | width x height | cmd array size | "
            "cmd array offset | data block | "
        )

    def __repr__(self):
        ret = (
            "% s | " % self.layer_type,
            "% 5d x% 7d | " % self.size,
            "% 14d | " % self.command_array_size,
            "% 16d | " % self.command_array_offset,
            "% 10d | " % self.compressed_data_offset,
        )
        return "".join(ret)

cdef class SLDLayer:
    """
    Layer inside the SLD frame.
    """
    # matrix representing the 4x4 blocks and the pixels in the image
    cdef vector[vector[pixel]] pcolor

    def __init__(self, layer_header, data):
        """
        SMX layer definition superclass. There can be various types of
        layers inside an SMX frame.

        :param layer_header: Header definition of the layer.
        :param data: File content as bytes.
        :type layer_header: SMXLayerHeader
        :type data: bytes, bytearray
        """
        self.info = layer_header

        if not (isinstance(data, bytes) or isinstance(data, bytearray)):
            raise ValueError("Layer data must be some bytes object")

        # memory pointer
        # convert the bytes obj to char*
        cdef const uint8_t[:] data_raw = data

        cdef unsigned int cmd_size = self.info.command_array_size
        cdef unsigned int cmd_offset = self.info.command_array_offset
        cdef unsigned int data_offset = self.info.compressed_data_offset

        # process cmd table
        self.process_drawing_cmds(data, cmd_size, cmd_offset, data_offset)

    @cython.boundscheck(False)
    @cython.wraparound(False)
    cdef void process_drawing_cmds(self,
                                   const uint8_t[:] &data_raw,
                                   Py_ssize_t cmd_size,
                                   Py_ssize_t first_cmd_offset,
                                   Py_ssize_t first_data_offset):
        """
        Process skip and draw commands from the command array.
        """
        cdef vector[pixel] transparent_block = vector[pixel](16, pixel(0,0,0,0))

        cdef unsigned char skip_count
        cdef unsigned char draw_count

        cdef unsigned int cmd_offset = first_cmd_offset
        cdef unsigned int data_offset = first_data_offset

        for _ in range(cmd_size):
            skip_count = data_raw[cmd_offset]
            for _ in range(skip_count):
                self.pcolor.push_back(transparent_block)

            cmd_offset =+ 1

            draw_count = data_raw[cmd_offset]
            for _ in range(draw_count):
                self.pcolor.push_back(self.decompress_block(data_raw, data_offset))
                data_offset += 8

            cmd_offset += 1

        return

    @cython.boundscheck(False)
    cdef vector[pixel] decompress_block(self,
                                        const uint8_t[:] &data_raw,
                                        Py_ssize_t block_offset):
        """
        Decompress a 4x4 pixel block.
        """
        pass

    def get_picture_data(self):
        """
        Convert the palette index matrix to a RGBA image.

        :return: Array of RGBA values.
        :rtype: numpy.ndarray
        """
        return determine_rgba_matrix(self.pcolor, self.info.size[0], self.info.size[1])

    def __repr__(self):
        return repr(self.info)


cdef class SLDLayerBC1(SLDLayer):
    """
    Compressed SLD layer using BC1 block compression.
    """
    def __init__(self, layer_header, data):
        super().__init__(layer_header, data)

    @cython.boundscheck(False)
    cdef inline vector[pixel] decompress_block(self,
                                               const uint8_t[:] &data_raw,
                                               Py_ssize_t block_offset):
        """
        Decompress a 4x4 pixel block.
        """
        cdef unsigned char offset
        cdef unsigned char byte_val
        cdef unsigned char index
        cdef unsigned char mask = 0b00000011
        cdef vector[pixel] block = vector[pixel](16)

        # Lookup table
        cdef pixel c0
        cdef pixel c1
        cdef pixel c2
        cdef pixel c3

        cdef unsigned short c0_val = data_raw[block_offset] + (data_raw[block_offset + 1] << 8)
        cdef unsigned short c1_val = data_raw[block_offset + 2] + (data_raw[block_offset + 3] << 8)

        # Color 0
        c0.r = (data_raw[block_offset + 1] & 0b11111000) >> 3
        c0.g = ((data_raw[block_offset + 1] & 0b00000111) << 3) +\
            ((data_raw[block_offset] & 0b11100000) >> 5)
        c0.b = data_raw[block_offset] & 0b00011111
        c0.a = 255

        # Color 1
        c1.r = (data_raw[block_offset + 3] & 0b11111000) >> 3
        c1.g = ((data_raw[block_offset + 3] & 0b00000111) << 3) +\
            ((data_raw[block_offset + 2] & 0b11100000) >> 5)
        c1.b = data_raw[block_offset + 2] & 0b00011111
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
        for _ in range(4):
            offset = block_offset + 4
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
        return determine_rgba_matrix(self.pcolor, self.info.size[0], self.info.size[1])


cdef class SLDLayerBC4(SLDLayer):
    """
    Compressed SLD layer using BC4 block compression.
    """
    def __init__(self, layer_header, data):
        super().__init__(layer_header, data)

    @cython.boundscheck(False)
    cdef inline vector[pixel] decompress_block(self,
                                               const uint8_t[:] &data_raw,
                                               Py_ssize_t block_offset):
        """
        Decompress a 4x4 pixel block.
        """
        cdef unsigned char offset
        cdef unsigned char byte_val
        cdef unsigned char index
        cdef unsigned char mask = 0b00000111
        cdef vector[pixel] block = vector[pixel](16)

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
            c6.a = 255
            c7.r = 255
            c7.g = 0
            c7.b = 0
            c7.a = 255

        # Lookup pixels
        for _ in range(2):
            offset = block_offset + 5
            for _ in range(3):
                byte_val = data_raw[offset]
                for _ in range(4):
                    index = byte_val & mask

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

                    byte_val = byte_val >> 2

                offset -= 1

            offset += 6

        return block

    def get_picture_data(self):
        """
        Convert the palette index matrix to a RGBA image.

        :return: Array of RGBA values.
        :rtype: numpy.ndarray
        """
        return determine_greyscale_matrix(self.pcolor, self.info.size[0], self.info.size[1])


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
        if img_x > width:
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
        if img_x > width:
            img_x = 0
            img_y += 4

    return array_data
