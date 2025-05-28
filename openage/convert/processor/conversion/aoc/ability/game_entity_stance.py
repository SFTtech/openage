# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the GameEntityStance ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def game_entity_stance_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the GameEntityStance ability to a line.

    :param line: Unit/Building line that gets the ability.
    :type line: ...dataformat.converter_object.ConverterObjectGroup
    :returns: The forward reference for the ability.
    :rtype: ...dataformat.forward_ref.ForwardRef
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.GameEntityStance"
    ability_raw_api_object = RawAPIObject(ability_ref,
                                          "GameEntityStance",
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.GameEntityStance")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Stances
    search_range = current_unit["search_radius"].value
    stance_names = ["Aggressive", "Defensive", "StandGround", "Passive"]

    # Attacking is prefered
    ability_preferences = []
    if line.is_projectile_shooter():
        ability_preferences.append(ForwardRef(line, f"{game_entity_name}.Attack"))

    elif line.is_melee() or line.is_ranged():
        if line.has_command(7):
            ability_preferences.append(ForwardRef(line, f"{game_entity_name}.Attack"))

        if line.has_command(105):
            ability_preferences.append(ForwardRef(line, f"{game_entity_name}.Heal"))

    # Units are prefered before buildings
    type_preferences = [
        dataset.pregen_nyan_objects["util.game_entity_type.types.Unit"].get_nyan_object(),
        dataset.pregen_nyan_objects["util.game_entity_type.types.Building"].get_nyan_object(),
    ]

    stances = []
    for stance_name in stance_names:
        stance_api_ref = f"engine.util.game_entity_stance.type.{stance_name}"

        stance_ref = f"{game_entity_name}.GameEntityStance.{stance_name}"
        stance_raw_api_object = RawAPIObject(stance_ref, stance_name, dataset.nyan_api_objects)
        stance_raw_api_object.add_raw_parent(stance_api_ref)
        stance_location = ForwardRef(line, ability_ref)
        stance_raw_api_object.set_location(stance_location)

        # Search range
        stance_raw_api_object.add_raw_member("search_range",
                                             search_range,
                                             "engine.util.game_entity_stance.GameEntityStance")

        # Ability preferences
        stance_raw_api_object.add_raw_member("ability_preference",
                                             ability_preferences,
                                             "engine.util.game_entity_stance.GameEntityStance")

        # Type preferences
        stance_raw_api_object.add_raw_member("type_preference",
                                             type_preferences,
                                             "engine.util.game_entity_stance.GameEntityStance")

        line.add_raw_api_object(stance_raw_api_object)
        stance_forward_ref = ForwardRef(line, stance_ref)
        stances.append(stance_forward_ref)

    ability_raw_api_object.add_raw_member("stances",
                                          stances,
                                          "engine.ability.type.GameEntityStance")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
