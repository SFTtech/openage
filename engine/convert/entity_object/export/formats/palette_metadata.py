# Copyright 2021-2022 the openage authors. See copying.md for legal info.

"""
Palette definition file.
"""

from ..data_definition import DataDefinition

FORMAT_VERSION = '1'


class PaletteMetadata(DataDefinition):
    """
    Collects palette metadata and can format it
    as a .opal custom format
    """

    def __init__(self, targetdir: str, filename: str):
        super().__init__(targetdir, filename)

        self.colours: list[tuple] = []

    def add_colour(self, colour: tuple) -> None:
        """
        Add a RGBA colour to the end of the palette.

        :param colour: RGBA colour tuple.
        :type colour: tuple
        """
        self.colours.append(colour)

    def add_colours(self, colours: list[tuple]) -> None:
        """
        Add a collection of RGBA colours to the end of the palette.

        :param colours: Collection of RGBA coulour tuples.
        :type colours: tuple, list
        """
        self.colours.extend(colours)

    def dump(self) -> str:
        output_str = ""

        # header
        output_str += "# openage palette definition file\n\n"

        # version
        output_str += f"version {FORMAT_VERSION}\n\n"

        # entries
        output_str += f"entries {len(self.colours)}\n\n"

        # palette
        output_str += "colours [\n"

        # frame definitions
        for colour in self.colours:
            output_str += f'{" ".join(str(param) for param in colour)}\n'

        output_str += "]\n"

        return output_str

    def __repr__(self):
        return f'PaletteMetadata<{self.filename}>'
