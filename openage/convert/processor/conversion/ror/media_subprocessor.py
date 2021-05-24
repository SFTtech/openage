# Copyright 2021-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-few-public-methods
"""
Convert media information to metadata definitions and export
requests. Subroutine of the main RoR processor.
"""
from ..aoc.media_subprocessor import AoCMediaSubprocessor


class RoRMediaSubprocessor:
    """
    Creates the exports requests for media files from AoC.
    """

    @classmethod
    def convert(cls, full_data_set):
        """
        Create all export requests for the dataset.
        """
        AoCMediaSubprocessor.create_graphics_requests(full_data_set)
        AoCMediaSubprocessor.create_sound_requests(full_data_set)
