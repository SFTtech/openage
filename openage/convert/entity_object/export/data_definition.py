# Copyright 2014-2022 the openage authors. See copying.md for legal info.

"""
Output format specification for data to write.
"""

from __future__ import annotations
import typing


class DataDefinition:
    """
    Contains a data definition that can then be
    formatted to an arbitrary output file.
    """

    def __init__(self, targetdir: str, filename: str):
        """
        Creates a new data definition.

        :param targetdir: Relative path to the export directory.
        :type targetdir: str
        :param filename: Filename of the resulting file.
        :type filename: str
        """
        self.targetdir = targetdir
        self.filename = filename

    def dump(self) -> typing.NoReturn:
        """
        Creates a human-readable string that can be written to a file.
        """
        raise NotImplementedError(f"{type(self)} has not implemented dump() method")

    def set_filename(self, filename: str) -> None:
        """
        Sets the filename for the file.

        :param filename: Filename of the resuilting file.
        :type filename: str
        """
        if not isinstance(filename, str):
            raise ValueError(f"str expected as filename, not {type(filename)}")

        self.filename = filename

    def set_targetdir(self, targetdir: str) -> None:
        """
        Sets the target directory for the file.

        :param targetdir: Relative path to the export directory.
        :type targetdir: str
        """
        if not isinstance(targetdir, str):
            raise ValueError("str expected as targetdir")

        self.targetdir = targetdir

    def __repr__(self):
        return f"DataDefinition<{type(self)}>"
