# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create export requests for sound files.
"""
from __future__ import annotations
import typing

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def create_sound_requests(full_data_set: GenieObjectContainer) -> None:
    """
    Create export requests for sounds referenced by CombinedSound objects.
    """
    # TODO: Sound exports (Wwise files)
