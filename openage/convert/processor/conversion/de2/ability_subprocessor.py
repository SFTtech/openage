# Copyright 2021-2022 the openage authors. See copying.md for legal info.

"""
Derives and adds abilities to lines. Subroutine of the
nyan subprocessor.
"""

from .ability.regenerate_attribute import regenerate_attribute_ability


class DE2AbilitySubprocessor:
    """
    Creates raw API objects for abilities in DE2.
    """

    regenerate_attribute_ability = staticmethod(regenerate_attribute_ability)
