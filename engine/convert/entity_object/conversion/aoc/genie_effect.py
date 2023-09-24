# Copyright 2019-2022 the openage authors. See copying.md for legal info.

"""
Contains structures and API-like objects for effects from AoC.
"""

from __future__ import annotations
import typing

from ..converter_object import ConverterObject

if typing.TYPE_CHECKING:
    from engine.convert.entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer
    from engine.convert.value_object.read.value_members import ValueMember


class GenieEffectObject(ConverterObject):
    """
    Single effect contained in GenieEffectBundle.
    """

    __slots__ = ('bundle_id', 'data')

    def __init__(
        self,
        effect_id: int,
        bundle_id: int,
        full_data_set: GenieObjectContainer,
        members: dict[str, ValueMember] = None
    ):
        """
        Creates a new Genie effect object.

        :param effect_id: The index of the effect in the .dat file's effect
        :param bundle_id: The index of the effect bundle that the effect belongs to.
                          (the index is referenced as tech_effect_id by techs)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :param members: An already existing member dict.
        """

        super().__init__(effect_id, members=members)

        self.bundle_id = bundle_id
        self.data = full_data_set

    def get_type(self) -> int:
        """
        Returns the effect's type.
        """
        return self["type_id"].value

    def __repr__(self):
        return f"GenieEffectObject<{self.get_id()}>"


class GenieEffectBundle(ConverterObject):
    """
    A set of effects of a tech.
    """

    __slots__ = ('effects', 'sanitized', 'data')

    def __init__(
        self,
        bundle_id: int,
        effects: list[GenieEffectObject],
        full_data_set: GenieObjectContainer,
        members: dict[str, ValueMember] = None
    ):
        """
        Creates a new Genie effect bundle.

        :param bundle_id: The index of the effect in the .dat file's effect
                          block. (the index is referenced as tech_effect_id by techs)
        :param effects: Effects of the bundle as list of GenieEffectObject.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :param members: An already existing member dict.
        """

        super().__init__(bundle_id, members=members)

        self.effects = effects

        # Sanitized bundles should not contain 'garbage' effects, e.g.
        #     - effects that do nothing
        #     - effects without a type        #
        # Processors should set this to True, once the bundle is sanitized.
        self.sanitized: bool = False

        self.data = full_data_set

    def get_effects(self, effect_type: int = None) -> list[GenieEffectObject]:
        """
        Returns the effects in the bundle, optionally only effects with a specific
        type.

        :param effect_type: Type that the effects should have.
        :type effect_type: int
        :returns: List of matching effects.
        :rtype: list
        """
        if effect_type:
            matching_effects = []
            for effect in self.effects.values():
                if effect.get_type() == effect_type:
                    matching_effects.append(effect)

            return matching_effects

        return list(self.effects.values())

    def is_sanitized(self) -> bool:
        """
        Returns whether the effect bundle has been sanitized.
        """
        return self.sanitized

    def __repr__(self):
        return f"GenieEffectBundle<{self.get_id()}>"
