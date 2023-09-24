# Copyright 2020-2022 the openage authors. See copying.md for legal info.

"""
Module for registering media files.
"""
from __future__ import annotations
import typing


from ...value_object.read.media_types import MediaType

if typing.TYPE_CHECKING:
    from argparse import Namespace


def get_existing_graphics(args: Namespace) -> list[str]:
    """
    List the graphics files that exist in the graphics file paths.
    """
    filenames = []
    for filepath in args.srcdir[MediaType.GRAPHICS.value].iterdir():
        filenames.append(filepath.stem)

    return filenames
