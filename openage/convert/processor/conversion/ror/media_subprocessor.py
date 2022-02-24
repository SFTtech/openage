# Copyright 2021-2022 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-few-public-methods
"""
Convert media information to metadata definitions and export
requests. Subroutine of the main RoR processor.
"""
from __future__ import annotations
import typing

from openage.convert.processor.conversion.aoc.media_subprocessor import AoCMediaSubprocessor

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer


class RoRMediaSubprocessor:
    """
    Creates the exports requests for media files from AoC.
    """

    @classmethod
    def convert(cls, full_data_set: GenieObjectContainer) -> None:
        """
        Create all export requests for the dataset.
        """
        AoCMediaSubprocessor.create_graphics_requests(full_data_set)
        AoCMediaSubprocessor.create_sound_requests(full_data_set)
