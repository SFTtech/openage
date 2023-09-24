# Copyright 2013-2023 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R,too-many-function-args
from __future__ import annotations
import typing

import math

import numpy


from .....log import dbg
from ..genie_structure import GenieStructure

if typing.TYPE_CHECKING:
    from PIL import Image

    from engine.convert.value_object.init.game_version import GameVersion
    from engine.convert.value_object.read.member_access import MemberAccess
    from engine.convert.value_object.read.read_members import ReadMember
    from engine.convert.value_object.read.value_members import StorageType
    from engine.util.fslike.wrapper import GuardedFile


class ColorTable(GenieStructure):
    __slots__ = ('header', 'version', 'palette')

    def __init__(self, data: typing.Union[list, tuple, bytes]):
        super().__init__()

        if isinstance(data, list) or isinstance(data, tuple):
            self.fill_from_array(data)
        else:
            self.fill(data)

        # Fast access for media conversion
        self.array = self.get_ndarray()

    def fill_from_array(self, ar: typing.Union[list, tuple]) -> None:
        self.palette = [tuple(e) for e in ar]

    def fill(self, data: bytes) -> None:
        # split all lines of the input data
        # \r\n windows windows windows baby
        lines = data.decode('ascii').split('\r\n')

        self.header = lines[0]
        self.version = lines[1]

        # check for palette header
        if not (self.header == "JASC-PAL" or self.header == "JASC-PALX"):
            raise SyntaxError("No palette header 'JASC-PAL' or 'JASC-PALX' found, "
                              "instead: %r" % self.header)

        if self.version != "0100":
            raise SyntaxError(f"palette version mispatch, got {self.version}")

        entry_count = int(lines[2])

        entry_start = 3
        if lines[3].startswith("$ALPHA"):
            # TODO: Definitive Editions have palettes with fixed alpha
            entry_start = 4

        self.palette = []

        # data entries from 'entry_start' to n
        for line in lines[entry_start:]:
            # skip comments and empty lines
            if not line or line.startswith("#"):
                continue

            # one entry looks like "13 37 42",
            # "red green blue"
            # => red 13, 37 green and 42 blue.
            # DE1 and DE2 have a fourth value, but it seems unused
            self.palette.append(tuple(int(val) for val in line.split()))

        if len(self.palette) != entry_count:
            raise SyntaxError("read a %d palette entries "
                              "but expected %d." % (
                                  len(self.palette), entry_count))

    def __getitem__(self, index):
        return self.palette[index]

    def __len__(self):
        return len(self.palette)

    def __repr__(self):
        return "ColorTable<%d entries>" % len(self.palette)

    def __str__(self):
        return f"{repr(self)}\n{self.palette}"

    def gen_image(self, draw_text: bool = True, squaresize: int = 100) -> Image:
        """
        writes this color table (palette) to a png image.
        """

        from PIL import Image, ImageDraw

        imgside_length = math.ceil(math.sqrt(len(self.palette)))
        imgsize = imgside_length * squaresize

        dbg("generating palette image with size %dx%d", imgsize, imgsize)

        palette_image = Image.new('RGBA', (imgsize, imgsize),
                                  (255, 255, 255, 0))
        draw = ImageDraw.ImageDraw(palette_image)

        # dirty, i know...
        text_padlength = len(str(len(self.palette)))
        text_format = "%%0%dd" % (text_padlength)

        drawn = 0

        # squaresize 1 means draw single pixels
        if squaresize == 1:
            for y in range(imgside_length):
                for x in range(imgside_length):
                    if drawn < len(self.palette):
                        r, g, b = self.palette[drawn]
                        draw.point((x, y), fill=(r, g, b, 255))
                        drawn = drawn + 1

        # draw nice squares with given side length
        elif squaresize > 1:
            for y in range(imgside_length):
                for x in range(imgside_length):
                    if drawn < len(self.palette):
                        sx = x * squaresize - 1
                        sy = y * squaresize - 1
                        ex = sx + squaresize - 1
                        ey = sy + squaresize
                        r, g, b = self.palette[drawn]
                        # begin top-left, go clockwise:
                        vertices = [(sx, sy), (ex, sy), (ex, ey), (sx, ey)]
                        draw.polygon(vertices, fill=(r, g, b, 255))

                        if draw_text and squaresize > 40:
                            # draw the color id
                            # insert current color id into string
                            ctext = text_format % drawn
                            tcolor = (255 - r, 255 - b, 255 - g, 255)

                            # draw the text
                            # TODO: use customsized font
                            draw.text((sx + 3, sy + 1), ctext,
                                      fill=tcolor, font=None)

                        drawn = drawn + 1

        else:
            raise ValueError("fak u, no negative values for squaresize pls.")

        return palette_image

    def get_ndarray(self) -> numpy.array:
        return numpy.array(self.palette, dtype=numpy.uint8, order='C')

    def save_visualization(self, fileobj: GuardedFile) -> None:
        self.gen_image().save(fileobj, 'png')

    @classmethod
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = (
            (True, "idx", None, "int32_t"),
            (True, "r", None,   "uint8_t"),
            (True, "g", None,   "uint8_t"),
            (True, "b", None,   "uint8_t"),
            (True, "a", None,   "uint8_t"),
        )

        return data_format


class PlayerColorTable(GenieStructure):
    """
    this class represents stock player color values.

    each player has 8 subcolors, where 0 is the darkest and 7 is the lightest
    """

    __slots__ = ('header', 'version', 'palette')

    def __init__(self, base_table: ColorTable):
        super().__init__()

        if not isinstance(base_table, ColorTable):
            raise TypeError(f"no ColorTable supplied, instead: {type(base_table)}")

        self.header = base_table.header
        self.version = base_table.version
        self.palette = list()
        # now, strip the base table entries to the player colors.

        # entry id = ((playernum-1) * 8) + subcolor
        players = range(1, 9)
        psubcolors = range(8)

        for i in players:
            for subcol in psubcolors:
                r, g, b = base_table[16 * i + subcol]
                self.palette.append((r, g, b))

    @classmethod
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = (
            (True, "idx", None, "int32_t"),
            (True, "r", None,   "uint8_t"),
            (True, "g", None,   "uint8_t"),
            (True, "b", None,   "uint8_t"),
            (True, "a", None,   "uint8_t"),
        )

        return data_format

    def __repr__(self):
        return "PlayerColorTable<%d entries>" % len(self.palette)
