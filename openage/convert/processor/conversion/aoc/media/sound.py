# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create export requests for sound files.
"""
from __future__ import annotations
import typing

from .....entity_object.export.media_export_request import MediaExportRequest
from .....value_object.read.media_types import MediaType

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer


def create_sound_requests(full_data_set: GenieObjectContainer) -> None:
    """
    Create export requests for sounds referenced by CombinedSound objects.

    :param full_data_set: Data set containing all objects and metadata that the export
                          requests are added to.
    """
    combined_sounds = full_data_set.combined_sounds.values()

    for sound in combined_sounds:
        sound_id = sound.get_file_id()

        targetdir = sound.resolve_sound_location()
        source_filename = f"{str(sound_id)}.wav"
        target_filename = f"{sound.get_filename()}.opus"

        export_request = MediaExportRequest(MediaType.SOUNDS,
                                            targetdir,
                                            source_filename,
                                            target_filename)

        full_data_set.sound_exports.update({sound_id: export_request})
