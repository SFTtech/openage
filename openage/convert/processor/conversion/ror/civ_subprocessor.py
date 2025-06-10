# Copyright 2020-2025 the openage authors. See copying.md for legal info.

"""
Creates patches and modifiers for civs.
"""
from .civ.starting_resources import get_starting_resources


class RoRCivSubprocessor:
    """
    Creates raw API objects for civs in RoR.
    """

    get_starting_resources = staticmethod(get_starting_resources)
