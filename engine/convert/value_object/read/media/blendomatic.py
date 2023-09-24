# Copyright 2013-2023 the openage authors. See copying.md for legal info.

# TODO pylint: disable=too-many-function-args

"""
Conversion for the terrain blending masks.
Those originate from blendomatic.dat.

For more information, see doc/media/blendomatic.md
"""
from __future__ import annotations
import typing


from math import sqrt
from struct import Struct, unpack_from
import numpy


from .....log import dbg
from ..genie_structure import GenieStructure


if typing.TYPE_CHECKING:
    from engine.convert.entity_object.export.texture import Texture
    from engine.convert.value_object.init.game_version import GameVersion
    from engine.convert.value_object.read.member_access import MemberAccess
    from engine.convert.value_object.read.read_members import ReadMember
    from engine.convert.value_object.read.value_members import StorageType
    from engine.util.fslike.wrapper import GuardedFile


class BlendingTile:
    """
    One blending mask tile.
    The blendomatic stores many of those.
    """

    # pylint: disable=too-few-public-methods

    def __init__(self, row_data: list[list[int]], width: int, height: int):
        self.row_data = row_data
        self.width = width
        self.height = height

    def get_picture_data(self) -> numpy.array:
        """
        Return a numpy array of image data for a blending tile.
        """
        tile_rows = []

        for picture_row in self.row_data:
            tile_row_data = []

            for alpha_data in picture_row:

                if alpha_data == -1:
                    # draw full transparency
                    alpha = 0
                    val = 0
                else:
                    if alpha_data == 128:
                        alpha = 255
                        val = 0
                    else:
                        # original data contains 7-bit values only
                        alpha = 128
                        val = (127 - (alpha_data & 0x7f)) * 2

                tile_row_data.append((val, val, val, alpha))

            tile_rows.append(tile_row_data)

        return numpy.array(tile_rows)


class BlendingMode:
    """
    One blending mode, which contains tiles that make up
    for all the influence directions possible.
    One mode is for example "ice" or "grass", i.e. the transition shape.
    """

    # pylint: disable=too-few-public-methods

    def __init__(
        self,
        idx: int,
        data_file: GuardedFile,
        tile_count: int,
        header: tuple
    ):
        """
        initialize one blending mode,
        consisting of multiple frames for all blending directions

        the bitmasks were used to decide whether this pixel has
        to be used for calculations.

        the alphamask is used to determine the alpha amount for blending.
        """

        # should be 2353 -> number of pixels (single alpha byte values)
        self.pxcount = header[0]
        # tile_flags = header[1:]  # TODO what do they do?

        dbg("blending mode %d tiles have %d pixels", idx, self.pxcount)

        # as we draw in isometric tile format, this is the row count
        self.row_count = int(sqrt(self.pxcount)) + 1  # should be 49

        # alpha_masks_raw is an array of bytes that will draw 32 images,
        # which are bit masks.
        #
        # one of these masks also has 2353 pixels
        # the storage of the bit masks is 4*tilesize, here's why:
        #
        # 4 * 8bit * 2353 pixels = 75296 bitpixels
        # ==> 75296/(32 images) = 2353 bitpixel/image
        #
        # this means if we interprete the 75296 bitpixels as 32 images,
        # each of these images gets 2353 bit as data.
        # TODO: why 32 images? isn't that depending on tile_count?

        alpha_masks_raw = unpack_from(f"{self.pxcount * 4:d}B",
                                      data_file.read(self.pxcount * 4))

        # list of alpha-mask tiles
        self.alphamasks = []

        # draw mask tiles for this blending mode
        for _ in range(tile_count):
            pixels = unpack_from(f"{self.pxcount:d}B",
                                 data_file.read(self.pxcount))
            self.alphamasks.append(self.get_tile_from_data(pixels))

        bitvalues = []
        for i in alpha_masks_raw:
            for b_id in range(7, -1, -1):
                # bitmask from 0b00000001 to 0b10000000
                bit_mask = 2 ** b_id
                bitvalues.append(i & bit_mask)

        # list of bit-mask tiles
        self.bitmasks = []

        # TODO: is 32 really hardcoded?
        for i in range(32):
            pixels = bitvalues[i * self.pxcount:(i + 1) * self.pxcount]

            self.bitmasks.append(self.get_tile_from_data(pixels))

    def get_tile_from_data(self, data: list[int]) -> BlendingTile:
        """
        get the data pixels, interprete them in isometric tile format

          ....*....
          ..*****..
          *********
          ..*****..
          ....*....  like this, only bigger..

        we end up drawing the rhombus with 49 rows.
        the space indicated by . is added by the function.
        """

        half_row_count = self.row_count // 2
        tile_size = len(data)

        read_so_far = 0
        max_width = 0
        tilerows = []

        for y_pos in range(self.row_count):
            if y_pos < half_row_count:
                # upper half of the tile
                # row i+1 has 4 more pixels than row i
                # another +1 for the middle one
                read_values = 1 + (4 * y_pos)
            else:
                # lower half of tile
                read_values = (((self.row_count * 2) - 1) -
                               (4 * (y_pos - half_row_count)))

            if read_values > (tile_size - read_so_far):
                raise SyntaxError("reading more bytes than tile has left")
            if read_values < 0:
                raise SyntaxError(f"reading negative count: {read_values:d}")

            # grab the pixels out of the big list
            pixels = list(data[read_so_far:(read_so_far + read_values)])

            # how many empty pixels on the left before the real data begins
            space_count = self.row_count - 1 - (read_values // 2)

            # insert padding to the left and right (-1 for fully transparent)
            padding = [-1] * space_count
            pixels = padding + pixels + padding

            if len(pixels) > max_width:
                max_width = len(pixels)

            read_so_far += read_values
            tilerows.append(pixels)

        if read_so_far != tile_size:
            raise SyntaxError(f"got leftover bytes: {tile_size - read_so_far:d}")

        return BlendingTile(tilerows, max_width, self.row_count)


class Blendomatic(GenieStructure):
    """
    Represents the blendomatic.dat file.
    In it are multiple blending modes,
    which then contain multiple tiles.

    TODO: Blendomatic should work like SLP, meaning that the Blendomatic PNG
          should not create the texture, but rather be created by Texture
          during __init__ with a custom merger.
    """

    name_struct = "blending_mode"
    name_struct_file = "blending_mode"
    struct_description = ("describes one blending mode, "
                          "a blending transition shape "
                          "between two different terrain types.")

    # struct blendomatic_header {
    #   unsigned int nr_blending_modes;
    #   unsigned int nr_tiles;
    # };
    blendomatic_header = Struct("< I I")

    def __init__(self, fileobj: GuardedFile, custom_mode_count: int = None):
        super().__init__()

        buf = fileobj.read(Blendomatic.blendomatic_header.size)
        self.header = Blendomatic.blendomatic_header.unpack_from(buf)

        blending_mode_count, tile_count = self.header

        dbg("%d blending modes, each %d tiles",
            blending_mode_count, tile_count)

        if custom_mode_count:
            blending_mode_count = custom_mode_count

            dbg("reading only the first %d blending modes",
                custom_mode_count)

        blending_mode = Struct(f"< I {tile_count:d}B")

        self.blending_modes = []

        for i in range(blending_mode_count):
            header_data = fileobj.read(blending_mode.size)
            bmode_header = blending_mode.unpack_from(header_data)

            new_mode = BlendingMode(i, fileobj, tile_count, bmode_header)

            self.blending_modes.append(new_mode)

        fileobj.close()

    def get_textures(self) -> list[Texture]:
        """
        generate a list of textures.

        one atlas per blending mode is generated,
        each atlas contains all blending masks merged on one texture
        """
        from ....entity_object.export.texture import Texture
        return [Texture(b_mode) for b_mode in self.blending_modes]

    @classmethod
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = (
            (True, "blend_mode", None, "int32_t"),
        )

        return data_format

    def __str__(self):
        return str(self.blending_modes)
