# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Extract effect objects from the AoC data.
"""
from __future__ import annotations
import typing

from ......entity_object.conversion.aoc.genie_effect import GenieEffectObject, GenieEffectBundle

if typing.TYPE_CHECKING:
    from ......value_object.read.value_members import ArrayMember
    from ......entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def extract_genie_effect_bundles(
    gamespec: ArrayMember,
    full_data_set: GenieObjectContainer
) -> None:
    """
    Extract effects and effect bundles from the game data.

    :param gamespec: Gamedata from empires.dat file.
    :type gamespec: ...dataformat.value_members.ArrayMember
    """
    raw_effect_bundles = gamespec[0]["effect_bundles"].value

    index_bundle = 0
    for raw_effect_bundle in raw_effect_bundles:
        bundle_id = index_bundle

        # call hierarchy: effect_bundle->effects
        raw_effects = raw_effect_bundle["effects"].value

        effects = {}

        index_effect = 0
        for raw_effect in raw_effects:
            effect_id = index_effect
            effect_members = raw_effect.value

            effect = GenieEffectObject(effect_id, bundle_id, full_data_set,
                                       members=effect_members)

            effects.update({effect_id: effect})

            index_effect += 1

        # Pass everything to the bundle
        effect_bundle_members = raw_effect_bundle.value
        # Remove effects we store them as separate objects
        effect_bundle_members.pop("effects")

        bundle = GenieEffectBundle(bundle_id, effects, full_data_set,
                                   members=effect_bundle_members)
        full_data_set.genie_effect_bundles.update({bundle.get_id(): bundle})

        index_bundle += 1


def sanitize_effect_bundles(full_data_set: GenieObjectContainer) -> None:
    """
    Remove garbage data from effect bundles.

    :param full_data_set: GenieObjectContainer instance that
                            contains all relevant data for the conversion
                            process.
    :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
    """
    effect_bundles = full_data_set.genie_effect_bundles

    for bundle in effect_bundles.values():
        sanitized_effects = {}

        effects = bundle.get_effects()

        index = 0
        for effect in effects:
            effect_type = effect["type_id"].value
            if effect_type < 0:
                # Effect has no type
                continue

            if effect_type == 3:
                if effect["attr_b"].value < 0:
                    # Upgrade to invalid unit
                    continue

            if effect_type == 102:
                if effect["attr_d"].value < 0:
                    # Tech disable effect with no tech id specified
                    continue

            sanitized_effects.update({index: effect})
            index += 1

        bundle.effects = sanitized_effects
        bundle.sanitized = True
