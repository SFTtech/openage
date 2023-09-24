# Copyright 2021-2023 the openage authors. See copying.md for legal info.

"""
Blendtable definition file.
"""
from __future__ import annotations
import typing

from ..data_definition import DataDefinition

FORMAT_VERSION = '1'


class BlendtableMetadata(DataDefinition):
    """
    Collects blentable metadata and can format it
    as a .bltable custom format
    """

    def __init__(self, targetdir: str, filename: str):
        super().__init__(targetdir, filename)

        self.blendtable: tuple = None
        self.patterns: dict[int, dict[str, typing.Any]] = {}

    def add_pattern(self, pattern_id: int, filename: str) -> None:
        """
        Define a pattern in the table.

        :param pattern_id: Pattern identifier.
        :type pattern_id: int
        :param filename: Path to the pattern file.
        :type filename: str
        """
        self.patterns[pattern_id] = {
            "pattern_id": pattern_id,
            "filename": filename
        }

    def set_blendtabe(self, table: tuple) -> None:
        """
        Set the blendtable. This expects a tuple of integers with nxn entries.

        :param table: Blending lookup table.
        :type table: tuple
        """
        self.blendtable = table

        self._check_table()

    def dump(self) -> str:
        output_str = ""

        # header
        output_str += "# openage blendtable definition file\n\n"

        # version
        output_str += f"version {FORMAT_VERSION}\n\n"

        # blendtable
        output_str += "blendtable [\n"

        # table entries
        table_width = self._get_table_width()
        for idx in range(table_width):
            row_entries = self.blendtable[idx * table_width:(idx + 1) * table_width]
            output_str += f'{" ".join(row_entries)}\n'

        output_str += "]\n\n"

        # pattern definitions
        for pattern in self.patterns.values():
            output_str += f"pattern {pattern['pattern_id']} {pattern['filename']}\n"

        output_str += "\n"

        return output_str

    def _get_table_width(self) -> int:
        """
        Get the width of the blending table.
        """
        table_size = len(self.blendtable)

        # Newton's method
        left = table_size
        right = (left + 1) // 2
        while right < left:
            left = right
            right = (left + table_size // left) // 2

        return left

    def _check_table(self) -> typing.Union[None, typing.NoReturn]:
        """
        Check if the blending table is a nxn matrix.
        """
        table_width = self._get_table_width()

        if table_width * table_width != len(self.blendtable):
            raise ValueError(f"blendtable entries malformed: "
                             f"{len(self.blendtable)} is not an integer square")

    def __repr__(self):
        return f'BlendtableMetadata<{self.filename}>'
