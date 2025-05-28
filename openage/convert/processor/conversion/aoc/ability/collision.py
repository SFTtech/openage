# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the Collision ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def collision_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the Collision ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Collision"
    ability_raw_api_object = RawAPIObject(ability_ref, "Collision", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.Collision")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Hitbox object
    hitbox_name = f"{game_entity_name}.Collision.{game_entity_name}Hitbox"
    hitbox_raw_api_object = RawAPIObject(hitbox_name,
                                         f"{game_entity_name}Hitbox",
                                         dataset.nyan_api_objects)
    hitbox_raw_api_object.add_raw_parent("engine.util.hitbox.Hitbox")
    hitbox_location = ForwardRef(line, ability_ref)
    hitbox_raw_api_object.set_location(hitbox_location)

    radius_x = current_unit["radius_x"].value
    radius_y = current_unit["radius_y"].value
    radius_z = current_unit["radius_z"].value

    hitbox_raw_api_object.add_raw_member("radius_x",
                                         radius_x,
                                         "engine.util.hitbox.Hitbox")
    hitbox_raw_api_object.add_raw_member("radius_y",
                                         radius_y,
                                         "engine.util.hitbox.Hitbox")
    hitbox_raw_api_object.add_raw_member("radius_z",
                                         radius_z,
                                         "engine.util.hitbox.Hitbox")

    hitbox_forward_ref = ForwardRef(line, hitbox_name)
    ability_raw_api_object.add_raw_member("hitbox",
                                          hitbox_forward_ref,
                                          "engine.ability.type.Collision")

    line.add_raw_api_object(hitbox_raw_api_object)
    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
