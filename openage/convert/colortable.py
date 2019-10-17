# Copyright 2013-2018 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

import math

from openage.convert.dataformat.genie_structure import GenieStructure
from .dataformat.data_definition import DataDefinition
from .dataformat.struct_definition import StructDefinition
from ..log import dbg


class ColorTable(GenieStructure):
    name_struct = "palette_color"
    name_struct_file = "color"
    struct_description = "indexed color storage."

    data_format = (
        (True, "idx", "int32_t"),
        (True, "r",   "uint8_t"),
        (True, "g",   "uint8_t"),
        (True, "b",   "uint8_t"),
        (True, "a",   "uint8_t"),
    )

    def __init__(self, data):
        super().__init__()

        if isinstance(data, list) or isinstance(data, tuple):
            self.fill_from_array(data)
        else:
            self.fill(data)

    def fill_from_array(self, ar):
        self.palette = [tuple(e) for e in ar]

    def fill(self, data):
        # split all lines of the input data
        # \r\n windows windows windows baby
        lines = data.decode('ascii').split('\r\n')

        self.header = lines[0]
        self.version = lines[1]

        # check for palette header
        if self.header != "JASC-PAL":
            raise Exception("No palette header 'JASC-PAL' found, "
                            "instead: %r" % self.header)

        if self.version != "0100":
            raise Exception("palette version mispatch, got %s" % self.version)

        entry_count = int(lines[2])

        self.palette = []

        # data entries are line 3 to n
        for line in lines[3:entry_count + 3]:
            # one entry looks like "13 37 42",
            # "red green blue"
            # => red 13, 37 green and 42 blue.
            self.palette.append(tuple(int(val) for val in line.split(' ')))

        if len(self.palette) != len(lines) - 4:
            raise Exception("read a %d palette entries "
                            "but expected %d." % (
                                len(self.palette), len(lines) - 4))

    def __getitem__(self, index):
        return self.palette[index]

    def __len__(self):
        return len(self.palette)

    def __repr__(self):
        return "ColorTable<%d entries>" % len(self.palette)

    def __str__(self):
        return "%s\n%s" % (repr(self), self.palette)

    def gen_image(self, draw_text=True, squaresize=100):
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
            raise Exception("fak u, no negative values for squaresize pls.")

        return palette_image

    def save_visualization(self, fileobj):
        self.gen_image().save(fileobj, 'png')

    def dump(self, filename):
        data = list()

        # dump all color entries
        for idx, entry in enumerate(self.palette):
            color_entry = {
                "idx": idx,
                "r":   entry[0],
                "g":   entry[1],
                "b":   entry[2],
                "a":   255,
            }
            data.append(color_entry)

        return [DataDefinition(self, data, filename)]

    @classmethod
    def structs(cls):
        return [StructDefinition(cls)]


class PlayerColorTable(ColorTable):
    """
    this class represents stock player color values.

    each player has 8 subcolors, where 0 is the darkest and 7 is the lightest
    """
    def __init__(self, base_table):
        # TODO pylint: disable=super-init-not-called
        if not isinstance(base_table, ColorTable):
            raise Exception("no ColorTable supplied, "
                            "instead: %s" % (type(base_table)))

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

    def __repr__(self):
        return "ColorTable<%d entries>" % len(self.palette)
