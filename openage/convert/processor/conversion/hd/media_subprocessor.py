# Copyright 2021-2023 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-statements

"""
Convert media information to metadata definitions and export
requests. Subroutine of the main HD processor.
"""
from __future__ import annotations
import typing

from .media.graphics import create_graphics_requests
from .media.sound import create_sound_requests

if typing.TYPE_CHECKING:
    from ....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


class HDMediaSubprocessor:
    """
    Creates the exports requests for media files from HD Edition.
    """

    @classmethod
    def convert(cls, full_data_set: GenieObjectContainer) -> None:
        """
        Create all export requests for the dataset.
        """
        create_graphics_requests(full_data_set)
        create_sound_requests(full_data_set)

    create_graphics_requests = staticmethod(create_graphics_requests)
    create_sound_requests = staticmethod(create_sound_requests)
