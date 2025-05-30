# Copyright 2019-2025 the openage authors. See copying.md for legal info.

"""
Convert media information to metadata definitions and export
requests. Subroutine of the main AoC processor.
"""
from __future__ import annotations
import typing

from .media.blend import create_blend_requests
from .media.graphics import create_graphics_requests
from .media.sound import create_sound_requests

if typing.TYPE_CHECKING:
    from ....entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer


class AoCMediaSubprocessor:
    """
    Creates the exports requests for media files from AoC.
    """

    @classmethod
    def convert(cls, full_data_set: GenieObjectContainer) -> None:
        """
        Create all export requests for the dataset.
        """
        cls.create_graphics_requests(full_data_set)
        # cls.create_blend_requests(full_data_set)
        cls.create_sound_requests(full_data_set)

    create_blend_requests = staticmethod(create_blend_requests)
    create_graphics_requests = staticmethod(create_graphics_requests)
    create_sound_requests = staticmethod(create_sound_requests)
