# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create export requests for blending files.
"""
from __future__ import annotations
import typing

from .....entity_object.export.media_export_request import MediaExportRequest
from .....value_object.read.media_types import MediaType

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer


def create_blend_requests(full_data_set: GenieObjectContainer) -> None:
    """
    Create export requests for Blendomatic objects.

    TODO: Blendomatic contains multiple files. Better handling?

    :param full_data_set: Data set containing all objects and metadata that the export
                          requests are added to.
    """
    export_request = MediaExportRequest(
        MediaType.BLEND,
        "data/blend/",
        full_data_set.game_version.edition.media_paths[MediaType.BLEND][0],
        "blendmode"
    )
    full_data_set.blend_exports.update({0: export_request})
