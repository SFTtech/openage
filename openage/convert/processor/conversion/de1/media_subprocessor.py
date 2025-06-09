# Copyright 2020-2023 the openage authors. See copying.md for legal info.

"""
Convert media information to metadata definitions and export
requests. Subroutine of the main DE1 processor.
"""
from __future__ import annotations
import typing

from ..aoc.media_subprocessor import AoCMediaSubprocessor
from .media.graphics import create_graphics_requests

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer


class DE1MediaSubprocessor:
    """
    Creates the exports requests for media files from DE1.
    """

    @classmethod
    def convert(cls, full_data_set: GenieObjectContainer) -> None:
        """
        Create all export requests for the dataset.
        """
        cls.create_graphics_requests(full_data_set)
        AoCMediaSubprocessor.create_sound_requests(full_data_set)

    create_graphics_requests = staticmethod(create_graphics_requests)
