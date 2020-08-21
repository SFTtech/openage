# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Module for registering media files.
"""

from ...value_object.dataformat.media_types import MediaType


def get_existing_graphics(args):
    """
    List the graphics files that exist in the graphics file paths.
    """
    filenames = []
    for filepath in args.srcdir[MediaType.GRAPHICS.value].iterdir():
        filenames.append(filepath.stem)

    return filenames
